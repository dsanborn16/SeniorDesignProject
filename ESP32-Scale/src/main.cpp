#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "HX711.h"

// --- Configuration Section ---
// Fill in your details below

// WiFi Credentials
const char* ssid = "LilDon";
const char* password = "LilyandDon219";

// MQTT Broker Details
const char* mqtt_server = "broker.hivemq.com"; // IP address of your broker
const int mqtt_port = 1883;
const char* mqtt_topic = "ESP32/loadCell/Weight"; // Topic to publish weight data

// HX711 Calibration
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;
float calibration_factor = 106.53; // IMPORTANT: Use the value you found during calibration

// --- End of Configuration ---


// Globals
WiFiClient espClient;
PubSubClient client(espClient);

HX711 scale1; // MODIFIED: First scale object

long lastMsg = 0;
char msg[50];

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32-DualScaleClient-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);

  // Initialize Scale 1
  scale1.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale1.set_scale(calibration_factor);
  scale1.tare(); 
  Serial.println("Scale 1 initialized and tared.");

  Serial.println("Ready to weigh.");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) { // Publish every 5 seconds
    lastMsg = now;

    // --- Read and Publish for Scale 1 ---
    if (scale1.is_ready()) {
      float weight1 = scale1.get_units(5);
      Serial.printf("Scale 1 Weight: %.2f kg\n", weight1);

      snprintf(msg, 50, "%.2f", weight1);
      client.publish(mqtt_topic, msg);
    } else {
      Serial.println("Scale 1 not found.");
    }

    Serial.println("--------------------");
  }
}