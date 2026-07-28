Phase 1: Project Objective

    Control one relay over the Internet using MQTT.

    When you press a button in the mobile app:

        Mobile App
            │
            ▼
        MQTT Broker
            │
            ▼
        ESP8266
            │
            ▼
        Relay
            │
            ▼
        LED

Phase 2: Hardware Required

    | Component              | Quantity |
    | ---------------------- | -------: |
    | ESP8266 NodeMCU        |        1 |
    | 1-Channel Relay Module |        1 |
    | LED                    |        1 |
    | 220Ω Resistor          |        1 |
    | Breadboard             |        1 |
    | Jumper Wires           |  Several |
    | USB Cable              |        1 |

Phase 3: Software Required

    Install:

        Arduino IDE
        ESP8266 Board Package
        PubSubClient Library
        ArduinoJson Library

Phase 4: Wiring

    ESP8266 → Relay

    | ESP8266    | Relay |
    | ---------- | ----- |
    | D1 (GPIO5) | IN    |
    | VIN        | VCC   |
    | GND        | GND   |

Phase 5: Install MQTT Broker

    For learning, I recommend HiveMQ Cloud (free tier).

    Create Account

    Visit:

    https://www.hivemq.com/mqtt-cloud-broker/

    Create a free account.

    Create:

        Cluster
        ↓
        Username
        ↓
        Password
        ↓
        Broker URL

    Example:

        Broker: abcd1234.s1.eu.hivemq.cloud

        Port:   8883 (TLS) or 1883 (non-TLS for local testing)

Phase 6: MQTT Explorer (PC)

    Install MQTT Explorer.

        Connect using

        Host: abcd1234.s1.eu.hivemq.cloud

        Username: xxxx

        Password: xxxx

    Now you'll be able to see every MQTT message.

Phase 7: Mobile MQTT App

    Install

    IoT MQTT Panel or MQTT Dash

    Create one button.

    Topic: home/relay1

    Payload ON: ON

    Payload OFF: OFF

 Phase 8: ESP8266 Program Flow

    Power ON
    ↓
    Initialize GPIO
    ↓
    Initialize Serial
    ↓
    Connect Wi-Fi
    ↓
    Connect MQTT
    ↓
    Subscribe
    ↓
    home/relay1
    ↓
    Wait
    ↓
    Receive Message
    ↓
    Switch Relay
    ↓
    Publish Status

Phase 9: Connect Wi-Fi

    Example

        WiFi.begin(ssid,password);

        while(WiFi.status()!=WL_CONNECTED)
        {
            delay(500);
        }

    Serial Monitor

        Connecting...
        Connecting...
        Connected IP 192.168.1.25

Phase 10: Connect MQTT

    Wi-Fi Connected
    ↓
    MQTT Connect
    ↓
    Authentication
    ↓
    Connected
    ↓
    Subscribe

    Topic: home/relay1

Phase 11: Publish Message

    From Mobile

        Topic: home/relay1
        Payload: ON

    MQTT Broker
    ↓
    ESP8266 receives
    ↓
    Callback Function

Phase 12: Callback Function

    The callback receives

        Topic: home/relay1
        Payload: ON

    Then

        digitalWrite(GPIO5,LOW);

    Relay ON
    LED ON

    OFF

        digitalWrite(GPIO5,HIGH);

    Relay OFF
    LED OFF

Phase 13: Publish Status

    After relay changes

    Publish

        home/status/relay1
        ON

    Now Mobile --> MQTT Explorer --> Linux --> Everyone knows relay state.

Phase 14: Communication Example

    Phone
    ↓
    Topic: home/relay1
    ↓
    Payload: ON
    ↓
    Broker
    ↓
    ESP8266
    ↓
    GPIO5 LOW
    ↓
    Relay
    ↓
    LED

Phase 15: Debugging

    Serial Monitor

            Booting...
            Connecting Wi-Fi...
            Connected IP 192.168.1.25
            Connecting MQTT...
            MQTT Connected
            Subscribed
            Waiting...

Phase 16: MQTT Communication Flow

    +------------------+
    |  Mobile App      |
    | MQTT Explorer    |
    +--------+---------+
            |
            | Publish
            |
            v
    +---------------------------+
    |      MQTT Broker          |
    | (HiveMQ / Mosquitto)      |
    +-------------+-------------+
                |
                | Subscribe
                |
                v
    +---------------------------+
    |      ESP8266 NodeMCU      |
    +-------------+-------------+
                |
                |
            Relay GPIO
                |
                v
            Relay Module

Phase 17: Complete Mapping Diagram

                    Wi-Fi Router
                 SSID: JioFiber
                 Password: ********
                       │
                       │
             192.168.1.25
                       │
                       ▼
               ESP8266 NodeMCU
                       │
             MQTT Client Library
                       │
          Broker: xxxxxxxx.s1.eu.hivemq.cloud
                       │
                 Port: 8883 (TLS)
                 Username: purushotham
                 Password: ********
                       │
         Subscribe: home/relay1
         Publish : home/status/relay1
                       │
                       ▼
                GPIO5 (D1)
                       │
                  Relay Module
                       │
                     LED