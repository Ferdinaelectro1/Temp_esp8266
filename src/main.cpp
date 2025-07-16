#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <DHT.h>
#define DHT_PIN 2
#define HUM_PIN A0

const char *ssid = "youpilab_fibre";
const char *password = "i_l@v3_yl2021Fibre";

ESP8266WebServer server;
DHT dht;
float data[3] = {0};

/*   DynamicJsonDocument doc(1024);
  doc["temperature"] = data[0];
  doc["Terre_Humidity"] = data[1];
  doc["Air_Humidity"] = data[2];
  json = serializeJson(doc,json); */

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
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200,"application/json",json);
}

void setup() {
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
  server.begin();
  dht.setup(DHT_PIN,DHT::DHT11);
  server.on("/data",sendData);
}

void loop() {
  server.handleClient();
/*   delay(dht.getMinimumSamplingPeriod());
  //Serial.print("Temperature : ");
  float temperature = dht.getTemperature();
  float humidityAire = dht.getHumidity();
  float humidityTerre =  (analogRead(HUM_PIN)/1023.0)*100.0;
  data[0] = temperature;
  data[1] = humidityTerre;
  data[2] = humidityAire; */
/*   Serial.print(temperature,2);
  Serial.print(" °C");
  Serial.print(" | ");
  Serial.print("Humidité de terre : ");
  Serial.print(humidityTerre);
  Serial.print(" %");
  Serial.print(" | ");
  Serial.print("Humidité de air : ");
  Serial.print(humidityAire);
  Serial.println(" %");
  delay(500); */
}

