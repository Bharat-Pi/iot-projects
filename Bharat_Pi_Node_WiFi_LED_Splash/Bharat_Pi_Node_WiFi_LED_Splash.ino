/*************************************************************************
   PROJECT NAME: Bharat Pi Node_WiFi_LED_Splash
   AUTHOR: Bharat Pi
   CREATED DATE: 26/03/2024
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   VERSION: 0.1.0

   DESCRIPTION: Bharat Pi Node_WiFi_LED_Splash.
   
   REVISION HISTORY TABLE:
   ------------------------------------------
   Date      | Firmware Version | Comments
   ------------------------------------------
   26/03/2024 -    0.1.0       -    Initial release of sample script.
   
 *************************************************************************/

void ledBlink(int time){
  digitalWrite(2, HIGH);
  delay(time);
  digitalWrite(2, LOW);
  delay(time);
  digitalWrite(12, HIGH);
  delay(time);
  digitalWrite(12, LOW);
  delay(time);
  digitalWrite(13, HIGH);
  delay(time);
  digitalWrite(13, LOW);  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
  ledBlink(25);
  ledBlink(25);
  ledBlink(25);
  ledBlink(25);
  ledBlink(25);
  ledBlink(25);  
  ledBlink(50);
  ledBlink(50);
  ledBlink(50);
  ledBlink(50);
  ledBlink(50);
  ledBlink(50);
  ledBlink(50);
  ledBlink(50);  
  ledBlink(100);
  ledBlink(100);
  ledBlink(100);
  ledBlink(100);
  ledBlink(100);
  ledBlink(100);
  ledBlink(100);
  ledBlink(100);  
  ledBlink(250);
  ledBlink(250);
  ledBlink(250);
  ledBlink(250); 
  ledBlink(500); 
  ledBlink(1000);  

  digitalWrite(2, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:  
  digitalWrite(2, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
}
