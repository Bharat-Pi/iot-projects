/*************************************************************************
   PROJECT NAME: Bharat Pi joystick sample
   AUTHOR: Bharat Pi
   CREATED DATE: 02/08/2023
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   VERSION: 0.1.0

   DESCRIPTION: joystick sample using Bharat Pi Board.

   REVISION HISTORY TABLE:
   ------------------------------------------
   Date      | Firmware Version | Comments
   ------------------------------------------
   02/08/2023 -    0.1.0       -    Initial release of sample script.
   
 *************************************************************************/


int joyX= 2;
int joyY= 4;
int xvalue;
int yvalue;
 
void setup() {
  Serial.begin(115200);
}
 
void loop() {
  // put your main code here, to run repeatedly:
  xvalue = analogRead (joyX);
  yvalue = analogRead (joyY);
 
  //print the values with to plot or view
  Serial.println(xvalue);
  delay(2500);
  Serial.println(yvalue);
  delay(3000);
}
