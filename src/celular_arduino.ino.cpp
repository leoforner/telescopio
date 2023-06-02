# 1 "C:\\Users\\leona\\AppData\\Local\\Temp\\tmpn4nso6ql"
#include <Arduino.h>
# 1 "C:/Users/leona/OneDrive/Documentos/GitHub/telescopio/src/celular_arduino.ino"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


const char* ssid = "POCO X3 NFC";
const char* password = "a1b2c3d4";
String host = "http://192.168.40.201:8080";
float temp_request_N = 0.000000;
char temp_request_S[16] = "0.000000";
int tempo = 0;
int intervalo = 500;
String input;
bool stop = 0;


struct {
  double X;
  double Y;
  double Z;
} primeira_ACC = {0, 0, 9.5}, atual_ACC, final_ACC, primeira_GR = {0,0,0}, atual_GR, final_GR;


double TIME = 0;


double ACX = 0;
double ACY = 0;
double ACZ = 0;



double GRX = 0;
double GRY = 0;
double GRZ = 0;



double correcao[6] = { 0, 0, 0, 0, 0, 0};

enum tipo {
  parado,
  inicial,
  seguindo,
} estado;


WiFiClient client;
HTTPClient http;


void initSerialWifi();
void testing(String teste);
void request();
void jsonACC();
void jsonGIRO();
void estados();
void setup();
void loop();
#line 60 "C:/Users/leona/OneDrive/Documentos/GitHub/telescopio/src/celular_arduino.ino"
void setup() {
  initSerialWifi();
  delay(1000);
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){
    if(millis() >= tempo){
      request();
      tempo = millis() + intervalo;
    }
    estados();
  }
}

void estados(){
  if(stop == 1) {
    enum tipo estado = parado;

  }else if ( (TIME > 10) && (TIME <20) ) {
    enum tipo estado = inicial;

  }else if((TIME > 20 ) && (correcao[0] != 0 || correcao[1] != 0 || correcao[2] != 0 ) ) {
    enum tipo estado = seguindo;

  }else {
    enum tipo estado = parado;
  }
  switch (estado){
    case 1 :
      correcao[0] = atual_ACC.X - primeira_ACC.X;
      correcao[1] = atual_ACC.Y - primeira_ACC.Y;
      correcao[2] = atual_ACC.Z - primeira_ACC.Z;
      correcao[3] = atual_GR.X - primeira_GR.X;
      correcao[4] = atual_GR.Y - primeira_GR.Y;
      correcao[5] = atual_GR.Z - primeira_GR.Z;
      for( int i = 0 ; i < 6 ; i ++){
        if(correcao[i] < 1) correcao[i] = 0;
      }
      atual_ACC.X = ACX; atual_ACC.Y = ACY; atual_ACC.Z = ACZ; atual_GR.X = GRX; atual_GR.Y = GRY; atual_GR.Z = GRZ;
      break;

    case 2 :
      correcao[0] = atual_ACC.X - final_ACC.X;
      correcao[1] = atual_ACC.Y - final_ACC.Y;
      correcao[2] = atual_ACC.Z - final_ACC.Z;
      correcao[3] = atual_GR.X - final_GR.X;
      correcao[4] = atual_GR.Y - final_GR.Y;
      correcao[5] = atual_GR.Z - final_GR.Z;
      for( int i =0 ; i < 6 ; i ++){
        if(correcao[i] < 1) correcao[i] = 0;
      }
      atual_ACC.X = ACX; atual_ACC.Y = ACY; atual_ACC.Z = ACZ; atual_GR.X = GRX; atual_GR.Y = GRY; atual_GR.Z = GRZ;
      break;

    default:
      atual_ACC.X = ACX; atual_ACC.Y = ACY; atual_ACC.Z = ACZ; atual_GR.X = GRX; atual_GR.Y = GRY; atual_GR.Z = GRZ;
      correcao[0] = 0;
      correcao[1] = 0;
      correcao[2] = 0;
      correcao[3] = 0;
      correcao[4] = 0;
      correcao[5] = 0;
      break;

  }
}

