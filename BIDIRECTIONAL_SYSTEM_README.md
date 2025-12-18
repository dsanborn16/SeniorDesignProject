# VEX V5 Brain ↔ ESP32 Bidirectional Communication System

## Overview

This system implements **bidirectional communication** between a VEX V5 Brain and an ESP32 microcontroller via RS-485, with integrated load cell monitoring and MQTT cloud publishing.

## System Architecture

```
┌─────────────────┐         RS-485          ┌─────────────────┐
│   VEX V5 Brain  │ ←────────────────────→  │     ESP32       │
│                 │      (MAX485 x1)        │                 │
│  - Sends: Brain │                         │  - Receives:    │
│    battery data │                         │    Brain data   │
│    temperature  │                         │  - Sends: Load  │
│    voltage      │                         │    cell weight  │
│  - Receives:    │                         │  - Reads: HX711 │
│    Load cell    │                         │    load cell    │
│    weight       │                         │  - Publishes to │
│  - Displays all │                         │    MQTT broker  │
│    data on LCD  │                         │                 │
└─────────────────┘                         └────────┬────────┘
                                                     │
                                                     │ WiFi
                                                     ▼
                                              ┌──────────────┐
                                              │ MQTT Broker  │
                                              │ (HiveMQ)     │
                                              └──────────────┘
```

## Features

### VEX V5 Brain (`vex_brain_bidirectional.v5cpp`)
- ✅ Sends brain status data to ESP32 (battery %, voltage, current, temperature, system time)
- ✅ Receives load cell weight data from ESP32
- ✅ Real-time LCD display of all data
- ✅ Robust message parsing with start/end delimiters
- ✅ Error handling and timeout detection

### ESP32 (`ESP32-Bidirectional/src/main.cpp`)
- ✅ Bidirectional RS-485 communication with VEX Brain
- ✅ HX711 load cell reading and averaging
- ✅ WiFi connectivity with auto-reconnect
- ✅ MQTT publishing to multiple topics:
  - `ESP32/loadCell/weight` - Load cell weight in kg
  - `ESP32/vexBrain/battery` - VEX Brain battery percentage
  - `ESP32/vexBrain/temperature` - VEX Brain temperature in °C
  - `ESP32/vexBrain/status` - Combined status string
- ✅ Automatic connection recovery for WiFi and MQTT
- ✅ Detailed serial debug output

## Communication Protocol

### Message Format
All messages use a simple delimited protocol:
```
<MESSAGE_TYPE|field1|field2|...>
```

- **Start delimiter**: `<`
- **End delimiter**: `>`
- **Field separator**: `|`

### Message Types

#### From VEX Brain to ESP32:
```
<BRAIN|time|battery%|voltage|current|temp>
```
Example: `<BRAIN|45.23|87.5|12.45|1.23|32.1>`

#### From ESP32 to VEX Brain:
```
<WEIGHT|value>
```
Example: `<WEIGHT|12.45>`

```
<STATUS|message>
```
Example: `<STATUS|MQTT Connected>`

## Hardware Requirements

### VEX V5 Brain Side
- 1x VEX V5 Brain
- 1x Smart Port cable (or custom wiring to Smart Port 21)
- Wiring:
  - VEX TX → MAX485 DI
  - VEX RX → MAX485 RO
  - VEX +5V → MAX485 VCC
  - VEX GND → MAX485 GND

### ESP32 Side
- 1x ESP32 development board
- 1x MAX485 RS-485 transceiver module (shared with VEX)
- 1x HX711 load cell amplifier
- 1x Load cell (strain gauge)
- Power supply for ESP32 and peripherals
- Wiring:
  - **MAX485 to ESP32:**
    - RO → GPIO 16 (RX)
    - DI → GPIO 17 (TX)
    - DE & RE → GPIO 4 (tied together)
    - VCC → ESP32 3.3V
    - GND → ESP32 GND
  - **HX711 to ESP32:**
    - DOUT → GPIO 25
    - SCK → GPIO 26
    - VCC → 3.3V or 5V
    - GND → GND

### RS-485 Connection
- Single MAX485 module connects both VEX and ESP32
- Use twisted pair cable for reliable communication
- Recommended: 120Ω termination resistor if experiencing issues

## Configuration

### VEX V5 Brain
Edit these constants in `vex_brain_bidirectional.v5cpp`:
```cpp
const int SERIAL_PORT_NUM = 21;          // Smart Port number
const int BAUD_RATE = 115200;            // Must match ESP32
const int SEND_INTERVAL_MS = 1000;       // Data send interval
```

### ESP32
Edit these constants in `ESP32-Bidirectional/src/main.cpp`:

#### WiFi Settings
```cpp
const char* WIFI_SSID = "YourWiFiName";
const char* WIFI_PASSWORD = "YourPassword";
```

#### MQTT Settings
```cpp
const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC_WEIGHT = "ESP32/loadCell/weight";
const char* MQTT_TOPIC_BRAIN_BATTERY = "ESP32/vexBrain/battery";
const char* MQTT_TOPIC_BRAIN_TEMP = "ESP32/vexBrain/temperature";
const char* MQTT_TOPIC_BRAIN_STATUS = "ESP32/vexBrain/status";
```

