#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

// LoRa Pins
#define ss 4
#define rst 14
#define dio0 2

// GPS Pins
#define GPS_RX_PIN 33
#define GPS_TX_PIN 32

// Frequency for LoRa (adjust based on your region)
#define LORA_FREQUENCY 866E6

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
int counter = 1; // Start counter from 1

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);

  // GPS Module Initialization
  gpsSerial.begin(115200, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  // LoRa Module Initialization
  Serial.println("Initializing LoRa...");
  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("LoRa initialization failed. Retrying...");
    delay(500);
  }
  LoRa.setSyncWord(0xF3); // Set sync word
  Serial.println("LoRa Initialized!");

  // Print Instructions
  Serial.println("\nAwaiting GPS data...");
}

void loop() {
  // Read GPS data
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        // Get latitude and longitude
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();

        // Prepare LoRa message with counter
        String message = "Count: " + String(counter) + ", Lat: " + String(latitude, 7) + ", Long: " + String(longitude, 7);

        // Transmit message via LoRa
        Serial.println("Sending packet: " + message);
        LoRa.beginPacket();
        LoRa.print(message);
        LoRa.endPacket();

        // Debugging: Display GPS and LoRa info on Serial Monitor
        Serial.print("View on Google Maps: http://maps.google.com/maps?q=");
        Serial.println(String(latitude, 7) + "," + String(longitude, 7));
        Serial.println("Packet Sent!");
        Serial.println();

        // Increment counter
        counter++;

        delay(1000); // Small delay before next packet
      } else {
        Serial.println("Waiting for valid GPS data...");
      }
    }
  }

  delay(10000); // Delay before checking GPS data again
}
