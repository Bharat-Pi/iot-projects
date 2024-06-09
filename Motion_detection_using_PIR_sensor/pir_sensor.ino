/*************************************************************************

   PROJECT NAME: Bharat Pi pir_sensor sample code
   AUTHOR: Bharat Pi
   CREATED DATE: 02/08/2023
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   VERSION: 0.1.0

   DESCRIPTION: Bharat Pi pir_sensor sample code

   REVISION HISTORY TABLE:
   ------------------------------------------
   Date      | Firmware Version | Comments
   ------------------------------------------
   02/08/2023 -    0.1.0       -    Initial release of sample script.

 *************************************************************************/


int led = 32;                // the pin that the LED is attached to bharat pi boards 
int sensor = 23;             // the pin that the sensor is attached to bharat pi boards
int state = HIGH;            // by default, no motion detected
int val = 0;                 // variable to store the sensor status (value)
int motionCounter = 0;       // variable to count the number of motions detected

void setup() {
  pinMode(led, OUTPUT);      // initialize LED as an output
  pinMode(sensor, INPUT);    // initialize sensor as an input
  Serial.begin(115200);      // initialize serial
}

void loop() {
  val = digitalRead(sensor);   // read sensor value
  Serial.print("Sensor value: ");
  Serial.println(val);         // print sensor value for debugging
  
  if (val == HIGH) {           // check if the sensor is HIGH
    digitalWrite(led, HIGH);   // turn LED ON
    delay(100);                // delay 100 milliseconds 

    if (state == LOW) {
      motionCounter++;         // increment motion counter
      Serial.print("Motion detected! Count: ");
      Serial.println(motionCounter); 
      state = HIGH;            // update variable state to HIGH
    }
  } 
  else {
    digitalWrite(led, LOW);    // turn LED OFF
    delay(200);                // delay 200 milliseconds 

    if (state == HIGH) {
      Serial.println("Motion stopped!");
      state = LOW;             // update variable state to LOW
    }
  }
}



