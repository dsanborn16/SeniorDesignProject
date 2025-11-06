// ============================================================================
// ESP32 - Bidirectional RS-485 Communication with VEX V5 Brain + Load Cell + MQTT
// ============================================================================
// This program implements:
// 1. Bidirectional RS-485 communication with VEX V5 Brain
// 2. HX711 load cell reading
// 3. WiFi connectivity
// 4. MQTT publishing of both load cell data AND VEX Brain data
// 5. Status feedback to VEX Brain
// ============================================================================

#include <Arduino.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "HX711.h"

// ============================================================================
// CONFIGURATION SECTION - MODIFY THESE VALUES FOR YOUR SETUP
// ============================================================================

// --- WiFi Configuration ---
const char* WIFI_SSID = "MCWarrior-Student";
const char* WIFI_PASSWORD = "QxS884LsC8";

// --- MQTT Broker Configuration ---
const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC_WEIGHT = "ESP32/loadCell/weight";
const char* MQTT_TOPIC_BRAIN_BATTERY = "ESP32/vexBrain/battery";
const char* MQTT_TOPIC_BRAIN_VOLTAGE = "ESP32/vexBrain/voltage";
const char* MQTT_TOPIC_BRAIN_STATUS = "ESP32/vexBrain/status";

// --- RS-485 Configuration ---
#define DE_RE_PIN 4              // MAX485 Direction control pin
#define RS485_RX_PIN 16          // ESP32 RX pin (connected to MAX485 RO)
#define RS485_TX_PIN 17          // ESP32 TX pin (connected to MAX485 DI)
const long RS485_BAUD_RATE = 115200;  // MUST match VEX Brain configuration

// --- HX711 Load Cell Configuration ---
#define HX711_DOUT_PIN 25        // HX711 Data Out pin
#define HX711_SCK_PIN 26         // HX711 Clock pin
const float CALIBRATION_FACTOR = 106.53;  // Use your calibrated value

// --- Timing Configuration ---
const unsigned long WEIGHT_READ_INTERVAL = 50;      // Read weight every 50ms
const unsigned long MQTT_PUBLISH_INTERVAL = 1000;    // Publish to MQTT every 1 second
const unsigned long WIFI_RECONNECT_INTERVAL = 5000;  // Try WiFi reconnect every 5 seconds
const unsigned long RS485_READ_INTERVAL = 20;        // Check RS485 every 20ms

// --- Message Protocol Configuration ---
const char MSG_START = '<';
const char MSG_END = '>';
const char MSG_SEPARATOR = '|';

// ============================================================================
// GLOBAL OBJECTS AND VARIABLES
// ============================================================================

// Hardware objects
HardwareSerial RS485Serial(2);  // Use UART2 for RS-485
HX711 loadCell;
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// RS-485 receive buffer
char rxBuffer[256];
int rxBufferIndex = 0;
bool messageInProgress = false;

// Data storage for VEX Brain data
struct BrainData {
  float systemTime;
  float batteryPercent;
  float batteryVoltage;
  float batteryCurrent;
  bool dataValid;
  unsigned long lastUpdate;
};
BrainData brainData = {0, 0, 0, 0, false, 0};

// Load cell data
float currentWeight = 0.0;
bool weightValid = false;

// Timing variables
unsigned long lastWeightRead = 0;
unsigned long lastMqttPublish = 0;
unsigned long lastWifiCheck = 0;
unsigned long lastRS485Read = 0;

// Status tracking
bool wifiConnected = false;
bool mqttConnected = false;

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================

void setupWiFi();
void setupMQTT();
void setupRS485();
void setupLoadCell();
void reconnectWiFi();
void reconnectMQTT();
void readLoadCell();
void readRS485Data();
void processReceivedMessage(const char* message);
void sendWeightToVexBrain();
void sendStatusToVexBrain(const char* status);
void publishToMQTT();
void setRS485Transmit();
void setRS485Receive();

// ============================================================================
// SETUP FUNCTIONS
// ============================================================================

void setupWiFi() {
  Serial.println("Connecting to WiFi...");
  Serial.print("SSID: ");
  Serial.println(WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    wifiConnected = false;
    Serial.println("\nWiFi connection failed. Will retry...");
  }
}

void setupMQTT() {
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  Serial.print("MQTT broker set to: ");
  Serial.println(MQTT_BROKER);
}

