# Wiring Instructions - VEX V5 Brain ↔ ESP32 Bidirectional System

## Overview
This document provides detailed wiring instructions for connecting the VEX V5 Brain to an ESP32 via RS-485, with HX711 load cell integration.

---

## Bill of Materials

### Electronic Components
- 1x VEX V5 Brain
- 1x ESP32 Development Board (ESP32-DevKitC or similar)
- 2x MAX485 RS-485 Transceiver Modules
- 1x HX711 Load Cell Amplifier Module
- 1x Load Cell (Strain Gauge, appropriate capacity for your application)
- 1x Power supply for ESP32 (USB or 5V regulated)
- Twisted pair cable for RS-485 bus (recommended: CAT5e or better, use one pair)
- Jumper wires
- 2x 120Ω resistors (optional, for RS-485 termination on long cable runs)

### VEX V5 Components
- 1x VEX Smart Port cable or equivalent wiring to Smart Port 21

---

## Part 1: VEX V5 Brain to MAX485 #1

### Smart Port 21 Pinout
The VEX V5 Smart Ports use an 8-pin connector. For generic serial communication on Port 21:

| Pin | Function | Wire Color (typical) |
|-----|----------|---------------------|
| 1   | +5V      | Red                 |
| 2   | GND      | Black               |
| 3   | TX (VEX) | White               |
| 4   | RX (VEX) | Green               |
| 5-8 | N/C      | -                   |

### MAX485 Module #1 (VEX Side) Connections

| MAX485 Pin | Connect To | Notes |
|------------|------------|-------|
| VCC        | Smart Port Pin 1 (+5V) | Power for the MAX485 module |
| GND        | Smart Port Pin 2 (GND) | Common ground |
| RO (Receiver Out) | Smart Port Pin 4 (RX) | VEX receives data from this pin |
| DI (Driver In) | Smart Port Pin 3 (TX) | VEX transmits data to this pin |
| DE (Driver Enable) | VCC (+5V) or leave floating* | See note below |
| RE (Receiver Enable) | GND (0V) or leave floating* | See note below |
| A          | RS-485 Bus Line A | Connect to MAX485 #2 |
| B          | RS-485 Bus Line B | Connect to MAX485 #2 |

**Important Note on DE/RE pins:**
The VEX V5 generic serial implementation typically controls direction internally. You can:
- **Option 1**: Tie DE to VCC and RE to GND (always in transmit mode, but VEX usually manages this)
- **Option 2**: Leave DE and RE floating if your MAX485 module has pull-up/pull-down resistors
- **Option 3**: Experiment with both tied to VCC or both tied to GND

For most applications, **tie DE high (+5V) and RE low (GND)** for simplicity.

---

## Part 2: ESP32 to MAX485 #2

### MAX485 Module #2 (ESP32 Side) Connections

| MAX485 Pin | Connect To | Notes |
|------------|------------|-------|
| VCC        | ESP32 3.3V or 5V | Check your module's voltage rating |
| GND        | ESP32 GND | Common ground |
| RO (Receiver Out) | ESP32 GPIO 16 | ESP32 receives data (UART2 RX) |
| DI (Driver In) | ESP32 GPIO 17 | ESP32 transmits data (UART2 TX) |
| DE (Driver Enable) | ESP32 GPIO 4 | Direction control (HIGH = transmit) |
| RE (Receiver Enable) | ESP32 GPIO 4 | Direction control (LOW = receive) |
| A          | RS-485 Bus Line A | Connect to MAX485 #1 |
| B          | RS-485 Bus Line B | Connect to MAX485 #1 |

**Important**: DE and RE are tied together and connected to GPIO 4. This single pin controls the direction of the RS-485 transceiver.

### ESP32 GPIO Summary

