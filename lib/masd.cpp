#include "meubiblioteca.h"

WiFiClient client;
HTTPClient http;

void initSerialWifi() {
  // Código aqui
}

void testing(String teste) {
  // Código aqui
}

void request() {
  // Código aqui
}

void jsonACC() {
  // Código aqui
}

void jsonGIRO() {
  // Código aqui
}

void estados() {
  // Código aqui
}

void setup() {
  initSerialWifi();
  delay(1000);
}

void loop() {
  if(WiFi.status() == WL_CONNECTED) {
    if(millis() >= tempo) {
      request();
      tempo = millis() + intervalo;
    }
    estados();
  }
}
