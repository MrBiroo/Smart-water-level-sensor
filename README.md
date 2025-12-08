# Smart Water Level Sensor

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](./LICENSE) [![Arduino Compatible](https://img.shields.io/badge/Arduino-compatible-blue.svg)](https://www.arduino.cc/) [![MQTT](https://img.shields.io/badge/MQTT-HiveMQ-orange.svg)](https://www.hivemq.com/public-mqtt-broker/) [![IoT](https://img.shields.io/badge/Category-IoT-lightgrey.svg)](#) [![Made with ❤️](https://img.shields.io/badge/Made%20with-%E2%9D%A4-red.svg)]

Smart Water Level Sensor is a lightweight IoT project using an ESP32 and an HC-SR04 ultrasonic sensor to measure water level inside a tank and publish three useful measurements to MQTT:
- Raw distance (cm)
- Level percentage (%)
- Volume (liters)

This repository contains a clean, maintainable Arduino-style firmware, documentation, wiring guidance, and a recommended repository layout that suits development in GitHub Codespaces, local machines, or CI.

Project highlights
- ESP32 microcontroller for WiFi + low-power deep sleep
- HC-SR04 ultrasonic sensor for non-contact distance measurement
- MQTT publishing (designed for HiveMQ public broker by default)
- Bubble-sort median filter for robust measurement
- WiFi reconnect logic and deep-sleep power savings
- Recruiter-friendly documentation and structure

Features
- Periodic level reporting to MQTT
- Median filtering (bubble-sort) to remove outliers
- Configurable tank geometry for liters calculation
- WiFi reconnection and robust MQTT client
- Graceful deep-sleep between measurements for battery operation
- Simple wiring and minimal external components

Hardware used (example)
| Component | Example |
|---|---|
| Microcontroller | ESP32 (DevKitC, WROOM or similar) |
| Ultrasonic sensor | HC-SR04 |
| Power | 5V USB / battery pack (ensure proper voltage for HC-SR04 trigger/echo levels or use level shifter) |
| Optional | Logic-level converter, resistor divider (if needed), screw terminal / breadboard |

Wiring diagram
- VCC of HC-SR04 -> 5V (or 5V supply)
- GND of HC-SR04 -> GND
- TRIG -> ESP32 GPIO 26 (example)
- ECHO -> ESP32 GPIO 25 (example, note: ECHO from HC-SR04 is 5V — use level shifting if your board is not 5V tolerant)
- ESP32 -> WiFi -> MQTT Broker

Example wiring (text)
- HC-SR04 VCC -> 5V
- HC-SR04 GND -> GND
- HC-SR04 TRIG -> GPIO 26 (configurable in firmware)
- HC-SR04 ECHO -> GPIO 25 (use divider or level-shifter)
- ESP32 GND -> GND
- ESP32 5V -> 5V (if powering sensor from ESP32 board)

Important safety note: the HC-SR04 echo pin outputs 5 V. On many ESP32 dev boards the pins are 5V tolerant, but it is best to use a resistor divider or level shifter for protection.

System architecture (ASCII)
---------------------------
Human-readable ASCII diagram showing how the sensor node fits in the system:

[HC-SR04 Ultrasonic]
       |
       v
[ESP32 Sensor Node]
  - measure distance
  - filter readings
  - compute % and liters
  - publish to MQTT
       |
       v
[MQTT Broker (HiveMQ public broker)]
       |
       v
[Consumers]
  - Dashboard (Node-RED, Home Assistant)
  - Logger (InfluxDB)
  - Mobile app

MQTT topics
| Topic | Payload | Description |
|---|---:|---|
| smartwater/<device_id>/distance_cm | float | Raw measured distance in centimeters |
| smartwater/<device_id>/level_percent | float | Percent full (0-100) |
| smartwater/<device_id>/liters | float | Calculated volume in liters |
| smartwater/<device_id>/status | string | "online" / "offline" or status messages |

Notes:
- <device_id> is derived from the ESP32 MAC address for uniqueness.
- Use retained messages for last-known state if necessary (optional in firmware).

Installation & flashing (Codespaces friendly)
--------------------------------------------
Prerequisites:
- Arduino CLI or PlatformIO in your development environment.
- Or use the Arduino IDE + ESP32 board support package.
- GitHub Codespaces: install `arduino-cli` or use a devcontainer with the Arduino toolchain. You can also use PlatformIO extension in VS Code.

Quick steps (Arduino CLI)
1. Clone the repo in Codespace or locally:
   - git clone <your-repo-url>
   - cd smart-water-level-sensor
2. Copy `src/config.example.h` to `src/config.h` and fill in:
   - WIFI_SSID, WIFI_PASS, MQTT_SERVER, MQTT_PORT
3. Install board support (one-time):
   - arduino-cli core update-index
   - arduino-cli core install esp32:esp32
4. Compile & upload:
   - arduino-cli compile --fqbn esp32:esp32:esp32 dev
   - arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 dev

Quick steps (PlatformIO)
1. Open repository in VS Code + PlatformIO extension (or Codespaces).
2. Copy `src/config.example.h` to `src/config.h` and fill in credentials.
3. Use PlatformIO "Build" and "Upload" actions for the Espressif32 target.

How the firmware runs
- On boot: connects to WiFi (tries reconnect attempts), connects to MQTT broker (with LWT), measures the distance N times, applies median filter, computes level % and liters, publishes to topics, then goes to deep sleep for the configured interval.

Screenshots / placeholders
- docs/figures/screenshot-dashboard.png (placeholder)
- docs/figures/hardware-photo.jpg (placeholder)

Future improvements
- Add TLS / username/password authentication for MQTT
- Support OTA updates (ArduinoOTA or PlatformIO)
- Add on-device web configuration portal (AP mode)
- Add low-pass / complementary filtering for moving liquids
- Add automatic calibration routine and per-tank configuration UI
- Add battery monitoring and temperature compensation for speed of sound

Repository structure (recommended)
```
/src
  main.ino
  config.example.h
/docs
  wiring.md
  usage.md
/diagrams
  architecture.txt
.gitignore
LICENSE
README.md
```

Credits & contact
- Author: MrBiroo

License
-------
MIT License — see LICENSE file in the repository.
