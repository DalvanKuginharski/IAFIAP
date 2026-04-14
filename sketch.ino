#include <Arduino.h>
#include <DHT.h>

// ==============================
// FarmTech Solutions - Fase 2
// ESP32 + Wokwi - Irrigação Inteligente para Goiaba
// Simulação didática:
// - N, P e K -> 3 botões (adequado=true / inadequado=false)
// - pH -> LDR (valor analógico mapeado para faixa de pH)
// - Umidade do solo -> DHT22 (proxy didático, embora meça umidade do ar)
// - Bomba d'água -> Relé azul
// - Chuva prevista -> pode ser informada via Serial (integração manual com Python)
// ==============================

#define DHTPIN 15
#define DHTTYPE DHT22

const int PIN_BTN_N = 18;
const int PIN_BTN_P = 19;
const int PIN_BTN_K = 21;
const int PIN_LDR   = 34;   // ADC
const int PIN_RELAY = 23;

DHT dht(DHTPIN, DHTTYPE);

float rainProbability = 0.0;   // % - pode vir do Python via Serial
float rainMmNext12h   = 0.0;   // mm - opcional
bool relayState = false;

unsigned long lastRead = 0;
const unsigned long READ_INTERVAL = 2500;

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float readPhFromLdr() {
  int raw = analogRead(PIN_LDR); // 0..4095
  // Mapeamento didático para aproximar a leitura a uma faixa agronômica utilizável.
  // Ajuste simples: 4.0 até 8.0
  float ph = mapFloat((float)raw, 0.0, 4095.0, 4.0, 8.0);
  return ph;
}

void processSerialCommands() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  cmd.toUpperCase();

  if (cmd.startsWith("CHANCE=")) {
    rainProbability = cmd.substring(7).toFloat();
    Serial.print("[OK] Probabilidade de chuva atualizada para: ");
    Serial.print(rainProbability);
    Serial.println("%");
  } else if (cmd.startsWith("RAINMM=")) {
    rainMmNext12h = cmd.substring(7).toFloat();
    Serial.print("[OK] Chuva prevista nas próximas 12h: ");
    Serial.print(rainMmNext12h);
    Serial.println(" mm");
  } else if (cmd == "STATUS") {
    Serial.println("[INFO] Comandos aceitos:");
    Serial.println("  CHANCE=75   -> define probabilidade de chuva (%)");
    Serial.println("  RAINMM=3.2  -> define chuva prevista em mm");
    Serial.println("  STATUS      -> mostra ajuda");
  } else {
    Serial.println("[ERRO] Comando não reconhecido. Use STATUS para ajuda.");
  }
}

String buildReason(bool nOk, bool pOk, bool kOk, float ph, float humidity, bool rainForecast, int nutrientScore) {
  String reason = "";

  if (humidity < 50.0 && !rainForecast) {
    reason += "Solo muito seco: irrigacao prioritaria. ";
  } else if (humidity >= 50.0 && humidity < 65.0 && !rainForecast) {
    reason += "Solo abaixo da faixa didatica de seguranca. ";
  } else if (humidity >= 65.0) {
    reason += "Umidade suficiente. ";
  }

  if (rainForecast) {
    reason += "Previsao de chuva ativa. ";
  }

  if (ph < 5.5 || ph > 7.0) {
    reason += "pH fora da faixa de referencia. ";
  } else {
    reason += "pH em faixa aceitavel. ";
  }

  reason += "NPK adequados: ";
  reason += String(nutrientScore);
  reason += "/3. ";

  reason += "N=" + String(nOk ? "OK" : "BAIXO");
  reason += " P=" + String(pOk ? "OK" : "BAIXO");
  reason += " K=" + String(kOk ? "OK" : "BAIXO");

  return reason;
}