void request(){






  String A_url0 = "/get?accX=";
  String A_url1 = "|acc_time&acc_time=";
  String A_url2 = "&accY=";
  String A_url3 = "|acc_time&accZ=";
  String A_url4 = "|acc_time";


  String G_url0 = "/get?gyrX=";
  String G_url1 = "|gyr_time&gyr_time=";
  String G_url2 = "&gyrY=";
  String G_url3 = "|gyr_time&gyrZ=";
  String G_url4 = "|gyr_time";


  sprintf(temp_request_S, "%.6f", temp_request_N);


  String host_A = host + A_url0 + temp_request_S + A_url1 + temp_request_S + A_url2 + temp_request_S + A_url3 + temp_request_S + A_url4;


  String host_G = host + G_url0 + temp_request_S + G_url1 + temp_request_S + G_url2 + temp_request_S + G_url3 + temp_request_S + G_url4;




  http.begin( host_A );
    int httpCode_A = http.GET();

    if (httpCode_A > 0) {

      input = http.getString();



      jsonACC();
    }
    else {
      Serial.println("Error on HTTP request acelerometro");
    }
  http.end();



  http.begin( host_G );
    int httpCode_G = http.GET();

    if (httpCode_G > 0) {

      input = http.getString();



      jsonGIRO();
    }
    else {
      Serial.println("Error on HTTP request giroscopio");
    }
  http.end();


}

void jsonACC(){

  StaticJsonDocument<144> filter;

  JsonObject filter_buffer = filter.createNestedObject("buffer");
  filter_buffer["accX"]["buffer"] = true;
  filter_buffer["acc_time"]["buffer"] = true;
  filter_buffer["accY"]["buffer"] = true;
  filter_buffer["accZ"]["buffer"] = true;

  StaticJsonDocument<3072> doc;

  DeserializationError error = deserializeJson(doc, input, DeserializationOption::Filter(filter));


  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }
# 250 "C:/Users/leona/OneDrive/Documentos/GitHub/telescopio/src/celular_arduino.ino"
  ACX = doc["buffer"]["accX"]["buffer"][0];
  ACY = doc["buffer"]["accY"]["buffer"][0];
  ACZ = doc["buffer"]["accZ"]["buffer"][0];
  TIME = doc["buffer"]["acc_time"]["buffer"][0];

  for(int i = 0; TIME != 0.000000 ; i += 1){
    temp_request_N = TIME;
    TIME = doc["buffer"]["acc_time"]["buffer"][i];
  }

  double TOTAL_ACC = sqrt(sq(ACX) + sq(ACY) + sq(ACZ));

  Serial.print(ACX, 6);
  Serial.print(" ");
  Serial.print(ACY, 6);
  Serial.print(" ");
  Serial.print(ACZ, 6);
  Serial.print(" ");
  Serial.print(TOTAL_ACC, 6);
  Serial.print(" ");


}

void jsonGIRO(){

  StaticJsonDocument<144> filter;

  JsonObject filter_buffer = filter.createNestedObject("buffer");
  filter_buffer["gyrX"]["buffer"] = true;
  filter_buffer["gyr_time"]["buffer"] = true;
  filter_buffer["gyrY"]["buffer"] = true;
  filter_buffer["gyrZ"]["buffer"] = true;

  StaticJsonDocument<3072> doc;

  DeserializationError error = deserializeJson(doc, input, DeserializationOption::Filter(filter));


  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }
# 327 "C:/Users/leona/OneDrive/Documentos/GitHub/telescopio/src/celular_arduino.ino"
  GRX = doc["buffer"]["gyrX"]["buffer"][0];
  GRY = doc["buffer"]["gyrY"]["buffer"][0];
  GRZ = doc["buffer"]["gyrZ"]["buffer"][0];
  TIME = doc["buffer"]["gyr_time"]["buffer"][0];

  for(int i = 0; TIME != 0.000000 ; i += 1){
    temp_request_N = TIME;
    TIME = doc["buffer"]["gyr_time"]["buffer"][i];
  }


  Serial.print(GRX, 6);
  Serial.print(" ");
  Serial.print(GRY, 6);
  Serial.print(" ");
  Serial.print(GRZ, 6);
  Serial.print(" ");


  Serial.println(0);


}

void initSerialWifi(){
  Serial.begin(115200);
  delay(100);


  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
}

void testing(String teste){
  Serial.println("");
  Serial.println("testando");
  Serial.println(teste);
  Serial.println("testando");
  Serial.println("");
}