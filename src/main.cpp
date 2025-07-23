#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#define DHT_PIN 2//d4
#define LED_PIN 5 //d1
#define HUM_PIN A0
#define BUZZER 16
#define TIM1 1000
#define TIM2 200

unsigned int timer1 =  0;
unsigned int timer2  = 0;
unsigned int now1 = TIM1;
unsigned int now2 = TIM2;
const char *ssid = "youpilab_fibre";
const char *password = "i_l@v3_yl2021Fibre";
const String FIREBASE_HOST =  "https://esptemperature-3ec83-default-rtdb.firebaseio.com/";
const String NODE_PATH = "/mesures.json";
const String NODE_PATH2 = "/commandes.json";


//ESP8266WebServer server;
DHT dht;
float data[3] = {0};

void updateData()
{
  delay(dht.getMinimumSamplingPeriod());
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
         
  json.substring(0,4);       
}

//https://esptemperature-3ec83-default-rtdb.firebaseio.com/

void sendToFirebase()
{
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
    //Serial.println(http.getString());  // réponse Firebase
  } else {
    Serial.printf("Erreur HTTP : %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void getTofirebase()
{
    HTTPClient http;
    WiFiClientSecure client;
    client.setInsecure();
    http.begin(client,FIREBASE_HOST + NODE_PATH2);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc,payload);
      if(error)
      {
        Serial.println("Erreur de DeserializationError");
      }
      else
      {
        Serial.println(String(doc["ledState"]));
        digitalWrite(LED_PIN,String(doc["ledState"]) == "true" ? true : false);
      }
      //Serial.println("Reçu de Firebase : " + payload);
    } else {
      Serial.printf("Erreur HTTP : %d\n", httpCode);
    }

    http.end();
}

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
  pinMode(LED_PIN,OUTPUT);
}

//IL Y A 2 THREADS que j'ai créé dans les deux if .
//le threads 1 pour envoyer les données de temp et hum dans firebase 
//le threads 2 pour lire l'état de la commande d'allumage de la led dans firebase
void loop() {
  if(now1 - timer1 > TIM1)
  {
    sendToFirebase();
    now1 = 0;
    timer1 = 0;
  }
  if(now2 - timer2 > TIM2)
  {
    getTofirebase();
    timer2 = 0;
    now2 = 0;
  }
  delay(1);
  now1 ++;
  now2 ++;
}