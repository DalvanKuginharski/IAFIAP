"""
FarmTech Solutions - Fase 2
Busca previsão meteorológica em API pública (Open-Meteo) e gera
valores fáceis de transferir para o ESP32 no Wokwi via Monitor Serial.

Exemplo de uso:
python weather_fetch.py

Saída esperada:
CHANCE=72
RAINMM=3.4
"""

from __future__ import annotations

import json
import urllib.parse
import urllib.request


def fetch_weather(latitude: float = -23.18, longitude: float = -49.68) -> dict:
    # Coordenadas aproximadas usadas como exemplo para Carlópolis/PR.
    params = {
        "latitude": latitude,
        "longitude": longitude,
        "hourly": "precipitation_probability,precipitation",
        "forecast_days": 1,
        "timezone": "auto",
    }
    url = "https://api.open-meteo.com/v1/forecast?" + urllib.parse.urlencode(params)

    with urllib.request.urlopen(url, timeout=20) as response:
        return json.loads(response.read().decode("utf-8"))


def summarize_for_esp32(data: dict) -> tuple[float, float]:
    probs = data["hourly"]["precipitation_probability"][:12]
    rains = data["hourly"]["precipitation"][:12]
    max_prob = max(probs) if probs else 0.0
    total_rain = round(sum(rains), 2) if rains else 0.0
    return float(max_prob), float(total_rain)


def main() -> None:
    data = fetch_weather()
    chance, rain_mm = summarize_for_esp32(data)

    print("=== PREVISÃO METEOROLÓGICA PARA USO NO WOKWI ===")
    print(f"Chance máxima de chuva nas próximas 12h: {chance:.0f}%")
    print(f"Chuva acumulada prevista nas próximas 12h: {rain_mm:.2f} mm")
    print("\nDigite estes comandos no Monitor Serial do ESP32:")
    print(f"CHANCE={chance:.0f}")
    print(f"RAINMM={rain_mm:.2f}")


if __name__ == "__main__":
    main()
