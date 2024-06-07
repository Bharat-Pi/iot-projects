/*************************************************************************
   PROJECT NAME:  Bharat Pi NavIC Shield test firmware 
   AUTHOR: Bharat Pi
   CREATED DATE: 25/03/2024
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   VERSION: 0.1.1

   DESCRIPTION: NavIC get latlong and other navigation parameters using Bharat Pi NavIC tracker shield.

   REVISION HISTORY TABLE:
   ------------------------------------------
   Date      | Firmware Version | Comments
   ------------------------------------------
   25/03/2024 -    0.1.0       -    Initial release with Lat long and other nav parameters. Prints to serial monitor.
                                    (Used Arduino IDE version - 2.3.2,
                                    Used board manager - esp32 by Espressif version - 2.0.16)
                                    
   o6/06/2024 -    0.1.1       -    Added the Updated version of board manager and lib 
                                    (esp32 by Espressif version - 3.0.0)

 *************************************************************************/

#include <TinyGPS++.h>
#include <SoftwareSerial.h>


#define GPS_RX_PIN 33
#define GPS_TX_PIN 32 


TinyGPSPlus gps;
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);


void setup() {
    Serial.begin(115200);  
    gpsSerial.begin(115200); 


    Serial.println("Getting GPS data...");
}


void loop() {
    // Read data from GPS module
    while (gpsSerial.available() > 0) {
        if (gps.encode(gpsSerial.read())) {
            if (gps.location.isValid()) {
                // Get latitude and longitude
                float latitude = gps.location.lat();
                float longitude = gps.location.lng();
                String  date = gps.date.day() + "/" + gps.date.month();
                String time = gps.time.hour() + ":" + gps.time.minute();
                double speedKnots = gps.speed.knots();
                float  courseDeg = gps.course.deg();
                double altitudeMts = gps.altitude.meters();
                uint32_t satellites = gps.satellites.value();
                double hdop = gps.hdop.hdop();


                // Print latitude and longitude
                Serial.print("");
                Serial.print("");
                Serial.print("Lat: ");
                Serial.println(latitude, 7);
                Serial.print("Long: ");
                Serial.println(longitude, 7);
                // Serial.print("Date: ");
                // Serial.println(date);
                // Serial.print("Time: ");
                // Serial.println(time);
                Serial.print("Speed (knots): ");
                Serial.println(speedKnots, 7);                                                
                Serial.print("Course (degree): ");
                Serial.println(courseDeg, 7);
                Serial.print("Altitude (Meters): ");
                Serial.println(altitudeMts, 7);    
                Serial.print("Satellites:");
                Serial.println(satellites, 7);  
                Serial.print("HDOP:");
                Serial.println(hdop, 7);    
                Serial.print("");
                Serial.print("");                                                            
            }
        }
    }
}
