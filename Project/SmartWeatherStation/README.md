# SmartWeatherStation

Starter project with ESP8266, BMP280, OLED and MQTT.

Wiring:
D1->SCL
D2->SDA
3.3V->VCC
GND->GND
OLED:0x3C BMP280:0x76

Topic: home/weather/bmp280

System Architecture

                    +----------------+
                    |    BMP280      |
                    |  Temperature   |
                    |   Pressure     |
                    +-------+--------+
                            |
                            | I²C
                            |
          +-----------------+----------------+
          |                                  |
          | SDA (GPIO4/D2)                   |
          | SCL (GPIO5/D1)                   |
          |                                  |
     +----+----------------------------------+----+
     |             ESP8266 NodeMCU                |
     |                                            |
     |  Wi-Fi            MQTT Client              |
     +----+----------------------------+----------+
          |                            |
          |                            |
      Internet                     OLED SSD1306
          |                            |
     MQTT Broker                  Live Display

I²C Connections

    Since both devices use I²C, they share the same SDA and SCL lines.

    | ESP8266    | BMP280 | OLED SSD1306 |
    | ---------- | ------ | ------------ |
    | 3.3V       | VIN    | VCC          |
    | GND        | GND    | GND          |
    | D1 (GPIO5) | SCL    | SCL          |
    | D2 (GPIO4) | SDA    | SDA          |

    This is one of the main advantages of I²C—you can connect multiple devices to the same bus as long as their addresses differ.

I²C Addresses

    Typical addresses are:

    | Device       | Address        |
    | ------------ | -------------- |
    | BMP280       | 0x76 (or 0x77) |
    | SSD1306 OLED | 0x3C           |

    No address conflict occurs.

Update Flow

    Boot
    ↓
    OLED Startup Logo
    ↓
    Wi-Fi Connecting
    ↓
    Wi-Fi Connected
    ↓
    MQTT Connecting
    ↓
    MQTT Connected
    ↓
    Read BMP280
    ↓
    Update OLED
    ↓
    Publish JSON
    ↓
    Repeat every 5 seconds

Example OLED Screen

    +--------------------------------+
    | WiFi : Connected               |
    | MQTT : Connected               |
    |                                |
    | Temp : 29.5 C                  |
    | Press:1009.1 hPa               |
    | Alt  :111.2 m                  |
    |                                |
    | IP:192.168.1.52                |
    +--------------------------------+

Professional Dashboard

    A more polished layout looks like this:

    +--------------------------------+
    | Smart Weather Station          |
    |--------------------------------|
    | Temp : 29.5 C                  |
    | Press:1009.1 hPa               |
    | Alt  :111.2 m                  |
    |--------------------------------|
    | WiFi : ✓                       |
    | MQTT : ✓                       |
    | RSSI : -53 dBm                 |
    +--------------------------------+