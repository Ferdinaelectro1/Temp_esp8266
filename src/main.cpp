#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#define DHT_PIN 2
#define HUM_PIN A0
#define BUZZER 16

const char *ssid = "youpilab_fibre";
const char *password = "i_l@v3_yl2021Fibre";
const String FIREBASE_HOST =  "https://esptemperature-3ec83-default-rtdb.firebaseio.com/";
const String NODE_PATH = "/mesures.json";

//ESP8266WebServer server;
DHT dht;
float data[3] = {0};

void updateData()
{
  delay(dht.getMinimumSamplingPeriod());
  //Serial.print("Temperature : ");
  float temperature = dht.getTemperature();
  float humidityAire = dht.getHumidity();
  float humidityTerre =  (analogRead(HUM_PIN)/1023.0)*100.0;
  data[0] = temperature;
  data[1] = humidityTerre;
  data[2] = humidityAire;  
}

void sendData()
{  
  updateData();
  String json;
  json = "{\"dht_temp\":" + String(data[0], 2) +
         ",\"soil_hum\":" + String(data[1], 2) +
         ",\"dht_hum\":" + String(data[2], 2) + "}";
  //Serial.println(json);
  //server.sendHeader("Access-Control-Allow-Origin", "*");
  //server.send(200,"application/json",json);
}

//https://esptemperature-3ec83-default-rtdb.firebaseio.com/

void setup() {
  pinMode(BUZZER,OUTPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connexion...");
  }
  Serial.print("Ip : ");
  Serial.println(WiFi.localIP());
  dht.setup(DHT_PIN,DHT::DHT11);

}

void loop() {
  tone(BUZZER,1000,2000);
  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure();
  updateData();
  String json = "{\"dht_temp\":"+String(data[0])+", \"soil_hum\": "+String(data[1])+",\"dht_hum\" :"+String(data[2])+"}";
  http.begin(client,FIREBASE_HOST + NODE_PATH);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.PUT(json);
  if (httpCode > 0) {
    Serial.printf("Code HTTP : %d\n", httpCode);
    Serial.println(http.getString());  // réponse Firebase
  } else {
    Serial.printf("Erreur HTTP : %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  delay(1000);
}

