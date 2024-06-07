/*************************************************************************
   PROJECT NAME: Bharat Pi big_sound sensor
   AUTHOR: Bharat Pi
   CREATED DATE: 02/08/2023
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   VERSION: 0.1.0

   DESCRIPTION: Bharat Pi big_sound sensor sample code

   REVISION HISTORY TABLE:
   ------------------------------------------
   Date      | Firmware Version | Comments
   ------------------------------------------
   02/08/2023 -    0.1.0       -    Initial release of sample script.
                                    (Used Arduino IDE version - 2.3.2,
                                    Used board manager - esp32 by Espressif version - 2.0.16)
                                    
   o6/06/2024 -    0.1.1       -    Added the Updated version of board manager and lib 
                                    (esp32 by Espressif version - 3.0.0)

 *************************************************************************/


int ledPin=23;
int sensorPin=32;
int val =0;

void setup(){
  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin, INPUT);
  Serial.begin (115200);
}
  
void loop (){
  val =digitalRead(sensorPin);
  Serial.println (val);
  // when the sensor detects a signal above the threshold value, LED flashes
  if (val==HIGH) {
    digitalWrite(ledPin, HIGH);
  }
  else {
    digitalWrite(ledPin, LOW);
  }
}