| GPIO Pin | Function | Connection |
|----------|----------|------------|
| GPIO 4   | RS-485 Direction Control | MAX485 DE & RE (tied together) |
| GPIO 16  | UART2 RX | MAX485 RO |
| GPIO 17  | UART2 TX | MAX485 DI |
| GPIO 25  | HX711 Data | HX711 DOUT |
| GPIO 26  | HX711 Clock | HX711 SCK |
| 3.3V     | Power | MAX485 VCC, HX711 VCC |
| GND      | Ground | MAX485 GND, HX711 GND, Load Cell Shield GND |

---

## Part 3: HX711 Load Cell Amplifier to ESP32

### HX711 Module Connections

| HX711 Pin | Connect To | Notes |
|-----------|------------|-------|
| VCC       | ESP32 3.3V | Some modules work with 5V, check your module |
| GND       | ESP32 GND | Common ground |
| DOUT (DT) | ESP32 GPIO 25 | Data output from HX711 |
| SCK (PD_SCK) | ESP32 GPIO 26 | Clock signal to HX711 |
| E+ (Excitation+) | Load Cell Red Wire | Positive excitation voltage |
| E- (Excitation-) | Load Cell Black Wire | Negative excitation voltage |
| A+ (Signal+) | Load Cell White Wire | Positive signal |
| A- (Signal-) | Load Cell Green Wire | Negative signal |

**Note**: Load cell wire colors may vary by manufacturer. Common color codes:
- **Red**: Excitation+
- **Black**: Excitation-
- **White** or **Green**: Signal+
- **Green** or **White**: Signal-

Consult your load cell's datasheet if colors differ.

---

## Part 4: RS-485 Bus Wiring

### Connecting the Two MAX485 Modules

Use **twisted pair cable** for best noise immunity. CAT5e ethernet cable works excellently—use one pair for A and B.

| MAX485 #1 (VEX Side) | MAX485 #2 (ESP32 Side) |
|----------------------|------------------------|
| A                    | A                      |
| B                    | B                      |

### Cable Length Considerations

| Cable Length | Termination Required? | Notes |
|--------------|-----------------------|-------|
| < 10 feet (3m) | No | Termination usually not needed |
| 10-100 feet | Optional | May improve reliability |
| > 100 feet | Yes | Use 120Ω resistors across A-B at both ends |

### Adding Termination Resistors (Optional)
If you experience communication errors over longer cables:
1. Connect a 120Ω resistor between A and B at the VEX side MAX485
2. Connect a 120Ω resistor between A and B at the ESP32 side MAX485

---

