Project Objective

    Read:

        Temperature (°C)
        Pressure (hPa)
        Altitude (m)
        Humidity (optional if using BME280 instead)

    from a BMP280 sensor over I²C and publish the readings to an MQTT Broker every 5 seconds.

System Architecture

    BMP280 Sensor (Temperature, Pressure)
                │
        I²C Interface
        SDA           SCL
        │             │
        ▼             ▼
    ESP8266 NodeMCU
              │
      Wi-Fi (802.11 b/g/n)
              │
         MQTT Client
              │
        MQTT Broker
              │
      MQTT Explorer /
      Home Assistant /
      Node-RED /
      Mobile App

Hardware Required

    | Component       | Quantity |
    | --------------- | -------: |
    | ESP8266 NodeMCU |        1 |
    | BMP280 Module   |        1 |
    | Breadboard      |        1 |
    | Jumper Wires    |        4 |
    | USB Cable       |        1 |

BMP280 Pinout

    Typical module:

        For I²C Only use:

            VIN
            GND
            SDA
            SCL

ESP8266 I²C Pins

    | ESP8266    | BMP280 |
    | ---------- | ------ |
    | D1 (GPIO5) | SCL    |
    | D2 (GPIO4) | SDA    |
    | 3.3V       | VIN    |
    | GND        | GND    |

Wiring Diagram

    ESP8266                    BMP280

    3.3V ---------------------- VIN

    GND ----------------------- GND

    D1(GPIO5) ----------------- SCL

    D2(GPIO4) ----------------- SDA

Communication Flow

    BMP280
    ↓
    I²C Driver
    ↓
    ESP8266
    ↓
    Read Sensor
    ↓
    Convert Values
    ↓
    JSON Packet
    ↓
    MQTT Publish
    ↓
    Cloud
    ↓
    Dashboard

Install Arduino Libraries

    Install:

        Adafruit BMP280 Library
        Adafruit Unified Sensor
        ESP8266WiFi
        PubSubClient
        ArduinoJson
    
MQTT Topics

    We'll publish all readings to a single topic:

        home/sensors/bmp280

JSON Payload

    Publish:

    {
        "temperature": 28.5,
        "pressure": 1008.7,
        "altitude": 110.3
    }

Program Flow

    Power ON
    ↓
    Initialize Serial
    ↓
    Initialize Wi-Fi
    ↓
    Connect MQTT
    ↓
    Initialize I²C
    ↓
    Initialize BMP280
    ↓
    Read Sensor
    ↓
    Create JSON
    ↓
    Publish MQTT
    ↓
    Delay 5 Seconds
    ↓
    Repeat 

Initialize I²C

    #include <Wire.h>

    void setup()
    {
        Wire.begin(D2, D1);   // SDA, SCL
    }

Initialize BMP280

    #include <Adafruit_BMP280.h>

    Adafruit_BMP280 bmp;

    void setup()
    {
        bmp.begin(0x76);
    }

    Some modules use address 0x77.

Read Temperature

    float temperature = bmp.readTemperature();

Read Pressure

    float pressure = bmp.readPressure()/100.0;

    Convert Pascals → hPa.

Read Altitude

    float altitude =
    bmp.readAltitude(1013.25);

    1013.25 is the standard sea-level pressure in hPa. For more accurate altitude, use the current local sea-level pressure for your location.

Create JSON

    StaticJsonDocument<256> doc;

    doc["temperature"] = temperature;

    doc["pressure"] = pressure;

    doc["altitude"] = altitude;

    char buffer[256];

    serializeJson(doc, buffer);

Result

    {
    "temperature":29.1,
    "pressure":1006.5,
    "altitude":112.2
    }

Publish MQTT

    client.publish(
    "home/sensors/bmp280",
    buffer
    );

MQTT Explorer

    Subscribe

        home/sensors/bmp280

    Every 5 seconds

        {
            "temperature":29.1,
            "pressure":1006.5,
            "altitude":112.2
        }

