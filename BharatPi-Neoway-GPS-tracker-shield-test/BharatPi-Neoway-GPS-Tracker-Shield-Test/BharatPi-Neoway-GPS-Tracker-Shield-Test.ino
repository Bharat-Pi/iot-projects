/*************************************************************************
   PROJECT NAME: Bharat Pi Neoway-GPS-Tracker-Shield-Test
   AUTHOR: Bharat Pi
   CREATED DATE: 06/01/2024
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   VERSION: 0.1.1

   DESCRIPTION: Neoway-GPS-Tracker-Shield-Test

   REVISION HISTORY TABLE:
   ------------------------------------------
   Date      | Firmware Version | Comments
   ------------------------------------------
   06/01/2024 -    0.1.0       -    Initial release of sample script.
                                    (Used Arduino IDE version - 2.3.2,
                                    Used board manager - esp32 by Espressif version - 2.0.16)
                                    
   o6/06/2024 -    0.1.1       -    Added the Updated version of board manager and lib 
                                    (esp32 by Espressif version - 3.0.0)
 *************************************************************************/

#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>


// The TinyGPSPlus object


TinyGPSPlus gps;

// The serial connection to the GPS module
SoftwareSerial ss(32, 33);

void setup() {
  Serial.begin(9600);
  ss.begin(9600);
}

void loop() {
  while (ss.available() > 0) {
    // get the byte data from the GPS
//        byte gpsData = ss.read();
//        Serial.write(gpsData);
    if (gps.encode(ss.read()))
      displayInfo();
    if (millis() > 5000 && gps.charsProcessed() < 10)
    {
      Serial.println(F("No GPS detected: check wiring."));
      while (true);
    }
  }
}

void displayInfo()
{
  Serial.println(F("Location: "));
  if (gps.location.isValid()) {
    Serial.print("Lat: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print("Lng: ");
    Serial.print(gps.location.lng(), 6);
    Serial.println();
  }
  else
  {
    Serial.println(F("INVALID"));
  }
}
