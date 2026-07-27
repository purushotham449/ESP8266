# DIP Switch and Relay Control with ESP8266

## Project Objective

Control 8 relays independently using 8 DIP switches.

## System Diagram

+---------------------+
|    8 DIP Switches   |
+----------+----------+
           |
           |
        ESP8266
           |
           |
+----------+----------+
|     8 Relay Module  |
+----------+----------+
           |
      AC/DC Loads

Each switch controls one relay.

## Example Mapping

| DIP Switch | Relay  | Load  |
| ---------- | ------ | ----- |
| SW1        | Relay1 | Light |
| SW2        | Relay2 | Fan   |
| SW3        | Relay3 | Pump  |
| ...        | ...    | ...   |
| SW8        | Relay8 | Motor |

## Components Required

| Component                                                | Qty     |
| -------------------------------------------------------- | -------:|
| ESP8266 NodeMCU                                          | 1       |
| 8-Channel Relay Module (5V, Opto-isolated)               | 1       |
| 8-position DIP Switch                                    | 1       |
| 10kΩ Resistors (optional if not using internal pull-ups) | 8       |
| Breadboard                                               | 1       |
| Jumper Wires                                             | Several |
| 5V 2A Power Supply                                       | 1       |

## Understanding the DIP Switch

A DIP switch is simply eight independent SPST switches.

- When the switch is ON: GPIO is connected to GND
- When the switch is OFF: GPIO is open (pulled high by pull-up resistor)

### Example

Switch state:  ON  OFF
Pins:          1 2 3 4 5 6 7 8

## Using Internal Pull-up

ESP8266 supports internal pull-up resistors for GPIO inputs.

3.3V
  |
[Internal Pull-up]
  |
 GPIO
  |
 Switch
  |
 GND

- Switch OFF => GPIO = HIGH
- Switch ON  => GPIO = LOW

Most cases do not require an external resistor when using the internal pull-up.

## Relay Module

A typical 8-channel relay module includes:

- IN1
- IN2
- IN3
- IN4
- IN5
- IN6
- IN7
- IN8
- VCC
- GND

Most relay modules are active LOW.

### Relay Logic

| GPIO | Relay state | Result   |
| ---- | ------------ | -------- |
| LOW  | ON           | Relay closes |
| HIGH | OFF          | Relay opens  |

## ESP8266 GPIO Selection

Not all ESP8266 GPIO pins are safe to use for inputs or outputs because some pins affect boot mode. NodeMCU exposes fewer reliable GPIOs than the raw ESP8266 module.

### Practical GPIO Mapping

| DIP | Switch GPIO        | Relay | Relay GPIO                    |
| --- | ------------------ | ----- | ----------------------------- |
| SW1 | D1 (GPIO5)         | R1    | D0 (GPIO16)                   |
| SW2 | D2 (GPIO4)         | R2    | D5 (GPIO14)                   |
| SW3 | D6 (GPIO12)        | R3    | D7 (GPIO13)                   |
| SW4 | D8 (GPIO15)*       | R4    | D4 (GPIO2)*                   |
| SW5 | D3 (GPIO0)*        | R5    | External expander recommended |
| SW6 | RX (GPIO3)         | R6    | External expander recommended |
| SW7 | TX (GPIO1)         | R7    | External expander recommended |
| SW8 | A0 (Analog)        | R8    | External expander recommended |

> Important: An ESP8266 does not comfortably provide 16 independent GPIOs (8 inputs + 8 outputs) without using boot-sensitive pins or the serial interface.

For a reliable design, consider:

- using an I²C GPIO expander (recommended)
- using shift registers
- moving to an ESP32 for more GPIO availability

## Wiring

### DIP Switch Wiring

- GPIO5 ---- Switch1 ---- GND
- GPIO4 ---- Switch2 ---- GND
- GPIO12 --- Switch3 ---- GND
- ...

### Relay Wiring

- GPIO16 ---- Relay IN1
- GPIO14 ---- Relay IN2
- GPIO12 ---- Relay IN3
- ...

- VCC -> 5V
- GND -> Common Ground

Remember to connect the ESP8266 GND and relay module GND together.

## Logic Flow

1. Read switch state
2. Is the switch ON?
   - Yes -> Turn relay ON
   - No  -> Turn relay OFF

## Debouncing

Mechanical switches bounce for a few milliseconds. Without debouncing, a single press may appear as multiple presses.

### Debounce Options

- Use software debouncing in code
- Add a small hardware RC filter
- Use a library that supports debouncing

## Notes

- Avoid using boot-mode-sensitive pins for reliable operation.
- Use the ESP8266 internal pull-up resistors when possible.
- Test the wiring and logic with one relay before connecting all channels.


wait 10–20 ms after a detected change and confirm the state,
or use a software debounce library,
or implement a finite-state machine.
Safety

Because relays often switch mains voltage:

Use an opto-isolated relay module.
Never power relay coils directly from the ESP8266.
Use a dedicated 5 V power supply for the relay board if required.
Ensure common ground between the ESP8266 and the relay module input side.
Keep low-voltage wiring separate from mains wiring.
Recommended Improvement: MCP23017 I²C GPIO Expander

Since your project requires 8 inputs and 8 outputs, I strongly recommend using an MCP23017 (16 GPIO) instead of relying on the limited GPIOs of the ESP8266.

Advantages:

Provides 16 additional GPIOs over I²C.
Uses only 2 ESP8266 pins (SDA and SCL).
Frees the ESP8266 from GPIO limitations.
Easily expandable by adding more MCP23017 devices.
Widely used in industrial and hobby embedded projects.

Your architecture would become:

8 DIP Switches
       │
       ▼
+------------------+
|    MCP23017      |
| 16 GPIO Expander |
+------------------+
       │ I²C
       ▼
   ESP8266
       │
       ▼
8-Relay Module

This design is more robust, easier to scale, and much closer to how professional embedded systems handle large numbers of digital I/O. Given your embedded Linux and driver-development background, it's also a great introduction to working with external peripherals over I²C before moving on to writing your own drivers.