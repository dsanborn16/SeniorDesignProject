# VEX V5 Brain ↔ ESP32 Bidirectional Communication System

## 🎯 Project Overview

This project implements **bidirectional communication** between a VEX V5 Brain and an ESP32 microcontroller via RS-485, with integrated load cell monitoring and MQTT cloud publishing.

### Key Features
- ✅ **Bidirectional RS-485 Communication** between VEX V5 Brain and ESP32
- ✅ **Load Cell Weight Monitoring** using HX711 amplifier
- ✅ **MQTT Cloud Publishing** of all sensor data
- ✅ **Real-Time Display** on VEX Brain LCD screen
- ✅ **Auto-Recovery** for WiFi and MQTT connections

---

## 📁 Project Structure

```
SeniorDesignProject/
│
├── vex_brain_bidirectional.v5cpp        ← Main VEX Brain code (USE THIS)
├── ESP32-Bidirectional/                 ← Main ESP32 project (USE THIS)
│   ├── src/main.cpp                     ← ESP32 firmware
│   └── platformio.ini                   ← PlatformIO config
│
├── QUICK_START.md                       ← Start here! Quick setup guide
├── WIRING_INSTRUCTIONS.md               ← Hardware wiring guide
├── BIDIRECTIONAL_SYSTEM_README.md       ← Complete documentation
│
└── Legacy folders (for reference only):
    ├── ESP32-MAX485/                    ← Old one-way TX example
    ├── ESP32-Scale/                     ← Old scale-only example
    ├── mac485.v5cpp                     ← Old RX-only VEX code
    └── v5esp.v5cpp                      ← Old TX-only VEX code
```

---

## 🚀 Quick Start

### 1. Read the Documentation (5 minutes)
Start with **`QUICK_START.md`** - it has everything you need to get running in 5 minutes!

### 2. Configure WiFi (1 minute)
Edit `ESP32-Bidirectional/src/main.cpp`:
```cpp
const char* WIFI_SSID = "YourWiFiName";        // Line 20
const char* WIFI_PASSWORD = "YourPassword";     // Line 21
const float CALIBRATION_FACTOR = 106.53;        // Line 35 - Use your calibrated value
```

### 3. Upload Code (3 minutes)
**VEX Brain:**
- Open `vex_brain_bidirectional.v5cpp` in VEXcode V5
- Download to VEX Brain

**ESP32:**
```powershell
cd ESP32-Bidirectional
pio run --target upload
pio device monitor
```

### 4. Wire Hardware (10 minutes)
Follow the detailed wiring guide in `WIRING_INSTRUCTIONS.md`

**Done! 🎉**

---

## 📊 System Architecture

```
┌─────────────┐      RS-485       ┌─────────────┐
│  VEX Brain  │ ←────────────────→│    ESP32    │
│             │   (MAX485 x1)     │             │
│  Sends:     │                   │  Receives:  │
│  - Battery  │                   │  - Brain    │
│  - Voltage  │                   │    data     │
│  - Current  │                   │  Sends:     │
│  - Temp     │                   │  - Weight   │
│             │                   │             │
│  Receives:  │                   │  Reads:     │
│  - Weight   │                   │  - HX711    │
│             │                   │  - Load     │
└─────────────┘                   │    Cell     │
                                  └──────┬──────┘
                                         │ WiFi
                                         ▼
                                  ┌─────────────┐
                                  │MQTT Broker  │
                                  │(Cloud)      │
                                  └─────────────┘
```

---

## 📡 MQTT Topics

The system publishes to these topics every second:

```
ESP32/loadCell/weight          → Weight in kg
ESP32/vexBrain/battery         → Battery %
ESP32/vexBrain/temperature     → Temperature °C
ESP32/vexBrain/status          → Combined status string
```

Test with:
```powershell
mosquitto_sub -h broker.hivemq.com -t "ESP32/#" -v
```

---

## 🔧 Hardware Requirements

### VEX Side
- 1x VEX V5 Brain
- 1x MAX485 RS-485 transceiver
- Smart Port cable (Port 21)

### ESP32 Side
- 1x ESP32 Dev Board
- 1x MAX485 RS-485 transceiver
- 1x HX711 Load Cell Amplifier
- 1x Load Cell (strain gauge)
- USB cable for programming

### RS-485 Bus
- Twisted pair cable (CAT5e recommended)
- Connect A-to-A and B-to-B between MAX485 modules

---

## 📚 Documentation Guide

| Document | Purpose | When to Read |
|----------|---------|--------------|
| **QUICK_START.md** | Fast 5-minute setup | Read first! |
| **WIRING_INSTRUCTIONS.md** | Hardware connections | During assembly |
| **BIDIRECTIONAL_SYSTEM_README.md** | Complete documentation | For deep dive |
| **INDEX.md** | Navigation guide | For finding info |

---

## 🐛 Troubleshooting

### No RS-485 Communication?
→ **Swap A and B wires** (most common issue!)

### Load Cell Not Working?
→ Check HX711 wiring and calibration factor

### WiFi Won't Connect?
→ Ensure 2.4GHz WiFi, check SSID/password

### MQTT Not Publishing?
→ Fix WiFi connection first

**More troubleshooting**: See `QUICK_START.md` and `BIDIRECTIONAL_SYSTEM_README.md`

---

## 🎓 Development Environment Setup

Choose your preferred IDE:

