/*************************************************************************

   PROJECT NAME: Bharat Pi big_sound sensor
   AUTHOR: Bharat Pi
   CREATED DATE: 22/01/2024
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   VERSION: 0.1.0

   DESCRIPTION: Bharat Pi big_sound sensor sample code

   REVISION HISTORY TABLE:
   ------------------------------------------
   Date      | Firmware Version | Comments
   ------------------------------------------
   22/01/2024 -    0.1.0       -    Initial release of sample script.

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
