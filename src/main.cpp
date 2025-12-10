#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <math.h>

// -------------------- PINOS --------------------
const int PINO_TEMP = 34;  // NTC 10k (sensor de temperatura)
// -------------------- CONFIGURAÇÃO DO NTC --------------------
// Termistor NTC 10k padrão (BETA 3950)
const float BETA    = 3950.0;
const float R0      = 10000.0;   // 10k ohms a 25°C
const float T0_K    = 298.15;    // 25°C em Kelvin (25 + 273.15)
const float R_FIXED = 10000.0;   // resistor de 10k do divisor

//const char* AP_SSID = "ESP32-SENSORES";
//const char* AP_PASS = "12345678"; 

WebServer server(80);

// -------------------- FUNÇÕES DOS SENSORES --------------------
int leituraMediaADC(int pino, int nAmostras = 10) {
  long soma = 0;
  for (int i = 0; i < nAmostras; i++) {
    soma += analogRead(pino);
    delay(2);
  }
  return soma / nAmostras;
}

float adcParaResistencia(int adc) {
  float v = adc / 4095.0f;        // ADC 12 bits (0 a 4095)
  if (v <= 0.0f) v = 0.0001f;     // evita divisão por zero
  return R_FIXED * (1.0f / v - 1.0f);
}

float resistenciaParaCelsius(float R) {
  float invT = (1.0f / T0_K) + (1.0f / BETA) * log(R / R0);
  float T = 1.0f / invT;      // Kelvin
  return T - 273.15f;         // Kelvin → Celsius
}

// -------------------- HANDLERS HTTP --------------------
void handleRoot() {
  if (!LittleFS.exists("/dashboard.html")) {
    server.send(500, "text/plain", "Erro: /dashboard.html nao encontrado no LittleFS");
    return;
  }

  File file = LittleFS.open("/dashboard.html", "r");
  if (!file) {
    server.send(500, "text/plain", "Erro ao abrir /dashboard.html");
    return;
  }

  server.streamFile(file, "text/html");
  file.close();
}

void handleChartJs() {
  if (!LittleFS.exists("/chart.umd.min.js")) {
    server.send(500, "text/plain", "Erro: /chart.umd.min.js nao encontrado no LittleFS");
    return;
  }

  File file = LittleFS.open("/chart.umd.min.js", "r");
  if (!file) {
    server.send(500, "text/plain", "Erro ao abrir /chart.umd.min.js");
    return;
  }

  // Tipo MIME para JavaScript
  server.streamFile(file, "application/javascript");
  file.close();
}


void handleDados() {
  // Lê sensores quando o cliente pedir os dados
  int adcTemp = leituraMediaADC(PINO_TEMP);
  float Rntc = adcParaResistencia(adcTemp);
  float tempC = resistenciaParaCelsius(Rntc);

  String json = "{";
  json += "\"temp\":";
  json += String(tempC, 2);

  json += "}";

  server.send(200, "application/json", json);
}

void handleNotFound() {
  server.send(404, "text/plain", "404 - Not found");
}

// -------------------- WIFI --------------------
const char* WIFI_SSID = "Nome Da Rede";       
const char* WIFI_PASS = "Senha da rede";      

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  pinMode(PINO_TEMP, INPUT);

  Serial.println();
  Serial.println("Iniciando LittleFS...");

  if (!LittleFS.begin(true)) {
    Serial.println("Falha ao montar LittleFS!");
  } else {
    Serial.println("LittleFS OK.");
  }

  Serial.println("Conectando ao WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Espera conectar
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP na rede local: ");
  Serial.println(WiFi.localIP());   // <<< ESTE IP será usado no ngrok

  // Rotas HTTP
  server.on("/", handleRoot);
  server.on("/dados", handleDados);
  server.on("/chart.umd.min.js", handleChartJs);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Servidor HTTP iniciado!");
}

// -------------------- LOOP --------------------
void loop() {
  server.handleClient();
}