## Table of Contents
- [PlatformIO (VS Code) Setup](#setting-up-vs-code-with-platformio) (Recommended)  
- [Arduino IDE Setup](#setting-up-arduino-ide)
- [MQTT Testing Tools](#mqtt-testing-tools) 
  
## Setting up VS Code with PlatformIO

> **Note**: This is the recommended development environment for the ESP32 code in this project.

### Install VS Code  
If you don't have it already, download and install Visual Studio Code from the [official website](https://code.visualstudio.com/).  

---
### Install PlatformIO IDE  
- Open VS Code
- Go to the Extensions view by clicking on the Extensions icon in the Activity Bar on the side of the window or by pressing `Ctrl+Shift+X`
- Search for "PlatformIO IDE" and install it
- After installation, you may be prompted to reload VS Code
  
### Open the ESP32-Bidirectional Project:  
---
- In VS Code, go to **File > Open Folder**
- Navigate to and select the `ESP32-Bidirectional` folder
- PlatformIO will automatically detect the `platformio.ini` file and configure the project
- All required libraries (HX711 and PubSubClient) are already specified in the config

---
### Configure WiFi Settings
- Open `src/main.cpp`
- Find lines 20-21 and update with your WiFi credentials:
```cpp
const char* WIFI_SSID = "YourWiFiName";
const char* WIFI_PASSWORD = "YourPassword";
```
- Update the load cell calibration factor on line 35

---
### Uploading the code  
- **Connect your ESP32** to your computer via USB
- In VS Code, click the **PlatformIO: Upload** button in the status bar at the bottom of the window (it looks like a right-pointing arrow)
- PlatformIO will compile the code and upload it to your ESP32
- Click the **PlatformIO: Serial Monitor** button (plug icon) to view debug output

---

## Setting up Arduino IDE

> **Note**: PlatformIO (VS Code) is recommended, but Arduino IDE also works.

### Add the ESP32 Board Manager URL

- Open the Arduino IDE
- Go to **File > Preferences**
- In the "Additional Board Manager URLs" field, enter the following URL:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
- If you already have other URLs in this field, you can separate them with a comma
- Click **OK**

---
### Install the ESP32 Board Package  

- Go to **Tools > Board > Boards Manager...**
- In the search bar, type "**esp32**"
- You should see "esp32 by Espressif Systems". Click the **Install** button
- The installation process will begin. It may take a few minutes to complete

---
### Install Required Libraries

- Go to **Sketch > Include Library > Manage Libraries...**
- Search for and install:
  - **HX711 Arduino Library** by Bogdan Necula
  - **PubSubClient** by Nick O'Leary

---
### Select Your ESP32 Board  

- Once the installation is complete, close the Boards Manager window
- Go to **Tools > Board > ESP32 Arduino**
- Select the specific board you are using (e.g., "ESP32 Dev Module")
- Go to **Tools > Port** and select the COM port that your ESP32 is connected to

---
### Open and Upload the Code

- Copy the contents of `ESP32-Bidirectional/src/main.cpp` into a new Arduino sketch
- Update WiFi credentials and calibration factor
- Click **Upload** (right arrow button)

After following these steps, your Arduino IDE should recognize your ESP32 board, and you'll be able to upload sketches to it.  

**If your board is still not recognized, you likely need to install USB drivers - see below**  

---

## MQTT Testing Tools

### Option 1: MQTT Explorer (Recommended)
- Download from [mqtt-explorer.com](http://mqtt-explorer.com/)
- Connect to `broker.hivemq.com` on port `1883`
- Subscribe to `ESP32/#` to see all topics

### Option 2: Mosquitto Command Line
Install mosquitto clients and use:
```powershell
mosquitto_sub -h broker.hivemq.com -t "ESP32/#" -v
```

### Option 3: Online MQTT Client
Use [HiveMQ WebSocket Client](http://www.hivemq.com/demos/websocket-client/) in your browser

---

## 📖 Additional Resources

- **VEXcode V5**: Download from [vexrobotics.com](https://www.vexrobotics.com/vexcode)
- **ESP32 Documentation**: [docs.espressif.com](https://docs.espressif.com/)
- **PlatformIO Docs**: [docs.platformio.org](https://docs.platformio.org/)
- **HX711 Library**: [github.com/bogde/HX711](https://github.com/bogde/HX711)
- **MQTT Tutorial**: [mqtt.org](https://mqtt.org/)

---

## 🤝 Contributing

This project is for educational purposes. Feel free to:
- Report issues
- Suggest improvements
- Fork and modify for your own use

---

## 📧 Support

For questions or issues:
1. Check the troubleshooting sections in documentation files
2. Review the wiring instructions carefully
3. Monitor ESP32 serial output for error messages
4. Test each subsystem individually

---

## ✅ Success Checklist

- [ ] VEXcode V5 installed
- [ ] PlatformIO or Arduino IDE set up
- [ ] WiFi credentials configured in code
- [ ] Load cell calibration factor set
- [ ] All hardware components available
- [ ] VEX Brain code uploaded
- [ ] ESP32 code uploaded
- [ ] Hardware wired per instructions
- [ ] VEX screen shows "VEX V5 <-> ESP32 Bridge"
- [ ] ESP32 connects to WiFi and MQTT
- [ ] Weight appears on VEX screen
- [ ] MQTT client receives data

---

## 📝 License

This project is provided for educational purposes as part of a Senior Design project.

---

**Project Status**: ✅ Complete and Ready to Use

**Last Updated**: November 2025