#### HX711 Calibration
```cpp
const float CALIBRATION_FACTOR = 106.53;  // Replace with your calibrated value
```

#### Pin Configuration
```cpp
#define DE_RE_PIN 4              // MAX485 direction control
#define RS485_RX_PIN 16          // ESP32 RX
#define RS485_TX_PIN 17          // ESP32 TX
#define HX711_DOUT_PIN 25        // HX711 data
#define HX711_SCK_PIN 26         // HX711 clock
```

## Installation and Setup

### VEX V5 Brain

1. Open `vex_brain_bidirectional.v5cpp` in VEXcode V5
2. Verify the configuration matches your hardware
3. Build and download to the VEX V5 Brain
4. Connect the MAX485 module to Smart Port 21

### ESP32

1. Install PlatformIO (if not already installed)
2. Open the `ESP32-Bidirectional` folder in VS Code with PlatformIO
3. Edit `src/main.cpp` with your WiFi credentials and calibration values
4. Connect your ESP32 via USB
5. Build and upload:
   ```bash
   pio run --target upload
   ```
6. Monitor serial output:
   ```bash
   pio device monitor
   ```

## Usage

1. **Power on both systems**
   - VEX Brain will display initialization screen
   - ESP32 will connect to WiFi and MQTT broker

2. **Verify communication**
   - VEX Brain LCD should show:
     - "VEX V5 <-> ESP32 Bridge"
     - Transmitted data (battery %, temperature)
     - Received weight from load cell
   - ESP32 Serial Monitor should show:
     - WiFi connection status
     - MQTT connection status
     - Received VEX Brain data
     - Published MQTT messages

3. **Monitor MQTT data**
   - Use an MQTT client (MQTT Explorer, mosquitto_sub, etc.)
   - Connect to `broker.hivemq.com:1883`
   - Subscribe to topics:
     - `ESP32/loadCell/weight`
     - `ESP32/vexBrain/battery`
     - `ESP32/vexBrain/temperature`
     - `ESP32/vexBrain/status`

## Troubleshooting

### No Communication Between VEX and ESP32
- ✓ Verify RS-485 A and B lines are connected correctly (not swapped)
- ✓ Check baud rate matches on both sides (115200)
- ✓ Verify MAX485 modules have power
- ✓ Test with shorter cable first (reduce interference)
- ✓ Check DE/RE pin control on ESP32 side

### Load Cell Not Reading
- ✓ Verify HX711 wiring (DOUT, SCK, VCC, GND)
- ✓ Check load cell wire colors match HX711 inputs
- ✓ Run calibration procedure to find correct calibration factor
- ✓ Ensure load cell is mechanically mounted correctly

### WiFi Connection Issues
- ✓ Verify SSID and password are correct
- ✓ Check WiFi signal strength near ESP32
- ✓ Ensure 2.4GHz WiFi is enabled (ESP32 doesn't support 5GHz)

### MQTT Publishing Fails
- ✓ Verify WiFi is connected first
- ✓ Check MQTT broker address and port
- ✓ Test broker connectivity with another MQTT client
- ✓ Consider using a private MQTT broker if public broker is unreliable

### VEX Brain Display Shows "No data"
- ✓ Verify ESP32 is sending weight messages
- ✓ Check RS-485 communication is working
- ✓ Monitor ESP32 serial output for errors
- ✓ Verify load cell is reading valid data

## Data Flow Summary

1. **VEX Brain → ESP32**: Battery, voltage, current, temperature (every 1 second)
2. **ESP32 → VEX Brain**: Load cell weight, status messages (every 50ms weight, status as needed)
3. **ESP32 → MQTT Broker**: Load cell weight + all VEX Brain data (every 1 second)

## Timing Configuration

| Operation | Interval | Configurable Constant |
|-----------|----------|----------------------|
| VEX sends data | 1000ms | `SEND_INTERVAL_MS` |
| VEX checks RS-485 | 20ms | `RECEIVE_CHECK_MS` |
| ESP32 reads load cell | 50ms | `WEIGHT_READ_INTERVAL` |
| ESP32 reads RS-485 | 20ms | `RS485_READ_INTERVAL` |
| ESP32 publishes MQTT | 1000ms | `MQTT_PUBLISH_INTERVAL` |
| ESP32 WiFi check | 5000ms | `WIFI_RECONNECT_INTERVAL` |

## MQTT Topic Hierarchy

```
ESP32/
├── loadCell/
│   └── weight          (float, kg)
└── vexBrain/
    ├── battery         (float, %)
    ├── temperature     (float, °C)
    └── status          (string, combined status)
```

## Future Enhancements

- [ ] Add multiple load cell support
- [ ] Implement data buffering for offline operation
- [ ] Add configuration via web interface
- [ ] Support for VEX motor/sensor data
- [ ] Data logging to SD card
- [ ] Alert notifications via MQTT
- [ ] Support for additional sensors on ESP32

## License

This project is provided as-is for educational and development purposes.

## Support

For issues or questions, refer to:
- VEX V5 API documentation
- ESP32 Arduino core documentation
- HX711 library documentation
- PubSubClient MQTT library documentation
