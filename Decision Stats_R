# FarmTech Solutions - Fase 2
# Analise estatistica simples em R para apoiar a decisao de irrigacao.
# Entrada: CSV gerado a partir das linhas "CSV,..." do Serial Monitor.
# Colunas esperadas:
# timestamp,humidity,ph,n,p,k,rainChance,rainMm,pump

arquivo <- "serial_log.csv"

cat("=== FarmTech Solutions | Analise Estatistica em R ===\n")

if (!file.exists(arquivo)) {
  cat("Arquivo serial_log.csv nao encontrado.\n")
  cat("Crie o CSV com cabecalho:\n")
  cat("timestamp,humidity,ph,n,p,k,rainChance,rainMm,pump\n")
  quit(save = "no")
}

dados <- read.csv(arquivo, stringsAsFactors = FALSE)

if (nrow(dados) < 3) {
  cat("Poucos dados para analise. Colete pelo menos 3 leituras.\n")
  quit(save = "no")
}

ultima <- dados[nrow(dados), ]
media_umidade <- mean(dados$humidity)
sd_umidade <- sd(dados$humidity)
media_ph <- mean(dados$ph)
sd_ph <- sd(dados$ph)

# Z-score da ultima leitura em relacao ao historico.
z_umidade <- ifelse(sd_umidade == 0, 0, (ultima$humidity - media_umidade) / sd_umidade)
z_ph <- ifelse(sd_ph == 0, 0, (ultima$ph - media_ph) / sd_ph)

nutrient_score <- ultima$n + ultima$p + ultima$k
ph_ok <- ultima$ph >= 5.5 & ultima$ph <= 7.0
rain_ok <- ultima$rainChance < 60 & ultima$rainMm < 2.0

# Regra estatistica/heuristica didatica:
# - Ligar se a umidade atual estiver abaixo da media historica
#   e tambem pelo menos 0.5 desvio abaixo do comportamento medio
# - E nao houver chuva relevante prevista
# - E o pH estiver em faixa aceitavel OU a umidade estiver criticamente baixa (< 50)
ligar <- FALSE
motivo <- c()

if (ultima$humidity < 50 && rain_ok) {
  ligar <- TRUE
  motivo <- c(motivo, "umidade criticamente baixa (<50%)")
} else if (ultima$humidity < media_umidade && z_umidade <= -0.5 && rain_ok && ph_ok && nutrient_score >= 2) {
  ligar <- TRUE
  motivo <- c(motivo, "umidade abaixo da media historica com z-score <= -0.5")
}

if (!rain_ok) motivo <- c(motivo, "previsao de chuva relevante")
if (!ph_ok) motivo <- c(motivo, "pH fora da faixa 5.5-7.0")
if (nutrient_score < 2) motivo <- c(motivo, "menos de 2 nutrientes em nivel adequado")

cat("\n--- RESUMO ESTATISTICO ---\n")
cat("Leituras analisadas:", nrow(dados), "\n")
cat("Media da umidade:", round(media_umidade, 2), "\n")
cat("Desvio-padrao da umidade:", round(sd_umidade, 2), "\n")
cat("Media do pH:", round(media_ph, 2), "\n")
cat("Desvio-padrao do pH:", round(sd_ph, 2), "\n")
cat("Z-score da ultima umidade:", round(z_umidade, 2), "\n")
cat("Z-score do ultimo pH:", round(z_ph, 2), "\n")

cat("\n--- ULTIMA LEITURA ---\n")
print(ultima)

cat("\n--- DECISAO FINAL ---\n")
if (ligar) {
  cat("LIGAR BOMBA\n")
} else {
  cat("MANTER BOMBA DESLIGADA\n")
}

if (length(motivo) > 0) {
  cat("Motivos:\n")
  for (m in unique(motivo)) cat("-", m, "\n")
}
