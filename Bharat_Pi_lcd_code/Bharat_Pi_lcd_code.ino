#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2-line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Initialize the LCD with the number of columns and rows
  lcd.init();
  // Turn on the backlight
  lcd.backlight();
  // Print a message to the LCD
  lcd.setCursor(0, 0); // Set cursor to column 0, line 0
  lcd.print("Hello, welcome");
  lcd.setCursor(0, 1); // Set cursor to column 0, line 1
  lcd.print("to Bharat pi");
}

void loop() {
  // No need to repeat anything in loop for this example
}
