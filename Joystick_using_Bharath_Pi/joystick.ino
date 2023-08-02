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