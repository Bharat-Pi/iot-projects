#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "iPhone";
const char* password = "123456789";

AsyncWebServer server(80);

// Define GPIO pins for LEDs
const int thumbLedPin = 33;
const int indexLedPin = 32;
const int middleLedPin = 25;
const int ringLedPin = 27;
const int pinkyLedPin = 26;

void setup() {
  Serial.begin(115200);
  
  pinMode(thumbLedPin, OUTPUT);
  pinMode(indexLedPin, OUTPUT);
  pinMode(middleLedPin, OUTPUT);
  pinMode(ringLedPin, OUTPUT);
  pinMode(pinkyLedPin, OUTPUT);

  digitalWrite(thumbLedPin, LOW);
  digitalWrite(indexLedPin, LOW);
  digitalWrite(middleLedPin, LOW);
  digitalWrite(ringLedPin, LOW);
  digitalWrite(pinkyLedPin, LOW);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {  // Try for 20 seconds
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Define HTTP request handlers for each LED
    server.on("/led/thumb/on", HTTP_GET, [](AsyncWebServerRequest *request){
      digitalWrite(thumbLedPin, HIGH);
      request->send(200, "text/plain", "Thumb LED is ON");
    });

    server.on("/led/thumb/off", HTTP_GET, [](AsyncWebServerRequest *request){
      digitalWrite(thumbLedPin, LOW);
      request->send(200, "text/plain", "Thumb LED is OFF");
    });

    server.on("/led/index/on", HTTP_GET, [](AsyncWebServerRequest *request){
      digitalWrite(indexLedPin, HIGH);
      request->send(200, "text/plain", "Index finger LED is ON");
    });

    server.on("/led/index/off", HTTP_GET, [](AsyncWebServerRequest *request){
      digitalWrite(indexLedPin, LOW);
      request->send(200, "text/plain", "Index finger LED is OFF");
    });

    server.on("/led/middle/on", HTTP_GET, [](AsyncWebServerRequest *request){
      digitalWrite(middleLedPin, HIGH);
      request->send(200, "text/plain", "Middle finger LED is ON");
    });

    server.on("/led/middle/off", HTTP_GET, [](AsyncWebServerRequest *request){
      digitalWrite(middleLedPin, LOW);
      request->send(200, "text/plain", "Middle finger LED is OFF");
    });

    server.on("/led/ring/on", HTTP_GET, [](AsyncWebServerRequest *request){
      digitalWrite(ringLedPin, HIGH);
      request->send(200, "text/plain", "Ring finger LED is ON");
    });

    server.on("/led/ring/off", HTTP_GET, [](AsyncWebServerRequest *request){
      digitalWrite(ringLedPin, LOW);
      request->send(200, "text/plain", "Ring finger LED is OFF");
    });

    server.on("/led/pinky/on", HTTP_GET, [](AsyncWebServerRequest *request){
      digitalWrite(pinkyLedPin, HIGH);
      request->send(200, "text/plain", "Pinky finger LED is ON");
    });

    server.on("/led/pinky/off", HTTP_GET, [](AsyncWebServerRequest *request){
      digitalWrite(pinkyLedPin, LOW);
      request->send(200, "text/plain", "Pinky finger LED is OFF");
    });

    server.begin();
    Serial.println("Server started");
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}

void loop() {
  // Additional code can be added here if needed, but typically not necessary for basic HTTP server operations.
}
