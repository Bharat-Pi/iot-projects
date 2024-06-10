#define BLYNK_TEMPLATE_ID "TMPL3qtqAe6AN" // Add your actual Template ID here
#define BLYNK_TEMPLATE_NAME "led"
#define BLYNK_AUTH_TOKEN "ZGMM0nHLso1Xwq8FMrbuo35wE59z0CFB"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "smile123";
char pass[] = "123456789";

int LED = 2; // Define LED as an Integer and use pin 2 on the ESP32

void setup() {
  // Debug console
  Serial.begin(115200);
  pinMode(LED, OUTPUT); // Set the LED (pin 2) as an output
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();
}

// This function will be called every time button Widget
// in Blynk app writes values to the Virtual Pin V3
BLYNK_WRITE(V3) {
  int pinValue = param.asInt(); // Assigning incoming value from pin V3 to a variable
  if (pinValue == 1) {
    digitalWrite(LED, HIGH); // Turn LED on
  } else {
    digitalWrite(LED, LOW); // Turn LED off
  }
}
