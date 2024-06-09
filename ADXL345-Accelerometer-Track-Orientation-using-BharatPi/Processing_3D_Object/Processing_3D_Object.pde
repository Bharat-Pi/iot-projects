/*************************************************************************
   PROJECT NAME: Bharat Pi Processing_3D_Object
   AUTHOR: Bharat Pi
   CREATED DATE: 06/01/2024
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   VERSION: 0.1.1

   DESCRIPTION:  Arduino and ADXL345 Accelerometer - 3D Visualization Example 
   by Dejan, https://howtomechatronics.com

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

import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

Serial myPort;

String data="";
float roll, pitch;

void setup() {
  size (1200, 900, P3D);
  String portName = Serial.list()[1];
  myPort = new Serial(this, portName, 9600); // starts the serial communication
  myPort.bufferUntil('\n');
}

void draw() {
  translate(width/2, height/2, 0);
  background(33);
  textSize(22);
  text("Roll: " + int(roll) + "     Pitch: " + int(pitch), -100, 265);

  // Rotate the object
  rotateX(radians(roll));
  rotateZ(radians(-pitch));
  
  // 3D 0bject
  textSize(30);  
  fill(0, 76, 153);
  box (386, 40, 200); // Draw box
  textSize(25);
  fill(255, 255, 255);
  text("Bharat Pi Accelerometer Demo", -183, 10, 101);

  //delay(10);
  //println("ypr:\t" + angleX + "\t" + angleY); // Print the values to check whether we are getting proper values
}

// Read data from the Serial Port
void serialEvent (Serial myPort) { 
  // reads the data from the Serial Port up to the character '.' and puts it into the String variable "data".
  data = myPort.readStringUntil('\n');

  // if you got any bytes other than the linefeed:
  if (data != null) {
    data = trim(data);
    // split the string at "/"
    String items[] = split(data, '/');
    if (items.length > 1) {

      //--- Roll,Pitch in degrees
      roll = float(items[0]);
      pitch = float(items[1]);
    }
  }
}