## Complete Wiring Diagram (ASCII)

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           VEX V5 BRAIN                                  │
│                         Smart Port 21                                   │
│  ┌────────┐                                                             │
│  │ Pin 1  │───── +5V ────────────────┐                                  │
│  │ Pin 2  │───── GND ────────────────┼─────────┐                        │
│  │ Pin 3  │───── TX ─────────────┐   │         │                        │
│  │ Pin 4  │───── RX ─────────┐   │   │         │                        │
│  └────────┘                  │   │   │         │                        │
└──────────────────────────────┼───┼───┼─────────┼────────────────────────┘
                               │   │   │         │
                          ┌────┴───┴───┴─────────┴───┐
                          │   MAX485 Module #1        │
                          │  (VEX Side)               │
                          │   RO ←────── Pin 4 (RX)   │
                          │   DI ←────── Pin 3 (TX)   │
                          │   DE ←────── +5V          │
                          │   RE ←────── GND          │
                          │   VCC ←───── +5V          │
                          │   GND ←───── GND          │
                          │   A  ─────── (Twisted     │
                          │   B  ─────── Pair Cable)  │
                          └────┬───┬─────────────────┘
                               │   │
                   RS-485 Bus  │   │  (Twisted Pair)
                   A ──────────┘   └────────── B
                   │                           │
                   │                           │
                          ┌────┴───┴─────────────────┐
                          │   MAX485 Module #2        │
                          │  (ESP32 Side)             │
                          │   RO ─────→ GPIO 16       │
                          │   DI ←──── GPIO 17        │
                          │   DE ←──── GPIO 4 ────┐   │
                          │   RE ←──── GPIO 4 ────┘   │
                          │   VCC ←─── 3.3V           │
                          │   GND ←─── GND            │
                          └───────────────────────────┘
                                      │
                                      │
            ┌─────────────────────────┴─────────────────────────────┐
            │                    ESP32 Board                         │
            │  GPIO 4  ──→ MAX485 DE/RE                             │
            │  GPIO 16 ──→ MAX485 RO (RX)                           │
            │  GPIO 17 ──→ MAX485 DI (TX)                           │
            │  GPIO 25 ──→ HX711 DOUT                               │
            │  GPIO 26 ──→ HX711 SCK                                │
            │  3.3V ─────→ MAX485 VCC, HX711 VCC                    │
            │  GND ──────→ MAX485 GND, HX711 GND                    │
            └───────────────────────┬───────────────────────────────┘
                                    │
                          ┌─────────┴──────────┐
                          │   HX711 Module     │
                          │   DOUT ←─ GPIO 25  │
                          │   SCK  ←─ GPIO 26  │
                          │   VCC  ←─ 3.3V     │
                          │   GND  ←─ GND      │
                          │   E+  ─→ Load Cell │
                          │   E-  ─→ Load Cell │
                          │   A+  ─→ Load Cell │
                          │   A-  ─→ Load Cell │
                          └────────────────────┘
                                    │
                          ┌─────────┴──────────┐
                          │    Load Cell       │
                          │  (Strain Gauge)    │
                          │   Red    ← E+      │
                          │   Black  ← E-      │
                          │   White  ← A+      │
                          │   Green  ← A-      │
                          └────────────────────┘
```

---

## Step-by-Step Wiring Procedure

### Step 1: Wire VEX Side MAX485
1. Connect MAX485 #1 VCC to Smart Port 21 Pin 1 (+5V)
2. Connect MAX485 #1 GND to Smart Port 21 Pin 2 (GND)
3. Connect MAX485 #1 RO to Smart Port 21 Pin 4 (RX)
4. Connect MAX485 #1 DI to Smart Port 21 Pin 3 (TX)
5. Connect MAX485 #1 DE to +5V
6. Connect MAX485 #1 RE to GND
7. Leave A and B pins for now

### Step 2: Wire ESP32 Side MAX485
1. Connect MAX485 #2 VCC to ESP32 3.3V
2. Connect MAX485 #2 GND to ESP32 GND
3. Connect MAX485 #2 RO to ESP32 GPIO 16
4. Connect MAX485 #2 DI to ESP32 GPIO 17
5. Connect MAX485 #2 DE to ESP32 GPIO 4
6. Connect MAX485 #2 RE to ESP32 GPIO 4 (same pin as DE)
7. Leave A and B pins for now

### Step 3: Connect RS-485 Bus
1. Cut a length of twisted pair cable
2. Connect one wire to A on both MAX485 modules
3. Connect the other wire to B on both MAX485 modules
4. **Important**: Keep polarity consistent (A-to-A, B-to-B)

### Step 4: Wire HX711 to ESP32
1. Connect HX711 VCC to ESP32 3.3V
2. Connect HX711 GND to ESP32 GND
3. Connect HX711 DOUT to ESP32 GPIO 25
4. Connect HX711 SCK to ESP32 GPIO 26

### Step 5: Connect Load Cell to HX711
1. Connect load cell Red wire to HX711 E+
2. Connect load cell Black wire to HX711 E-
3. Connect load cell White wire to HX711 A+
4. Connect load cell Green wire to HX711 A-
   (Verify wire colors with your load cell's documentation)

### Step 6: Power Connections
1. Power VEX V5 Brain via battery
2. Power ESP32 via USB or external 5V supply
3. Verify all ground connections are common

---

## Testing the Wiring

### Test 1: Power Check
1. Power on VEX V5 Brain
2. Power on ESP32
3. Verify no smoke, unusual heat, or burning smells
4. Check that MAX485 modules have power LEDs illuminated (if equipped)

### Test 2: ESP32 Serial Monitor
1. Connect ESP32 to computer via USB
2. Open Serial Monitor at 115200 baud
3. Verify ESP32 boots and connects to WiFi
4. Look for "RS-485 initialized" message

### Test 3: Load Cell Reading
1. Check Serial Monitor for "Load cell initialized and tared successfully"
2. Apply weight to load cell
3. Verify weight readings appear in Serial Monitor

### Test 4: RS-485 Communication
1. Upload VEX Brain code and run
2. Check VEX Brain screen for "VEX V5 <-> ESP32 Bridge"
3. Monitor ESP32 Serial output for "Received from VEX:" messages
4. Check VEX Brain screen for weight display

### Test 5: MQTT Publishing
1. Use MQTT Explorer or similar tool
2. Connect to broker.hivemq.com:1883
3. Subscribe to `ESP32/#` (all topics)
4. Verify data appears from both load cell and VEX Brain

