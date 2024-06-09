/*************************************************************************
   PROJECT NAME: Bharat Pi Blink_LED_Test
   AUTHOR: Bharat Pi
   CREATED DATE: 06/01/2024
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   VERSION: 0.1.1

   DESCRIPTION: Turns an LED on for one second, then off for one second, repeatedly.

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

#define LED_BUILTIN 2
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}
