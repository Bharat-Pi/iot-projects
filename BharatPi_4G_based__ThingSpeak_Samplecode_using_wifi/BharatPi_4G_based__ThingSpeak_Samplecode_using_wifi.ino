/*************************************************************************
   PROJECT NAME: Bharat Pi ThingSpeak_Samplecode_using_wifi
   AUTHOR: Bharat Pi
   CREATED DATE: 06/05/2024
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   VERSION: 0.1.0

   DESCRIPTION: BharatPi_4G_based__ThingSpeak_Samplecode_using_wifi

   REVISION HISTORY TABLE:
   ------------------------------------------
   Date      | Firmware Version | Comments
   ------------------------------------------
   06/05/2024 -    0.1.0       -    Initial release of sample script.
 *************************************************************************/

#include <WiFi.h>
#include "DHT.h"

// WiFi credentials
const char* ssid = "smile123"; // change the pass and user id
const char* password = "123456789";

// Thingspeak API key
const String apiKey = "//////////";// use ur api key

// Thingspeak server
const char* server = "api.thingspeak.com";

// DHT sensor pin and type
#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("Serial Begin");

  // Initialize DHT sensor
  dht.begin();
        
  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  // Read DHT sensor data
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again)
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send data to Thingspeak
  if (sendToThingspeak(humidity, temperature)) {
    Serial.println("Data sent to Thingspeak successfully");
  } else {
    Serial.println("Failed to send data to Thingspeak");
  }
  
  // Delay before next reading
  delay(10000); // Delay for 10 seconds (adjust as needed)
}

bool sendToThingspeak(float humidity, float temperature) {
  WiFiClient client;

  // Create HTTP request body
  String requestBody = "api_key=" + apiKey;
  requestBody += "&field1=" + String(humidity);
  requestBody += "&field2=" + String(temperature);
  
  // Send HTTP POST request to Thingspeak
  if (client.connect(server, 80)) {
    client.println("POST /update HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(requestBody.length()));
    client.println();
    client.print(requestBody);
    delay(500); // Allow time for response
    client.stop();
    return true; // Data sent successfully
  } else {
    Serial.println("Connection to Thingspeak failed");
    return false; // Failed to send data
  }
}
