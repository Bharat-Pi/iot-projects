#include <NewPing.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


#define TRIG_PIN 33
#define ECHO_PIN 32


#define MAX_DISTANCE 200 // Maximum distance to measure (in centimeters)


NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); // Create a NewPing object


LiquidCrystal_I2C lcd(0x27, 16, 2); // Change 0x27 to your LCD I2C address if different


void setup() {
  lcd.begin();
  lcd.backlight();
  lcd.print("Ultrasonic Sensor");
  lcd.print("By Bharath Pi");
  }