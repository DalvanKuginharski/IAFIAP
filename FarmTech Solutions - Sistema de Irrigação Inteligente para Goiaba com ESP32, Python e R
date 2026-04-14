# FarmTech Solutions - Fase 2

## Sistema de Irrigação Inteligente para Goiaba com ESP32, Python e R

Este projeto foi desenvolvido na **Fase 2** da PBL com foco em agricultura digital, automação e análise de dados. A proposta consiste na construção de um sistema de irrigação inteligente para uma fazenda de goiaba localizada em **Ouro Verde, Carlópolis/PR**, utilizando o **ESP32 no Wokwi**, integração climática em **Python** e apoio à decisão estatística em **R**.

---

## 1. Objetivo do projeto

Desenvolver um sistema automatizado capaz de monitorar variáveis simuladas do solo e do ambiente para decidir se a irrigação deve ser acionada ou não.

O sistema considera:

- **Nitrogênio (N)** simulado por um botão
- **Fósforo (P)** simulado por um botão
- **Potássio (K)** simulado por um botão
- **pH do solo** simulado por um sensor **LDR**
- **Umidade do solo** simulada por um **DHT22**
- **Bomba d’água** simulada por um **relé azul**
- **Previsão de chuva** obtida por API meteorológica e informada ao ESP32

---

## 2. Contexto da solução

Como o Wokwi não possui sensores agrícolas específicos para NPK, pH e umidade do solo, foram utilizadas substituições didáticas para viabilizar a simulação:

- **3 botões verdes** representam os níveis de **N, P e K**
- **LDR** representa o **pH do solo**
- **DHT22** representa a **umidade do solo**
- **Relé azul** representa o acionamento da **bomba de irrigação**

> **Importante:** trata-se de uma simulação acadêmica. Os sensores utilizados não medem diretamente NPK, pH ou umidade do solo real, mas foram adaptados para fins didáticos e de lógica computacional.

---

## 3. Cultura escolhida

A cultura agrícola adotada neste projeto é a **goiaba**, considerando o contexto regional de **Carlópolis/PR** e a relevância da produção local.

---

## 4. Lógica de funcionamento do sistema

O sistema avalia simultaneamente:

- umidade
- condição química simulada do solo
- equilíbrio dos nutrientes N, P e K
- previsão de chuva

### Regras implementadas

A bomba de irrigação é **ligada** quando:

1. a umidade estiver **abaixo de 50%** e **não houver previsão relevante de chuva**;

ou

2. a umidade estiver entre **50% e 65%**, sem previsão de chuva significativa, com:
   - **pH estimado entre 5.5 e 7.0**
   - pelo menos **2 dos 3 nutrientes (N, P e K) em condição adequada**

A bomba é **desligada** quando:

- houver previsão relevante de chuva;
- a umidade estiver adequada;
- o pH estiver fora da faixa mínima definida para a simulação;
- a condição geral do solo indicar que a irrigação não é necessária naquele momento.

> Os limiares adotados são didáticos, ajustados para a simulação no Wokwi e inspirados em referências técnicas de manejo agrícola.

---

## 5. Componentes utilizados no Wokwi

- 1x **ESP32 DevKit**
- 1x **DHT22**
- 1x **LDR**
- 1x **Resistor 10kΩ**
- 3x **Pushbuttons** (N, P e K)
- 1x **Módulo relé**
- Jumpers de conexão

---

## 6. Ligações do circuito

### Mapeamento dos pinos

- **DHT22 DATA** -> GPIO 15
- **Botão N** -> GPIO 18
- **Botão P** -> GPIO 19
- **Botão K** -> GPIO 21
- **LDR analógico** -> GPIO 34
- **Relé** -> GPIO 23

### Alimentação

- DHT22 -> 3V3
- LDR -> 3V3
- Relé -> 5V
- Todos os componentes compartilham o mesmo **GND**

---

## 7. Estrutura do repositório

```text
IAFIAP/
├── README.md
├── esp32/
│   └── sketch.ino
├── wokwi/
│   └── diagram.json
├── python/
│   └── weather_fetch.py
├── r/
│   └── decision_stats.R
├── images/
│   ├── circuito-completo.png
│   └── serial-monitor.png
└── serial_log.csv
