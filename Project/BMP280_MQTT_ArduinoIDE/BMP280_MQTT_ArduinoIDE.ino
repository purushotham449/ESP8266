#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_BMP280.h>

const char* ssid="YOUR_WIFI";
const char* password="YOUR_PASSWORD";
const char* mqtt_server="broker.hivemq.com";
const int mqtt_port=1883;
const char* mqtt_user="";
const char* mqtt_password="";

#define SDA_PIN D2
#define SCL_PIN D1
#define TOPIC "home/sensors/bmp280"

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_BMP280 bmp;

void wifiConnect(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){delay(500);}
}

void reconnect(){
  while(!client.connected()){
    if(client.connect("ESP8266BMP280",mqtt_user,mqtt_password)){}
    else delay(2000);
  }
}

void setup(){
  Serial.begin(115200);
  Wire.begin(SDA_PIN,SCL_PIN);
  if(!bmp.begin(0x76)){Serial.println("Try 0x77"); while(1);}
  wifiConnect();
  client.setServer(mqtt_server,mqtt_port);
}

void loop(){
  if(WiFi.status()!=WL_CONNECTED) wifiConnect();
  if(!client.connected()) reconnect();
  client.loop();

  StaticJsonDocument<200> doc;
  doc["temperature"]=bmp.readTemperature();
  doc["pressure"]=bmp.readPressure()/100.0;
  doc["altitude"]=bmp.readAltitude(1013.25);

  char payload[200];
  serializeJson(doc,payload);
  Serial.println(payload);
  client.publish(TOPIC,payload,true);
  delay(5000);
}
