# ESP8266 Relay LED Control Tutorial

## Required Components

| Component                   | Quantity |
| --------------------------- | -------- |
| ESP8266 NodeMCU             | 1        |
| 1-Channel Relay Module (5V) | 1        |
| 1 LED                       | 1        |
| 220Ω Resistor               | 1        |
| USB Cable                   | 1        |
| Breadboard                  | 1        |
| Jumper Wires                | 5        |
| Mobile Phone                | 1        |
| Wi-Fi Router                | 1        |
| Blynk IoT Cloud             | 1 service |

## Project Architecture


Smartphone
    │
Blynk Cloud
    │
Internet / Wi-Fi
    │
Wi-Fi Router
    │
ESP8266 NodeMCU
    │
Relay Driver GPIO
    │
Relay Module
    │
Electrical Load


## Circuit Diagram

ESP8266               Relay Module
D1 (GPIO5) ---------- IN
VIN (5V) ----------- VCC
GND ---------------- GND


Relay Contacts
COM --------------- +5V
NO ---------------- 220Ω Resistor
220Ω -------------- LED (+)
LED (-) ----------- GND

## Why Use a 220Ω Resistor?

Without a resistor, excessive current can damage the LED.

Calculation:

- Supply = 5V
- LED drop = 2V
- Current = 15mA
- R = (5 − 2) / 0.015 ≈ 200Ω

Use the nearest standard value: **220Ω**.

## Step 1: Create a Blynk Account

1. Install Blynk IoT from the Google Play Store or Apple App Store.
2. Sign up with your email.
3. Verify your email.
4. Log in.

## Step 2: Create a Blynk Template

1. Open the Blynk web dashboard.
2. Click **New Template**.
3. Enter:
   - Template Name: `ESP8266 Relay`
   - Hardware: `ESP8266`
   - Connection: `WiFi`
4. Click **Done**.

## Step 3: Create a Datastream

1. Open the template.
2. Go to **Datastreams** → **New Datastream** → **Virtual Pin**.
3. Create the datastream:
   - Name: `Relay1`
   - Virtual Pin: `V0`
   - Data Type: `Integer`
   - Min: `0`
   - Max: `1`
   - Default: `0`
4. Save it.

## Step 4: Create a Device

1. Click **Devices** → **New Device** → **From Template** → **ESP8266 Relay**.
2. Blynk generates:
   - `BLYNK_TEMPLATE_ID`
   - `BLYNK_DEVICE_NAME`
   - `BLYNK_AUTH_TOKEN`

Example:

```cpp
#define BLYNK_TEMPLATE_ID "TMPxxxx"
#define BLYNK_DEVICE_NAME "Home Relay"
#define BLYNK_AUTH_TOKEN "xxxxxxxxxxxxxxxx"
```

Keep the authentication token secure.

## Step 5: Build the Mobile Dashboard

1. Open the Blynk app.
2. Select your device.
3. Add a **Button** widget.
4. Configure the widget:
   - Datastream: `V0`
   - Mode: `Switch`
   - Value when pressed: `1`
   - Value when released: `0`

## Step 6: Build the Mobile Dashboard

1. Open the Blynk app.
2. Select your device.
3. Add a **Button** widget.
4. Configure the widget:
   - Datastream: `V0`
   - Mode: `Switch`
   - Value when pressed: `1`
   - Value when released: `0`

## Step 7: Wire the Relay

### NodeMCU to Relay

- `D1 (GPIO5)` → `IN`
- `GND` → `GND`
- `VIN (5V)` → `VCC`

### Load Wiring

- `AC Live` → `COM`
- `NO` → `220Ω Resistor`
- `220Ω Resistor` → `LED (+)`
- `LED (-)` → `GND`

> Warning: If you are switching mains voltage, take proper electrical safety precautions. For initial testing, use a low-voltage load or ask an experienced person to assist.

## Step 8: Install Arduino Libraries

Install via Library Manager:

- `Blynk`
- `ESP8266WiFi`

## Step 9: Upload the Program

1. Connect the ESP8266 via USB.
2. Select the board: **NodeMCU 1.0 (ESP-12E Module)**.
3. Select the correct COM port.
4. Upload the sketch.
5. Open the Serial Monitor at `115200` baud to view connection status.

## Step 10: Test

Power on the board.

The startup sequence is:

- ESP boots
- Connects to Wi-Fi
- Connects to Blynk Cloud
- Ready

Press the button in the app:

- Button ON → `V0 = 1` → `GPIO5 LOW` → Relay ON
- Button OFF → `V0 = 0` → `GPIO5 HIGH` → Relay OFF

## Relay Operation

When you press ON in the Blynk app:

- Mobile → Blynk Cloud → ESP8266 → GPIO LOW → Relay energised → NO closes → LED ON

When you press OFF:

- Relay OFF → NO opens → LED OFF

## Step 11: Expand to 8 Relays

Connect the relay inputs to eight GPIOs. Choose GPIOs carefully because some ESP8266 pins are boot-sensitive.

### Example Mapping

| Relay  | GPIO  | Blynk Virtual Pin |
| ------ | ----- | ----------------- |
| Relay1 | D1    | V0                |
| Relay2 | D2    | V1                |
| Relay3 | D5    | V2                |
| Relay4 | D6    | V3                |
| Relay5 | D7    | V4                |
| Relay6 | D0    | V5                |
| Relay7 | D3*   | V6                |
| Relay8 | D4*   | V7                |

> Note: `GPIO0` (D3) and `GPIO2` (D4) are boot-sensitive pins. Avoid driving these pins low during boot.

For a more robust 8-relay design, consider:

- using an I²C GPIO expander (e.g., MCP23017)
- using an ESP32

Each relay requires:

- One Button Widget
- One Virtual Pin
- One `BLYNK_WRITE()` handler (or a shared handler with an array)
