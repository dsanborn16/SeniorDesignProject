# Quick Start Guide - VEX V5 Brain ↔ ESP32 Bidirectional System

## 🚀 Quick Setup (5 Minutes)

### 1. Configure ESP32 Code
Edit `ESP32-Bidirectional/src/main.cpp`:
```cpp
const char* WIFI_SSID = "YourWiFiName";        // Line 20
const char* WIFI_PASSWORD = "YourPassword";     // Line 21
const float CALIBRATION_FACTOR = 106.53;        // Line 35 - Use your value
```

### 2. Upload Code
**VEX Brain:**
- Open `vex_brain_bidirectional.v5cpp` in VEXcode V5
- Download to VEX Brain

**ESP32:**
```bash
cd ESP32-Bidirectional
pio run --target upload
pio device monitor
```

### 3. Connect Hardware
- Wire MAX485 module as per `WIRING_INSTRUCTIONS.md`
- Connect both VEX and ESP32 to the single MAX485 module
- Connect HX711 load cell to ESP32
- Power both systems

### 4. Verify Operation
- ✓ VEX screen shows "VEX V5 <-> ESP32 Bridge"
- ✓ ESP32 Serial shows "System ready - entering main loop"
- ✓ Load cell weight appears on VEX screen
- ✓ VEX battery data appears in ESP32 Serial

---

## 📊 Data Flow

```
VEX Brain ──[Battery, Temp]──> ESP32 ──> MQTT Broker
VEX Brain <──[Load Cell Weight]── ESP32
```

---

## 🔧 Configuration Summary

| Parameter | VEX Brain | ESP32 | Must Match |
|-----------|-----------|-------|------------|
| Baud Rate | 115200 | 115200 | ✅ Yes |
| Smart Port | 21 | N/A | - |
| RS-485 RX | Port 21 Pin 4 | GPIO 16 | - |
| RS-485 TX | Port 21 Pin 3 | GPIO 17 | - |
| Direction Control | Not used | GPIO 4 | - |
| Load Cell Data | GPIO 25 | - |
| Load Cell Clock | GPIO 26 | - |

---

## 📡 MQTT Topics

Subscribe to these on your MQTT client:

```
ESP32/loadCell/weight          → Weight in kg (float)
ESP32/vexBrain/battery         → Battery % (float)
ESP32/vexBrain/temperature     → Temperature °C (float)
ESP32/vexBrain/status          → Combined status (string)
```

**Test with mosquitto_sub:**
```bash
mosquitto_sub -h broker.hivemq.com -t "ESP32/#" -v
```

---

## 🐛 Quick Troubleshooting

### No RS-485 Communication
1. **Swap A and B wires** (most common issue!)
2. Check baud rate is 115200 on both sides
3. Verify MAX485 modules have power

### No Load Cell Data
1. Check HX711 wiring (DOUT→GPIO25, SCK→GPIO26)
2. Verify load cell wire colors match HX711 terminals
3. Run ESP32 serial monitor to see if load cell initialized