void setupRS485() {
  // Configure DE/RE control pin
  pinMode(DE_RE_PIN, OUTPUT);
  setRS485Receive();  // Start in receive mode
  
  // Initialize RS-485 serial port
  RS485Serial.begin(RS485_BAUD_RATE, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);
  
  Serial.println("RS-485 initialized");
  Serial.printf("  Baud: %ld\n", RS485_BAUD_RATE);
  Serial.printf("  RX Pin: %d, TX Pin: %d\n", RS485_RX_PIN, RS485_TX_PIN);
  Serial.printf("  DE/RE Pin: %d\n", DE_RE_PIN);
}

void setupLoadCell() {
  Serial.println("Initializing load cell...");
  
  loadCell.begin(HX711_DOUT_PIN, HX711_SCK_PIN);
  
  if (loadCell.wait_ready_timeout(1000)) {
    loadCell.set_scale(CALIBRATION_FACTOR);
    loadCell.tare();
    weightValid = true;
    Serial.println("Load cell initialized and tared successfully");
  } else {
    weightValid = false;
    Serial.println("Load cell initialization failed");
  }
}

// ============================================================================
// COMMUNICATION FUNCTIONS
// ============================================================================

void setRS485Transmit() {
  digitalWrite(DE_RE_PIN, HIGH);
  delayMicroseconds(100);  // Small delay for transceiver switching
}

void setRS485Receive() {
  digitalWrite(DE_RE_PIN, LOW);
}

void reconnectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      wifiConnected = true;
      Serial.println("\nWiFi reconnected!");
    } else {
      wifiConnected = false;
      Serial.println("\nWiFi reconnection failed");
    }
  }
}

void reconnectMQTT() {
  if (!wifiConnected) {
    return;
  }
  
  if (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    String clientId = "ESP32-VexBridge-";
    clientId += String(random(0xffff), HEX);
    
    if (mqttClient.connect(clientId.c_str())) {
      mqttConnected = true;
      Serial.println("connected");
      
      // Send initialization status to VEX Brain
      sendStatusToVexBrain("MQTT Connected");
    } else {
      mqttConnected = false;
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" - will retry");
    }
  }
}

void readLoadCell() {
  if (!weightValid) {
    return;
  }
  
  if (loadCell.wait_ready_timeout(100)) {
    currentWeight = loadCell.get_units(5);  // Average of 5 readings
    
    // Send weight to VEX Brain
    sendWeightToVexBrain();
    
    Serial.printf("Weight: %.2f kg\n", currentWeight);
  }
}

void readRS485Data() {
  // Check for available data
  while (RS485Serial.available() > 0) {
    char receivedChar = RS485Serial.read();
    
    // Message parsing state machine
    if (receivedChar == MSG_START) {
      messageInProgress = true;
      rxBufferIndex = 0;
    }
    else if (receivedChar == MSG_END && messageInProgress) {
      rxBuffer[rxBufferIndex] = '\0';
      processReceivedMessage(rxBuffer);
      messageInProgress = false;
      rxBufferIndex = 0;
    }
    else if (messageInProgress && rxBufferIndex < sizeof(rxBuffer) - 1) {
      rxBuffer[rxBufferIndex++] = receivedChar;
    }
  }
}

void processReceivedMessage(const char* message) {
  // Expected format: "BRAIN|time|battery%|voltage|current"
  
  Serial.print("Received from VEX: ");
  Serial.println(message);
  
  // Create working copy for parsing
  char msgCopy[256];
  strncpy(msgCopy, message, sizeof(msgCopy) - 1);
  msgCopy[sizeof(msgCopy) - 1] = '\0';
  
  // Parse message type
  char* token = strtok(msgCopy, "|");
  
  if (token != NULL && strcmp(token, "BRAIN") == 0) {
    // Parse Brain data fields
    token = strtok(NULL, "|");  // time
    if (token) brainData.systemTime = atof(token);
    
    token = strtok(NULL, "|");  // battery%
    if (token) brainData.batteryPercent = atof(token);
    
    token = strtok(NULL, "|");  // voltage
    if (token) brainData.batteryVoltage = atof(token);
    
    token = strtok(NULL, "|");  // current
    if (token) brainData.batteryCurrent = atof(token);
    
    brainData.dataValid = true;
    brainData.lastUpdate = millis();
    
    Serial.printf("Brain Data - Battery: %.1f%%, Voltage: %.2fV\n", 
                  brainData.batteryPercent, brainData.batteryVoltage);
  }
}

