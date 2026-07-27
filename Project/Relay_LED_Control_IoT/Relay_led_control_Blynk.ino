#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "ESP8266 Relay"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASSWORD";

#define RELAY D1

BLYNK_WRITE(V0)
{
    int state = param.asInt();

    if(state)
    {
        digitalWrite(RELAY, LOW);   // Active LOW relay ON
    }
    else
    {
        digitalWrite(RELAY, HIGH);  // Relay OFF
    }
}

void setup()
{
    pinMode(RELAY, OUTPUT);

    digitalWrite(RELAY, HIGH);

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop()
{
    Blynk.run();
}