bool shouldIrrigate(bool nOk, bool pOk, bool kOk, float ph, float humidity) {
  int nutrientScore = (nOk ? 1 : 0) + (pOk ? 1 : 0) + (kOk ? 1 : 0);
  bool phIdeal = (ph >= 5.5 && ph <= 7.0);
  bool rainForecast = (rainProbability >= 60.0) || (rainMmNext12h >= 2.0);

  // Regra 1: solo muito seco -> liga, exceto se houver chuva prevista relevante.
  if (humidity < 50.0 && !rainForecast) {
    return true;
  }

  // Regra 2: solo moderadamente seco -> exige solo quimicamente mais próximo do alvo.
  if (humidity >= 50.0 && humidity < 65.0 && !rainForecast && phIdeal && nutrientScore >= 2) {
    return true;
  }

  return false;
}

void printCsvLine(unsigned long ts, float humidity, float ph, bool nOk, bool pOk, bool kOk, float chance, float rainMm, bool pump) {
  Serial.print("CSV,");
  Serial.print(ts);
  Serial.print(",");
  Serial.print(humidity, 2);
  Serial.print(",");
  Serial.print(ph, 2);
  Serial.print(",");
  Serial.print(nOk ? 1 : 0);
  Serial.print(",");
  Serial.print(pOk ? 1 : 0);
  Serial.print(",");
  Serial.print(kOk ? 1 : 0);
  Serial.print(",");
  Serial.print(chance, 2);
  Serial.print(",");
  Serial.print(rainMm, 2);
  Serial.print(",");
  Serial.println(pump ? 1 : 0);
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(PIN_BTN_N, INPUT_PULLUP);
  pinMode(PIN_BTN_P, INPUT_PULLUP);
  pinMode(PIN_BTN_K, INPUT_PULLUP);
  pinMode(PIN_RELAY, OUTPUT);

  digitalWrite(PIN_RELAY, LOW);

  Serial.println("========================================");
  Serial.println("FarmTech Solutions - Fase 2");
  Serial.println("Sistema de irrigacao inteligente - Goiaba");
  Serial.println("Comandos Serial: CHANCE=75 | RAINMM=3.2 | STATUS");
  Serial.println("Saida CSV: CSV,timestamp,humidity,ph,n,p,k,rainChance,rainMm,pump");
  Serial.println("========================================");
}

void loop() {
  processSerialCommands();

  if (millis() - lastRead < READ_INTERVAL) {
    return;
  }
  lastRead = millis();

  bool nOk = digitalRead(PIN_BTN_N) == LOW;
  bool pOk = digitalRead(PIN_BTN_P) == LOW;
  bool kOk = digitalRead(PIN_BTN_K) == LOW;

  float humidity = dht.readHumidity();
  float ph = readPhFromLdr();

  if (isnan(humidity)) {
    Serial.println("Falha ao ler o DHT22.");
    return;
  }

  int nutrientScore = (nOk ? 1 : 0) + (pOk ? 1 : 0) + (kOk ? 1 : 0);
  bool rainForecast = (rainProbability >= 60.0) || (rainMmNext12h >= 2.0);

  relayState = shouldIrrigate(nOk, pOk, kOk, ph, humidity);
  digitalWrite(PIN_RELAY, relayState ? HIGH : LOW);

  String reason = buildReason(nOk, pOk, kOk, ph, humidity, rainForecast, nutrientScore);

  Serial.println("\n----------- LEITURA ATUAL -----------");
  Serial.print("Umidade (proxy solo / DHT22): "); Serial.print(humidity, 2); Serial.println(" %");
  Serial.print("pH estimado (via LDR): "); Serial.println(ph, 2);
  Serial.print("N: "); Serial.println(nOk ? "adequado" : "baixo");
  Serial.print("P: "); Serial.println(pOk ? "adequado" : "baixo");
  Serial.print("K: "); Serial.println(kOk ? "adequado" : "baixo");
  Serial.print("Chance de chuva: "); Serial.print(rainProbability, 1); Serial.println(" %");
  Serial.print("Chuva prox. 12h: "); Serial.print(rainMmNext12h, 2); Serial.println(" mm");
  Serial.print("Bomba/Relé: "); Serial.println(relayState ? "LIGADO" : "DESLIGADO");
  Serial.print("Justificativa: "); Serial.println(reason);

  printCsvLine(millis(), humidity, ph, nOk, pOk, kOk, rainProbability, rainMmNext12h, relayState);
}