void sendWeightToVexBrain() {
  // Construct message: <WEIGHT|value>
  char txBuffer[64];
  int msgLen = snprintf(txBuffer, sizeof(txBuffer), "<WEIGHT|%.2f>", currentWeight);
  
  if (msgLen > 0 && msgLen < sizeof(txBuffer)) {
    setRS485Transmit();
    RS485Serial.print(txBuffer);
    RS485Serial.flush();
    setRS485Receive();
  }
}

void sendStatusToVexBrain(const char* status) {
  // Construct message: <STATUS|text>
  char txBuffer[128];
  int msgLen = snprintf(txBuffer, sizeof(txBuffer), "<STATUS|%s>", status);
  
  if (msgLen > 0 && msgLen < sizeof(txBuffer)) {
    setRS485Transmit();
    RS485Serial.print(txBuffer);
    RS485Serial.flush();
    setRS485Receive();
    
    Serial.printf("Sent status to VEX: %s\n", status);
  }
}

void publishToMQTT() {
  if (!mqttConnected) {
    return;
  }
  
  char payload[64];
  
  // Publish load cell weight
  if (weightValid) {
    snprintf(payload, sizeof(payload), "%.2f", currentWeight);
    if (mqttClient.publish(MQTT_TOPIC_WEIGHT, payload)) {
      Serial.printf("Published weight: %s kg\n", payload);
    }
  }
  
  // Publish VEX Brain data if available
  if (brainData.dataValid) {
    // Battery percentage
    snprintf(payload, sizeof(payload), "%.1f", brainData.batteryPercent);
    mqttClient.publish(MQTT_TOPIC_BRAIN_BATTERY, payload);
    
    // Voltage
    snprintf(payload, sizeof(payload), "%.2f", brainData.batteryVoltage);
    mqttClient.publish(MQTT_TOPIC_BRAIN_VOLTAGE, payload);
    
    // Combined status
    snprintf(payload, sizeof(payload), "Batt:%.1f%% Volt:%.2fV Curr:%.2fA", 
             brainData.batteryPercent, brainData.batteryVoltage, brainData.batteryCurrent);
    mqttClient.publish(MQTT_TOPIC_BRAIN_STATUS, payload);
    
    Serial.println("Published VEX Brain data to MQTT");
  }
}

// ============================================================================
// MAIN SETUP AND LOOP
// ============================================================================

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\n===========================================");
  Serial.println("ESP32 Bidirectional VEX Bridge with MQTT");
  Serial.println("===========================================\n");
  
  // Initialize all subsystems
  setupRS485();
  setupLoadCell();
  setupWiFi();
  setupMQTT();
  
  // Initial MQTT connection attempt
  if (wifiConnected) {
    reconnectMQTT();
  }
  
  Serial.println("\nSystem ready - entering main loop");
  Serial.println("-------------------------------------------\n");
  
  // Initialize timing
  lastWeightRead = millis();
  lastMqttPublish = millis();
  lastWifiCheck = millis();
  lastRS485Read = millis();
}

void loop() {
  unsigned long currentMillis = millis();
  
  // --- WiFi Management ---
  if (currentMillis - lastWifiCheck >= WIFI_RECONNECT_INTERVAL) {
    lastWifiCheck = currentMillis;
    
    if (WiFi.status() != WL_CONNECTED) {
      wifiConnected = false;
      reconnectWiFi();
    } else {
      wifiConnected = true;
    }
  }
  
  // --- MQTT Management ---
  if (wifiConnected) {
    if (!mqttClient.connected()) {
      mqttConnected = false;
      reconnectMQTT();
    } else {
      mqttConnected = true;
      mqttClient.loop();  // Process MQTT messages
    }
  }
  
  // --- Read Load Cell ---
  if (currentMillis - lastWeightRead >= WEIGHT_READ_INTERVAL) {
    lastWeightRead = currentMillis;
    readLoadCell();
  }
  
  // --- Read RS-485 Data from VEX Brain ---
  if (currentMillis - lastRS485Read >= RS485_READ_INTERVAL) {
    lastRS485Read = currentMillis;
    readRS485Data();
  }
  
  // --- Publish to MQTT ---
  if (currentMillis - lastMqttPublish >= MQTT_PUBLISH_INTERVAL) {
    lastMqttPublish = currentMillis;
    publishToMQTT();
  }
  
  // Small delay to prevent watchdog issues
  delay(1);
}
