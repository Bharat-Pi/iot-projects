/*************************************************************************
   PROJECT NAME: Bharat Pi Enable_OTA
   AUTHOR: Bharat Pi
   CREATED DATE: 15/03/2024
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   VERSION: 0.1.0

   DESCRIPTION: Enable_OTA using Bharat Pi Board.

   REVISION HISTORY TABLE:
   ------------------------------------------
   Date      | Firmware Version | Comments
   ------------------------------------------
   15/03/2024 -    0.1.0       -    Initial release of sample script.

 *************************************************************************/


#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "smile123";
const char* password = "123456789";

void setup() 
  {
    Serial.begin(115200);
    Serial.println("Booting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
      while (WiFi.waitForConnectResult() != WL_CONNECTED) 
        {
          Serial.println("Connection Failed! Rebooting...");
          delay(5000);
          ESP.restart();
         }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    //ArduinoOTA.setHostname("OTAESP32");
    //ArduinoOTA.setPassword("pass");
    //ArduinoOTA.begin();
       
    ArduinoOTA
      .onStart([]() {
        Serial.println("Started updating");
      })
      .onEnd([]() {
        Serial.println("\nEnd");
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", progress / (total / 100));
       
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
      });
   
  }

void loop() 
 {
   ArduinoOTA.handle();
 }