### No WiFi Connection
1. Check SSID and password in code
2. Ensure WiFi is 2.4GHz (ESP32 doesn't support 5GHz)
3. Move ESP32 closer to router

### MQTT Not Publishing
1. Verify WiFi is connected first (ESP32 Serial: "WiFi connected")
2. Check MQTT broker is accessible
3. Try public broker: `broker.hivemq.com`

---

## 📝 Message Protocol

### VEX → ESP32
```
<BRAIN|time|battery%|voltage|current|temp>
Example: <BRAIN|45.23|87.5|12.45|1.23|32.1>
```

### ESP32 → VEX
```
<WEIGHT|kg_value>
Example: <WEIGHT|12.45>

<STATUS|message>
Example: <STATUS|MQTT Connected>
```

---

## 🔍 Monitoring & Debugging

### ESP32 Serial Output
```bash
pio device monitor -b 115200
```

Look for:
- `WiFi connected` - WiFi OK
- `MQTT broker set to: ...` - MQTT config OK
- `Load cell initialized and tared successfully` - Load cell OK
- `Received from VEX: BRAIN|...` - RS-485 receiving OK
- `Published weight: X.XX kg` - MQTT publishing OK

### VEX Brain Screen
Should display:
```
Line 1: VEX V5 <-> ESP32 Bridge
Line 2: Port 21 @ 115200 baud
Line 4: TX to ESP32:
Line 5: Batt:XX% Temp:XX.XC
Line 7: RX from ESP32:
Line 8: (Status messages)
Line 10: Load Cell Weight:
Line 11: XX.XX kg
```

---

## ⏱️ Timing Reference

| Operation | Frequency | Adjustable Constant |
|-----------|-----------|---------------------|
| VEX sends to ESP32 | 1/sec | `SEND_INTERVAL_MS` |
| ESP32 reads load cell | 20/sec | `WEIGHT_READ_INTERVAL` |
| ESP32 publishes MQTT | 1/sec | `MQTT_PUBLISH_INTERVAL` |
| RS-485 polling | 50/sec | `RS485_READ_INTERVAL` |

---

## 📦 File Structure

```
SeniorDesignProject/
├── vex_brain_bidirectional.v5cpp        ← VEX Brain code
├── ESP32-Bidirectional/
│   ├── platformio.ini                   ← PlatformIO config
│   └── src/
│       └── main.cpp                     ← ESP32 code
├── BIDIRECTIONAL_SYSTEM_README.md       ← Full documentation
├── WIRING_INSTRUCTIONS.md               ← Detailed wiring guide
└── QUICK_START.md                       ← This file
```

---

## 🎯 Expected Behavior

1. **Power On**: Both systems initialize, ESP32 connects to WiFi/MQTT
2. **VEX → ESP32**: VEX sends battery data every 1 second
3. **ESP32 → VEX**: ESP32 sends weight data every 50ms
4. **ESP32 → MQTT**: All data published every 1 second
5. **VEX Display**: Shows sent data, received weight, and status
6. **ESP32 Serial**: Shows detailed debug information

---

## 💡 Tips

- **Start Simple**: Test RS-485 with short cable first (<3 feet)
- **Monitor Both**: Keep VEX screen and ESP32 Serial visible
- **Check Basics**: Power, baud rate, A/B wiring are most common issues
- **Use MQTT Explorer**: Great visual tool for monitoring MQTT topics
- **Calibrate Load Cell**: Run HX711 calibration sketch before using this system

---

## 🔗 Key Files to Modify

| File | Lines to Change | What to Change |
|------|-----------------|----------------|
| `ESP32-Bidirectional/src/main.cpp` | 20-21 | WiFi credentials |
| `ESP32-Bidirectional/src/main.cpp` | 24-28 | MQTT broker & topics |
| `ESP32-Bidirectional/src/main.cpp` | 35 | Load cell calibration |
| `vex_brain_bidirectional.v5cpp` | 66 | Smart Port number (if not 21) |

---

## ✅ Success Checklist

- [ ] VEX Brain code uploaded and running
- [ ] ESP32 code uploaded with correct WiFi credentials
- [ ] MAX485 modules wired correctly (A-to-A, B-to-B)
- [ ] HX711 and load cell connected to ESP32
- [ ] Load cell calibrated (calibration_factor set)
- [ ] VEX screen shows "VEX V5 <-> ESP32 Bridge"
- [ ] ESP32 Serial shows "WiFi connected"
- [ ] ESP32 Serial shows "MQTT broker set to: ..."
- [ ] Weight appears on VEX screen
- [ ] VEX battery data appears in ESP32 Serial
- [ ] MQTT client receives data on all topics

---

## 📚 Additional Documentation

- **Full System Details**: See `BIDIRECTIONAL_SYSTEM_README.md`
- **Wiring Diagrams**: See `WIRING_INSTRUCTIONS.md`
- **Troubleshooting**: Both README files have extensive troubleshooting sections

---

## 🆘 Still Having Issues?

1. Check ESP32 Serial Monitor output for error messages
2. Verify all wiring with a multimeter
3. Test each component individually:
   - Load cell: Run basic HX711 example
   - RS-485: Use simple transmit/receive test
   - MQTT: Test with mosquitto_pub/sub
4. Review the detailed troubleshooting sections in the main README

---

**Quick Start Version**: 1.0  
**System Ready in**: ~5 minutes (assuming hardware is wired)
