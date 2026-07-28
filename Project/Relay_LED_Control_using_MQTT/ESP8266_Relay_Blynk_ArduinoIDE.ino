#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Secure MQTT client using the ESP8266's secure WiFi client.
WiFiClientSecure espClient;
PubSubClient client(espClient);

// WiFi network credentials.
const char* ssid = "B-220_2.4Ghz";
const char* password = "SuhaasPooja@1929";

// MQTT broker settings.
const char* mqtt_server = "8ff66edb6259450d8d29c1e80d066941.s1.eu.hivemq.cloud";
const int mqtt_port = 8883; // TLS port for secure MQTT
const char* mqtt_user = "hivemq.webclient.1785169513397";
const char* mqtt_password = "bGk523NLatrLhtzf4p2jNLtHs%xzLC$j";

// Hardware and MQTT topics.
#define RELAY_PIN 16
#define CMD_TOPIC "home/relay1"
#define STATUS_TOPIC "home/status/relay1"

// Connect to the configured WiFi network and wait until connected.
void connectWiFi() {
  Serial.printf("Connecting to WiFi '%s'...\n", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("WiFi connected, IP: ");
  Serial.println(WiFi.localIP());
}

// MQTT callback invoked when a message arrives on a subscribed topic.
void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";

  // Convert the payload bytes to a String.
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("MQTT message received [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  msg.trim();       // Remove whitespace from the message.
  msg.toUpperCase(); // Normalize command text.

  // Control the relay based on the received MQTT command.
  if (msg == "ON" || msg == "1") {
    digitalWrite(RELAY_PIN, LOW); // Relay active-low: LOW turns relay on.
    client.publish(STATUS_TOPIC, "ON", true); // Publish retained status update.
    Serial.println("Relay turned ON");
  } else if (msg == "OFF" || msg == "0") {
    digitalWrite(RELAY_PIN, HIGH); // Relay off when pin is HIGH.
    client.publish(STATUS_TOPIC, "OFF", true);
    Serial.println("Relay turned OFF");
  } else {
    Serial.println("Unknown MQTT command");
  }
}

// Ensure the MQTT client is connected and subscribed to the command topic.
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Relay", mqtt_user, mqtt_password)) {
      Serial.println("connected");

      if (client.subscribe(CMD_TOPIC)) {
        Serial.print("Subscribed to ");
        Serial.println(CMD_TOPIC);
      } else {
        Serial.print("Failed to subscribe to ");
        Serial.println(CMD_TOPIC);
      }
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
  Serial.println("Starting ESP8266 Relay MQTT debug mode");

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Start with relay off.

  connectWiFi();

  // Use insecure TLS mode because no certificate validation is configured.
  espClient.setInsecure();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  // Keep WiFi connected. If disconnected, reconnect automatically.
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // Keep MQTT connected and process incoming messages.
  if (!client.connected()) {
    reconnectMQTT();
  }

  client.loop();
}
