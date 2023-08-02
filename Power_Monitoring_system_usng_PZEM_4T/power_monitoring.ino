#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Preferences.h>
#include <PZEM004Tv30.h>

// LCD settings
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// PZEM settings
PZEM004Tv30 pzem(Serial2, 16, 17);  // RX, TX

// Preferences settings
Preferences preferences;
unsigned long lastUpdateTime = 0;
float totalPowerConsumed = 0.0;
float totalCurrentConsumed = 0.0;
float currentConsumedToday = 0.0;
float powerConsumedToday = 0.0;

void setup() {
  // Initialize the LCD
  Serial.begin(115200);
  Serial.begin(115200);
  lcd.begin();
  lcd.setCursor(0, 0);
  Serial.println("power monitoring");
  delay(500);
  lcd.print("Power Monitoring");
  lcd.setCursor(0, 1);
  lcd.print("by refillbot.com");
  Serial.println("by refillbot.com");
  delay(500);

  delay(2000);
  lcd.clear();
  lcd.begin();
  lcd.backlight();

  preferences.begin("power_prefs", false);

  totalPowerConsumed = preferences.getFloat("totalPowerConsumed", 0.0);
  totalCurrentConsumed = preferences.getFloat("totalCurrentConsumed", 0.0);

  // Print initial values on LCD
  lcd.setCursor(0, 0);
  lcd.print("Power: -- W");
  lcd.setCursor(0, 1);
  lcd.print("Current: -- A");
  delay(2000);
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= 1000) {
    lastUpdateTime = currentTime;

    // Read the voltage, current, power, and frequency from the PZEM
    float voltage = pzem.voltage();
    float current = pzem.current();
    float power = pzem.power();
    float frequency = pzem.frequency();

    // Calculate energy consumed in 1 second 
    float energyConsumed = power / 3600.0;

    // Calculate energy consumed in one day (24 hours)
    float energyConsumedOneDay = energyConsumed * 24.0;

    // Calculate power consumed in one day
    powerConsumedToday += energyConsumed;

    // Calculate current consumed in one day
    currentConsumedToday += current;

    // Update total cumulative values
    totalPowerConsumed += energyConsumed;
    totalCurrentConsumed += current;

    // Save the cumulative values to Preferences library
    preferences.putFloat("totalPowerConsumed", totalPowerConsumed);
    preferences.putFloat("totalCurrentConsumed", totalCurrentConsumed);

    lcd.setCursor(0, 0);
    lcd.print("Power: ");
    lcd.print(power);
    lcd.print(" W");

    lcd.setCursor(0, 1);
    lcd.print("Current: ");
    lcd.print(current);
    lcd.print(" A");

    Serial.print("Power: ");
    Serial.print(power);
    Serial.println(" W");

    Serial.print("Current: ");
    Serial.print(current);
    Serial.println(" A");

    delay(4000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Frequency: ");
    lcd.print(frequency);
    lcd.print(" Hz");

    lcd.setCursor(0, 1);
    lcd.print("Energy: ");
    lcd.print(energyConsumed, 3);
    lcd.print(" Wh");
    delay(4000);

    Serial.print("Frequency: ");
    Serial.print(frequency);
    Serial.println(" Hz");

    Serial.print("Energy: ");
    Serial.print(energyConsumed, 3);
    Serial.println(" Wh");

    delay(4000);

    lcd.begin();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Power C: ");
    lcd.print(powerConsumedToday, 3);
    lcd.print(" Wh");

    lcd.setCursor(0, 1);
    lcd.print("Voltage C: ");
    lcd.print(totalPowerConsumed, 3);
    lcd.print(" Wh");
    delay(4000);

    Serial.print("Power Consumption Today: ");
    Serial.print(powerConsumedToday, 3);
    Serial.println(" Wh");

    Serial.print("Voltage Consumption Total: ");
    Serial.print(totalPowerConsumed, 3);
    Serial.println(" Wh");

    delay(1000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Current_c: ");
    lcd.print(currentConsumedToday, 3);
    lcd.print(" A");

    lcd.setCursor(0, 1);
    lcd.print("Current_T: ");
    lcd.print(totalCurrentConsumed, 3);
    lcd.print(" A");
    delay(4000);

    Serial.print("Curren _C _T: ");
    Serial.print(currentConsumedToday, 3);
    Serial.println(" A");

    Serial.print("Current Total: ");
    Serial.print(totalCurrentConsumed, 3);
    Serial.println(" A");

    delay(1000);
  }
}
