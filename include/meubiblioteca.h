#ifndef MINHABIBLIOTECA_H
#define MINHABIBLIOTECA_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

extern const char* ssid;
extern const char* password;
extern String host;
extern float temp_request_N;
extern char temp_request_S[16];
extern int tempo;
extern int intervalo;
extern String input;
extern bool stop;

struct {
  double X;
  double Y;
  double Z;
} primeira_ACC, atual_ACC, final_ACC, primeira_GR, atual_GR, final_GR;

double TIME;

double ACX;
double ACY;
double ACZ;

double GRX;
double GRY;
double GRZ;

double correcao[6];

enum tipo {
  parado,
  inicial,
  seguindo,
} estado;

void initSerialWifi();
void testing(String teste);
void request();
void jsonACC();
void jsonGIRO();
void estados();

#endif
