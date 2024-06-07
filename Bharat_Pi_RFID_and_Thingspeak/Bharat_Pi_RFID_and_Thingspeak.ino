/*************************************************************************
   PROJECT NAME: Bharat Pi RFID_and_Thingspeak
   AUTHOR: Bharat Pi
   CREATED DATE: 15/05/2024
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   VERSION: 0.1.1

   DESCRIPTION: Bharat Pi RFID_and_Thingspeak
    RFID Module pin connection
    SDA (SS)	-> GPIO21
    SCK	->  GPIO 18 
    MOSI	-> GPIO 23
    MISO ->	GPIO 19
    RST	-> GPIO27
    3.3V	-> 3.3V pin
    GND ->	GND pin 

   REVISION HISTORY TABLE:
   ------------------------------------------
   Date      | Firmware Version | Comments
   ------------------------------------------
   15/05/2024 -    0.1.0       -    Initial release of sample script.
 *************************************************************************/

#include <WiFi.h>
#include <HTTPClient.h>
#include <MFRC522.h>
#include <SPI.h>

#define SS_PIN 21
#define RST_PIN 27

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

// Valid UIDs and corresponding student names
byte validUIDs[][4] = {
  {0xA3, 0x57, 0xB3, 0x30},  // UID of student 1
  {0x53, 0x15, 0x57, 0xC8},  // UID of student 2
  // Add more valid UIDs here
};
const char* studentNames[] = {
  "Bharatpi",  // Name of student 1
  "Welcome",   // Name of student 2
  // Add more student names here
};

const char* ssid = "smile123";
const char* password = "123456789";
const char* server = "http://api.thingspeak.com/update";
const char* apiKey = "1ZQ9LAB3G9CYY6GA";  // change the api key 

void setup() {
  Serial.begin(115200);
  SPI.begin();       // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String rfid = "";
  // Read the RFID tag
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    rfid += String(mfrc522.uid.uidByte[i], HEX);
  }
  rfid.toUpperCase();
  Serial.println("RFID tag: " + rfid);

  // Match the UID
  String studentName = "Unknown";
  for (byte i = 0; i < sizeof(validUIDs) / sizeof(validUIDs[0]); i++) {
    if (memcmp(mfrc522.uid.uidByte, validUIDs[i], mfrc522.uid.size) == 0) {
      studentName = studentNames[i];
      break;
    }
  }
  Serial.println("Student name: " + studentName);

  // Send data to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "?api_key=" + apiKey + "&field2=" + studentName + "&field1=" + rfid;
    Serial.println("URL: " + url); // Debugging URL
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Response payload: " + payload); // Debugging payload
    } else {
      Serial.println("Error in HTTP request: " + String(http.errorToString(httpCode).c_str()));
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }

  delay(10000); // Send data every 10 seconds
}
