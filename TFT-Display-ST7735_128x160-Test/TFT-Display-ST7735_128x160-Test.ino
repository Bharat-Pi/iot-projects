/*************************************************************************
   PROJECT: Bharat Pi - TFT ST7735 Display test sample code
   AUTHOR: Bharat Pi
   FUNCTION: This is a test of the rotation capabilities of the TFT library
   IMPORTANT: Please ensure the pic configurations below are matching with the display you have. 
   There are multiple versions of ST7735 TFT diplays

   TODO: (Before you upload the code to Bharat Pi board) 
   1) Pin connections as defined in below code
   2) Ensure the following pins from the TFT are connected to respective Bharat Pi Pins: 
   ----------------------------
   TFT Pins     | Bharat Pi Pins
   ----------------------------
      LED       | 3.3v, 
      SCK       | SCL 
      SDA       | SDA
      A0/DC     | 13
      RESET/RST | 14
      CS        | 12
      GND       | GND
      VCC       | 5v
   -----------------------------
   DESC: This script will connect using 4G sim and send a sms and make a http call 
   to a test url setup on Pipe Dream. You need to set your own Pipe dream url 
   and configure the same in this code to be able to receive a call.
 
   COPYRIGHT: 
    This is a library for the Adafruit 1.8" SPI display.
    Written by Limor Fried/Ladyada for Adafruit Industries.
    MIT license, all text above must be included in any redistribution   
    Code ported to Bharat Pi boards
 *************************************************************************/

//**********************************************************************************
//include library code
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
//***********************************************************************************
//define pins of TFT screen -> Bharat Pi
#define TFT_CS     12
#define TFT_RST    14 
#define TFT_DC     13
#define TFT_SCLK   22
#define TFT_MOSI   21
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
//**********************************************************************************
void setup(void) {
  Serial.begin(115200);//initialise serial communication at 115200 bps
  Serial.print("This is the rotation test");
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  Serial.println("Initializing...");
  tft.setTextWrap(false); // Allow text to run off right edge
  tft.fillScreen(ST7735_BLACK);
  Serial.println("This is a test of the rotation capabilities of the TFT library!");
  Serial.println("Press <SEND> (or type a character) to advance");}
void loop(void) {
  rotateLine();
  rotateText();
  rotatePixel();
  rotateFastline();
  rotateDrawrect();
  rotateFillrect();
  rotateDrawcircle();
  rotateFillcircle();
  rotateTriangle();
  rotateFillTriangle();
  rotateRoundRect();
  rotateFillRoundRect();
  rotateChar();
  rotateString();
}
void rotateText() {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST7735_BLACK);
    Serial.println(tft.getRotation(), DEC);
    tft.setCursor(0, 30);
    tft.setTextColor(ST7735_RED);
    tft.setTextSize(1);
    tft.println("Hello World!");
    tft.setTextColor(ST7735_YELLOW);
    tft.setTextSize(2);
    tft.println("Hello World!");
    tft.setTextColor(ST7735_GREEN);
    tft.setTextSize(3);
    tft.println("Hello World!");
    tft.setTextColor(ST7735_BLUE);
    tft.setTextSize(4);
    tft.print(1234.567);
    while (!Serial.available());
    Serial.read();  Serial.read();  Serial.read();  
    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateFillcircle(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST7735_BLACK);
    Serial.println(tft.getRotation(), DEC);
    tft.fillCircle(10, 30, 10, ST7735_YELLOW);
    while (!Serial.available());
    Serial.read();  Serial.read();  Serial.read();
    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateDrawcircle(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST7735_BLACK);
    Serial.println(tft.getRotation(), DEC);
    tft.drawCircle(10, 30, 10, ST7735_YELLOW); 
    while (!Serial.available());
    Serial.read();  Serial.read();  Serial.read();  
    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateFillrect(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST7735_BLACK);
    Serial.println(tft.getRotation(), DEC);
    tft.fillRect(10, 20, 10, 20, ST7735_GREEN); 
    while (!Serial.available());
    Serial.read();  Serial.read();  Serial.read();
    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateDrawrect(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST7735_BLACK);
    Serial.println(tft.getRotation(), DEC);
    tft.drawRect(10, 20, 10, 20, ST7735_GREEN); 
    while (!Serial.available());
    Serial.read();  Serial.read();  Serial.read();
    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateFastline(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST7735_BLACK);
    Serial.println(tft.getRotation(), DEC);
    tft.drawFastHLine(0, 20, tft.width(), ST7735_RED);
    tft.drawFastVLine(20, 0, tft.height(), ST7735_BLUE);
    while (!Serial.available());
    Serial.read();  Serial.read();  Serial.read();
    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateLine(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST7735_BLACK);
    Serial.println(tft.getRotation(), DEC);
    tft.drawLine(tft.width()/2, tft.height()/2, 0, 0, ST7735_RED);
    while (!Serial.available());
    Serial.read();  Serial.read();  Serial.read();
    tft.setRotation(tft.getRotation()+1);
  }
}

void rotatePixel(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST7735_BLACK);
    Serial.println(tft.getRotation(), DEC);
    tft.drawPixel(10,20, ST7735_WHITE);
    while (!Serial.available());
    Serial.read();  Serial.read();  Serial.read();
    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateTriangle(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST7735_BLACK);
    Serial.println(tft.getRotation(), DEC);
    tft.drawTriangle(20, 10, 10, 30, 30, 30, ST7735_GREEN);
    while (!Serial.available());
    Serial.read();  Serial.read();  Serial.read();
    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateFillTriangle(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST7735_BLACK);
    Serial.println(tft.getRotation(), DEC);
    tft.fillTriangle(20, 10, 10, 30, 30, 30, ST7735_RED);
    while (!Serial.available());
    Serial.read();  Serial.read();  Serial.read();
    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateRoundRect(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST7735_BLACK);
    Serial.println(tft.getRotation(), DEC);
    tft.drawRoundRect(20, 10, 25, 15, 5, ST7735_BLUE);
    while (!Serial.available());
    Serial.read();  Serial.read();  Serial.read();
    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateFillRoundRect(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST7735_BLACK);
    Serial.println(tft.getRotation(), DEC);
    tft.fillRoundRect(20, 10, 25, 15, 5, ST7735_CYAN);
    while (!Serial.available());
    Serial.read();  Serial.read();  Serial.read();
    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateChar(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST7735_BLACK);
    Serial.println(tft.getRotation(), DEC);
    tft.drawChar(25, 15, 'A', ST7735_WHITE, ST7735_WHITE, 1);
    while (!Serial.available());
    Serial.read();  Serial.read();  Serial.read();
    tft.setRotation(tft.getRotation()+1);
  }
}

void rotateString(void) {
  for (uint8_t i=0; i<4; i++) {
    tft.fillScreen(ST7735_BLACK);
    Serial.println(tft.getRotation(), DEC);
    tft.setCursor(8, 25);
    tft.setTextSize(1);
    tft.setTextColor(ST7735_WHITE);
    tft.print("Adafruit Industries");
    while (!Serial.available());
    Serial.read();  Serial.read();  Serial.read();
    tft.setRotation(tft.getRotation()+1);
  }
}
