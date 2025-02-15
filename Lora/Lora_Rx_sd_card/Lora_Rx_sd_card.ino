/*************************************************************************
    PROJECT NAME: Bharat Pi Lora_Rx
    AUTHOR: Bharat Pi
    CREATED DATE: 08/11/2023
    COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
    VERSION: 0.1.0

    DESCRIPTION: Bharat Pi Lora_Rx

    REVISION HISTORY TABLE:
    ------------------------------------------
    Date      | Firmware Version | Comments
    ------------------------------------------
    08/11/2023 -    0.1.0       -     Initial release of sample script.
 *************************************************************************/

#include <SPI.h>
#include <LoRa.h>
#include <SD.h>

// define the pins used by the transceiver module
#define ss 4
#define rst 14
#define dio0 2

void setup() {
  // initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver");

  // setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  // replace the LoRa.begin(---E-) argument with your location's frequency 
  // 433E6 for Asia
  // 866E6 for Europe
  // 915E6 for North America
  while (!LoRa.begin(866E6)) {
    Serial.println(".");
    delay(500);
  }

  // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");

  // initialize SD card
  if (!SD.begin()) {
    Serial.println("SD card initialization failed!");
    while (true); // Halt the program if SD card initialization fails
  }
  Serial.println("SD card initialized.");
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    String LoRaData = "";
    while (LoRa.available()) {
      LoRaData += (char)LoRa.read();
    }
    Serial.print(LoRaData);

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

    // store the received message in the SD card
    File dataFile = SD.open("/datalog.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.println(LoRaData);
      dataFile.close();
      Serial.println("Data written to SD card.");
    } else {
      // If file didn't open, handle silently or retry logic can be added here
      // Uncomment below line to retry (optional)
      // loop();
    }
  }
}
