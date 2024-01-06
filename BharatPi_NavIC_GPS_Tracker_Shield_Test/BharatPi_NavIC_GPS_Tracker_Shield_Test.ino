#include <SoftwareSerial.h>
//#include <TinyGPSPlus.h>


// The TinyGPSPlus object


//TinyGPSPlus gps;

// The serial connection to the GPS module
SoftwareSerial ss(33, 32);

void setup() {
  Serial.begin(115200);
  ss.begin(115200);
}

void loop() {
  if (ss.available() > 0) {
    // get the byte data from the GPS
      //Serial.print("Serial Data: ");
       byte gpsData = ss.read();
       Serial.write(gpsData);
    // if (gps.encode(ss.read()))
    //   displayInfo();
    // if (millis() > 5000 && gps.charsProcessed() < 10)
    // {
    //   Serial.println(F("No GPS detected: check wiring."));
    //   while (true);
    // }
    
  } else {
    //Serial.println("No GPS data available yet");
    delay(500);
  }
}

// void displayInfo()
// {
//   Serial.println(F("Location: "));
//   if (gps.location.isValid()) {
//     Serial.print("Lat: ");
//     Serial.print(gps.location.lat(), 6);
//     Serial.print(F(","));
//     Serial.print("Lng: ");
//     Serial.print(gps.location.lng(), 6);
//     Serial.println();
//   }
//   else
//   {
//     Serial.println(F("INVALID"));
//   }
// }