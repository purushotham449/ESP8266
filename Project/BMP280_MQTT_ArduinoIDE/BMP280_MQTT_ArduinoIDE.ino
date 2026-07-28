#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_BMP280.h>

// WiFi credentials
const char* ssid = "xxxxxxxx";
const char* password = "123456789";

// MQTT broker settings
const char* mqtt_server = "8ff66edb6259450d8d29c1e80d066941.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "hivemq.webclient.1785169513397";
const char* mqtt_password = "bGk523NLatrLhtzf4p2jNLtHs%xzLC$j";

// I2C pins and MQTT topic
#define SDA_PIN 4
#define SCL_PIN 5
#define TOPIC "home/sensors/bmp280"

WiFiClientSecure espClient;
PubSubClient client(espClient);
Adafruit_BMP280 bmp;

// Connect to the configured WiFi network.
void wifiConnect() {
  Serial.printf("Connecting to WiFi '%s'...\n", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
}

// Reconnect to MQTT broker if the client is disconnected.
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266BMP280", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("; retrying in 2 seconds");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("Initializing BMP280 sensor and MQTT publishing...");

  // Initialize I2C for BMP280 communication.
  Wire.begin(SDA_PIN, SCL_PIN);

  // Try the default BMP280 I2C address first.
  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 not found at 0x76, try 0x77");
    while (1) {
      delay(1000);
    }
  }

  Serial.println("BMP280 sensor initialized successfully");

  wifiConnect();

  // Configure MQTT broker and keep it ready for connection.
  espClient.setInsecure(); // Accept the broker certificate without validation.
  client.setServer(mqtt_server, mqtt_port);
  Serial.print("MQTT broker set to: ");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.println(mqtt_port);
}

void loop() {
  // Ensure WiFi stays connected.
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, reconnecting...");
    wifiConnect();
  }

  // Read data from BMP280 sensor.
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0;
  float altitude = bmp.readAltitude(1013.25);

  Serial.println("Sensor readings:");
  Serial.print("  Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");
  Serial.print("  Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");
  Serial.print("  Altitude: ");
  Serial.print(altitude);
  Serial.println(" m");

  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;
  doc["pressure"] = pressure;
  doc["altitude"] = altitude;

  char payload[200];
  serializeJson(doc, payload);

  Serial.print("Publishing payload to ");
  Serial.print(TOPIC);
  Serial.print(": ");
  Serial.println(payload);

  // Ensure MQTT stays connected.
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  
  bool published = client.publish(TOPIC, payload, true);
  if (published) {
    Serial.println("Publish successful");
  } else {
    Serial.println("Publish failed");
  }

  delay(5000);
}
