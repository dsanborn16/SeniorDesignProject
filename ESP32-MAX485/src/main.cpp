#include <Arduino.h>

// Define the pins used for the MAX485
#define RXD2_PIN 16
#define TXD2_PIN 17
#define DIR_PIN  4  // Direction control pin for MAX485

void setup() {
    // Start the main serial port for debugging
    Serial.begin(115200);
    Serial.println("ESP32 to VEX Brain Communication");

    // Configure the direction pin as an output
    pinMode(DIR_PIN, OUTPUT);

    // Begin the second serial port for RS-485 communication
    // VEX V5 uses a baud rate of 115200
    Serial2.begin(115200, SERIAL_8N1, RXD2_PIN, TXD2_PIN);
}

void loop() {
    // --- Sending Data to VEX Brain ---
    // Set MAX485 to transmit mode
    digitalWrite(DIR_PIN, HIGH);
    delay(1); // Small delay to allow the transceiver to switch

    // Send a message
    Serial2.println("Hello VEX Brain from ESP32!");
    Serial.println("Sent message to VEX.");
    
    // IMPORTANT: Wait for the serial buffer to be empty before switching direction
    Serial2.flush();

    // --- Receiving Data from VEX Brain ---
    // Set MAX485 back to receive mode
    digitalWrite(DIR_PIN, LOW);

    // Check if the VEX Brain has sent any data back
    if (Serial2.available()) {
        String response = Serial2.readStringUntil('\n');
        Serial.print("Received from VEX: ");
        Serial.println(response);
    }
    
    // Wait for 2 seconds before repeating
    delay(2000);
}