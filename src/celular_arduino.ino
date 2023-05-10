#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//  variaveis
const char* ssid     = "POCO X3 NFC";
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

// Variáveis do JSON ACC
double ACX = 0;
double ACY = 0;
double ACZ = 0;


// Variáveis do JSON GIRO
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

//   prototipos
void initSerialWifi();
void testing(String teste);
void request();
void jsonACC();
void jsonGIRO();
void estados();


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

  }else if((TIME > 20 ) && (correcao[0] != 0 || correcao[1] != 0 || correcao[2] != 0  ) ) {
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
  // Criando a URL da requisição
  /*     ///// Base
  //String url = "http://192.168.40.201:8080/get?accX=full&acc_time=full&accY=full&accZ=full";
  //String url = "/get?accX=1.00000000|acc_time&acc_time=1.00000000&accY=1.00000000|acc_time&accZ=1.00000000|acc_time";
  */
  //partes url aceleração 
  String A_url0 = "/get?accX=";
  String A_url1 = "|acc_time&acc_time=";
  String A_url2 = "&accY=";
  String A_url3 = "|acc_time&accZ=";
  String A_url4 = "|acc_time";

  //partes url giroscópio 
  String G_url0 = "/get?gyrX=";
  String G_url1 = "|gyr_time&gyr_time=";
  String G_url2 = "&gyrY=";
  String G_url3 = "|gyr_time&gyrZ=";
  String G_url4 = "|gyr_time";

  // tempo enviado na requisição
  sprintf(temp_request_S, "%.6f", temp_request_N);  

  //formando a url da aceleração
  String host_A = host + A_url0 + temp_request_S + A_url1 + temp_request_S + A_url2 + temp_request_S + A_url3 + temp_request_S + A_url4;

   //formando a url do giroscópio
  String host_G = host + G_url0 + temp_request_S + G_url1 + temp_request_S + G_url2 + temp_request_S + G_url3 + temp_request_S + G_url4;


  // manda a requisição da aceleração
    
  http.begin( host_A ); //Specify the URL
    int httpCode_A = http.GET();               //Make the request
  
    if (httpCode_A > 0) { //Check for the returning code
      
      input = http.getString();
      //Serial.println();
      //Serial.println(input);
    
      jsonACC();
    }
    else {
      Serial.println("Error on HTTP request acelerometro");
    }
  http.end(); //Free the resources

    // manda a requisição do giroscópio
    
  http.begin( host_G ); //Specify the URL
    int httpCode_G = http.GET();           //Make the request
  
    if (httpCode_G > 0) { //Check for the returning code
      
      input = http.getString();
      //Serial.println();
      //Serial.println(input);
    
      jsonGIRO();
    }
    else {
      Serial.println("Error on HTTP request giroscopio");
    }
  http.end(); //Free the resources
     
  
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
  // "accX", "acc_time", "accY", "accZ"

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  
  /*
  for (JsonPair buffer_item : doc["buffer"].as<JsonObject>()) {
    const char* buffer_item_key = buffer_item.key().c_str(); // "accX", "acc_time", "accY", "accZ"

    JsonArray buffer_item_value_buffer = buffer_item.value()["buffer"];
    double buffer_item_value_buffer_0 = buffer_item_value_buffer[0]; // 0.2041505, 12.747389, -0.0018234253, ...
    
    double buffer_item_value_buffer_1 = buffer_item_value_buffer[1]; // 0.19064736, 12.752367, -0.012022257, ...
    double buffer_item_value_buffer_2 = buffer_item_value_buffer[2]; // 0.10417209, 12.757343, 0.018059969, ...
    double buffer_item_value_buffer_3 = buffer_item_value_buffer[3]; // 0.059488017, 12.762318, ...
    double buffer_item_value_buffer_4 = buffer_item_value_buffer[4]; // 0.1432063, 12.767293, -0.015189886, ...
    double buffer_item_value_buffer_5 = buffer_item_value_buffer[5]; // -0.04597757, 12.772269, 0.029649019, ...
    double buffer_item_value_buffer_6 = buffer_item_value_buffer[6]; // -0.52668536, 12.777244, ...
    double buffer_item_value_buffer_7 = buffer_item_value_buffer[7]; // -0.67463917, 12.782212, ...
    double buffer_item_value_buffer_8 = buffer_item_value_buffer[8]; // -0.47140771, 12.787188, ...
    double buffer_item_value_buffer_9 = buffer_item_value_buffer[9]; // -0.22775066, 12.792163, 0.02148366, ...
    double buffer_item_value_buffer_10 = buffer_item_value_buffer[10]; // 0.0281711, 12.797138, 0.019434929, ...
    double buffer_item_value_buffer_11 = buffer_item_value_buffer[11]; // 0.25115514, 12.802114, ...
    double buffer_item_value_buffer_12 = buffer_item_value_buffer[12]; // 0.17551692, 12.807089, ...
    double buffer_item_value_buffer_13 = buffer_item_value_buffer[13]; // 0.063234299, 12.812062, ...
    double buffer_item_value_buffer_14 = buffer_item_value_buffer[14]; // 0.056985687, 12.817037, ...
    double buffer_item_value_buffer_15 = buffer_item_value_buffer[15]; // 0.2080197, 12.822012, 0.023874044, ...
    double buffer_item_value_buffer_16 = buffer_item_value_buffer[16]; // 0.19668895, 12.826988, ...
    double buffer_item_value_buffer_17 = buffer_item_value_buffer[17]; // 0.11240172, 12.831963, ...
    double buffer_item_value_buffer_18 = buffer_item_value_buffer[18]; // 0.095943652, 12.836939, ...
    double buffer_item_value_buffer_54 = buffer_item_value_buffer[54]; // 0.095943652, 12.836939, ...
    double valor = buffer_item_value_buffer_1;
    Serial.println(valor, 6);
  }
  */

  ACX = doc["buffer"]["accX"]["buffer"][0];
  ACY = doc["buffer"]["accY"]["buffer"][0];
  ACZ = doc["buffer"]["accZ"]["buffer"][0];
  TIME = doc["buffer"]["acc_time"]["buffer"][0];
  
  for(int i = 0; TIME != 0.000000 ; i += 1){
    temp_request_N = TIME;
    TIME  = doc["buffer"]["acc_time"]["buffer"][i];
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
  // "gyrX", "gyr_time", "gyrY", "gyrZ"

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  
  /*
  for (JsonPair buffer_item : doc["buffer"].as<JsonObject>()) {
    const char* buffer_item_key = buffer_item.key().c_str(); // "gyrX", "gyr_time", "gyrY", "gyrZ"

    JsonArray buffer_item_value_buffer = buffer_item.value()["buffer"];
    double buffer_item_value_buffer_0 = buffer_item_value_buffer[0]; // 0.2041505, 12.747389, -0.0018234253, ...
    
    double buffer_item_value_buffer_1 = buffer_item_value_buffer[1]; // 0.19064736, 12.752367, -0.012022257, ...
    double buffer_item_value_buffer_2 = buffer_item_value_buffer[2]; // 0.10417209, 12.757343, 0.018059969, ...
    double buffer_item_value_buffer_3 = buffer_item_value_buffer[3]; // 0.059488017, 12.762318, ...
    double buffer_item_value_buffer_4 = buffer_item_value_buffer[4]; // 0.1432063, 12.767293, -0.015189886, ...
    double buffer_item_value_buffer_5 = buffer_item_value_buffer[5]; // -0.04597757, 12.772269, 0.029649019, ...
    double buffer_item_value_buffer_6 = buffer_item_value_buffer[6]; // -0.52668536, 12.777244, ...
    double buffer_item_value_buffer_7 = buffer_item_value_buffer[7]; // -0.67463917, 12.782212, ...
    double buffer_item_value_buffer_8 = buffer_item_value_buffer[8]; // -0.47140771, 12.787188, ...
    double buffer_item_value_buffer_9 = buffer_item_value_buffer[9]; // -0.22775066, 12.792163, 0.02148366, ...
    double buffer_item_value_buffer_10 = buffer_item_value_buffer[10]; // 0.0281711, 12.797138, 0.019434929, ...
    double buffer_item_value_buffer_11 = buffer_item_value_buffer[11]; // 0.25115514, 12.802114, ...
    double buffer_item_value_buffer_12 = buffer_item_value_buffer[12]; // 0.17551692, 12.807089, ...
    double buffer_item_value_buffer_13 = buffer_item_value_buffer[13]; // 0.063234299, 12.812062, ...
    double buffer_item_value_buffer_14 = buffer_item_value_buffer[14]; // 0.056985687, 12.817037, ...
    double buffer_item_value_buffer_15 = buffer_item_value_buffer[15]; // 0.2080197, 12.822012, 0.023874044, ...
    double buffer_item_value_buffer_16 = buffer_item_value_buffer[16]; // 0.19668895, 12.826988, ...
    double buffer_item_value_buffer_17 = buffer_item_value_buffer[17]; // 0.11240172, 12.831963, ...
    double buffer_item_value_buffer_18 = buffer_item_value_buffer[18]; // 0.095943652, 12.836939, ...
    double buffer_item_value_buffer_54 = buffer_item_value_buffer[54]; // 0.095943652, 12.836939, ...
    double valor = buffer_item_value_buffer_1;
    Serial.println(valor, 6);
  }
  */

  GRX = doc["buffer"]["gyrX"]["buffer"][0];
  GRY = doc["buffer"]["gyrY"]["buffer"][0];
  GRZ = doc["buffer"]["gyrZ"]["buffer"][0];
  TIME = doc["buffer"]["gyr_time"]["buffer"][0];
  
  for(int i = 0; TIME != 0.000000 ; i += 1){
    temp_request_N = TIME;
    TIME  = doc["buffer"]["gyr_time"]["buffer"][i];
  }

   
  Serial.print(GRX, 6);
  Serial.print(" ");  
  Serial.print(GRY, 6);  
  Serial.print(" "); 
  Serial.print(GRZ, 6);
  Serial.print(" "); 

  //Serial.print(TIME, 6);
  Serial.println(0);  
  
    
}

void initSerialWifi(){
  Serial.begin(115200);
  delay(100);

  // We start by connecting to a WiFi network
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


