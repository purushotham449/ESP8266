/*
  SmartWeatherStation.ino
  ESP8266 + BMP280 + SSD1306 OLED + MQTT

  Replace WiFi and MQTT credentials before uploading.
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SDA_PIN 4
#define SCL_PIN 5
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C
#define BMP280_ADD 0x76
#define MQTT_TOPIC "home/weather/bmp280"

// Hardware driver objects for the BMP280 sensor and 128x64 OLED display.
Adafruit_BMP280 bmp;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// HiveMQ Cloud uses TLS on port 8883, so PubSubClient needs WiFiClientSecure.
WiFiClientSecure espClient;
PubSubClient client(espClient);

// WiFi network credentials.
const char* ssid = "xxxxxxxxxxxxx";
const char* password = "1234567890";

// MQTT broker credentials and endpoint.
const char* mqtt_server = "8ff66edb6259450d8d29c1e80d066941.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "hivemq.webclient.1785169513397";
const char* mqtt_password = "bGk523NLatrLhtzf4p2jNLtHs%xzLC$j";

// Show a short status message on the OLED during startup and reconnects.
void showMessage(const char* line1, const char* line2 = "") {
  Serial.print("[OLED] ");
  Serial.print(line1);
  if (line2[0] != '\0') {
    Serial.print(" - ");
    Serial.print(line2);
  }
  Serial.println();

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(line1);
  display.println(line2);
  display.display();
}

// Connect to WiFi and wait here until the ESP8266 receives an IP address.
void connectWiFi() {
  Serial.println();
  Serial.println("[WiFi] Starting connection");
  Serial.printf("Connecting to WiFi '%s'", ssid);
  showMessage("Connecting WiFi", ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("[WiFi] Connected, IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("[WiFi] Signal strength RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  showMessage("WiFi connected", WiFi.localIP().toString().c_str());
}

// Keep trying MQTT until the broker accepts the username/password.
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.println();
    Serial.print("[MQTT] Connecting to ");
    Serial.print(mqtt_server);
    Serial.print(":");
    Serial.println(mqtt_port);
    showMessage("Connecting MQTT", mqtt_server);

    if (client.connect("ESP8266Weather", mqtt_user, mqtt_password)) {
      Serial.println("[MQTT] Connected");
      Serial.print("[MQTT] Publishing topic: ");
      Serial.println(MQTT_TOPIC);
      showMessage("MQTT connected", MQTT_TOPIC);
    } else {
      Serial.print("[MQTT] Connection failed, client state: ");
      Serial.print(client.state());
      Serial.println(" - retrying in 2 seconds");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println("=================================");
  Serial.println("ESP8266 Smart Weather Station");
  Serial.println("=================================");
  Serial.println("[Setup] Serial monitor ready at 115200 baud");
  Serial.println("[Setup] Initializing I2C on SDA=4(D2), SCL=5(D1)");
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("[Setup] Initializing OLED display at address 0x3C");
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("[Error] SSD1306 OLED not found at 0x3C");
    while (true) {
      delay(1000);
    }
  }

  showMessage("Smart Weather", "Starting...");

  Serial.println("[Setup] Initializing BMP280 sensor at address 0x76");
  if (!bmp.begin(BMP280_ADD)) {
    Serial.println("[Error] BMP280 not found at 0x76");
    showMessage("BMP280 missing", "Check wiring/address");
    while (true) {
      delay(1000);
    }
  }
  Serial.println("[Setup] BMP280 initialized successfully");

  connectWiFi();

  // setInsecure() skips certificate validation. It is simple for classroom/demo use.
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  Serial.println("[Setup] MQTT server configured");
  Serial.println("[Setup] Entering main loop");
}

void loop() {
  // Reconnect WiFi first because MQTT depends on the WiFi connection.
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] Disconnected");
    connectWiFi();
  }

  // Reconnect MQTT before publishing sensor readings.
  if (!client.connected()) {
    Serial.println("[MQTT] Disconnected");
    reconnectMQTT();
  }

  // Let PubSubClient process MQTT keep-alive packets.
  client.loop();

  // Read the sensor values. BMP280 returns temperature in C and pressure in Pa.
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0;
  float altitude = bmp.readAltitude(1013.25);

  Serial.println();
  Serial.println("[Sensor] New BMP280 reading");
  Serial.print("[Sensor] Temperature: ");
  Serial.print(temperature, 1);
  Serial.println(" C");
  Serial.print("[Sensor] Pressure: ");
  Serial.print(pressure, 1);
  Serial.println(" hPa");
  Serial.print("[Sensor] Altitude: ");
  Serial.print(altitude, 1);
  Serial.println(" m");

  // Display larger text using short labels so all three rows fit on 128x64.
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.printf("T:%.1fC", temperature);
  display.setCursor(0, 22);
  display.printf("P:%.0fhPa", pressure);
  display.setCursor(0, 44);
  display.printf("A:%.0fm", altitude);
  display.display();

  // Build a compact JSON payload without needing ArduinoJson.
  String payload = "{";
  payload += "\"temperature\":";
  payload += String(temperature, 1);
  payload += ",\"pressure\":";
  payload += String(pressure, 1);
  payload += ",\"altitude\":";
  payload += String(altitude, 1);
  payload += "}";

  Serial.print("[MQTT] Publishing to ");
  Serial.print(MQTT_TOPIC);
  Serial.print(": ");
  Serial.println(payload);

  // The true flag makes this a retained MQTT message for new subscribers.
  if (client.publish(MQTT_TOPIC, payload.c_str(), true)) {
    Serial.println("[MQTT] Publish successful");
  } else {
    Serial.print("[MQTT] Publish failed, client state: ");
    Serial.println(client.state());
  }

  Serial.println("[Loop] Waiting 5 seconds before next reading");
  delay(5000);
}