---

## Troubleshooting

### No RS-485 Communication
- **Swap A and B wires** - Polarity might be reversed
- Check that VEX is transmitting (monitor with oscilloscope if available)
- Verify baud rates match (115200 on both sides)
- Test with very short cable first (eliminate cable issues)
- Check DE/RE pins on VEX side MAX485

### Load Cell Not Working
- Verify HX711 VCC is 3.3V (some modules require 5V)
- Check load cell wire connections (consult datasheet)
- Ensure load cell is mechanically installed correctly
- Try running HX711 calibration sketch first

### ESP32 WiFi Not Connecting
- Verify SSID and password are correct
- Ensure WiFi is 2.4GHz (ESP32 doesn't support 5GHz)
- Check WiFi signal strength

### Intermittent Communication
- Add RS-485 termination resistors (120Ω across A-B at both ends)
- Use shielded twisted pair cable
- Keep cable away from power lines and motors
- Shorten cable length if possible

---

## Safety Notes

⚠️ **Important Safety Information**:
- Never connect/disconnect components while powered on
- Verify voltage levels before making connections (3.3V vs 5V)
- Use proper wire gauge for power connections
- Ensure proper grounding to prevent damage from static discharge
- Do not exceed maximum current ratings of any component
- Keep wiring organized and secure to prevent shorts
- If using VEX in a robot, secure all wiring to prevent snagging

---

## Pin Configuration Quick Reference

### VEX V5 Smart Port 21
- TX: Pin 3
- RX: Pin 4
- +5V: Pin 1
- GND: Pin 2

### ESP32 GPIOs
- GPIO 4: RS-485 Direction (DE/RE)
- GPIO 16: RS-485 RX (RO)
- GPIO 17: RS-485 TX (DI)
- GPIO 25: HX711 Data (DOUT)
- GPIO 26: HX711 Clock (SCK)

### MAX485 (Both Sides)
- A & B: RS-485 differential pair
- DE: Driver Enable (HIGH = transmit)
- RE: Receiver Enable (LOW = receive)
- RO: Receiver Output (to MCU RX)
- DI: Driver Input (from MCU TX)

---

## Additional Resources

- [VEX V5 Generic Serial API Documentation](https://api.vexcode.cloud/v5/)
- [MAX485 Datasheet](https://www.ti.com/lit/ds/symlink/max485.pdf)
- [HX711 Datasheet](https://www.mouser.com/datasheet/2/813/hx711_english-1022875.pdf)
- [ESP32 Pinout Reference](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
- [RS-485 Protocol Overview](https://en.wikipedia.org/wiki/RS-485)

---

**Document Version**: 1.0  
**Last Updated**: November 2025  
**Compatible Hardware**: VEX V5 Brain, ESP32 (all variants), MAX485 modules, HX711 load cell amplifiers
