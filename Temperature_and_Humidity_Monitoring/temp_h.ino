/*************************************************************************

   PROJECT NAME: Bharat Pi Tempereture and Humidity sensor Sample Code
   AUTHOR: Bharat Pi
   CREATED DATE: 22/01/2024
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   VERSION: 0.1.0

   DESCRIPTION:  This script will give you the sample testing of tempereture and humidity sensor,
   which will measures the tempereture and humidty
   and gives the respective values as output when you run the code.

   REVISION HISTORY TABLE:
   ------------------------------------------
   Date      | Firmware Version | Comments
   ------------------------------------------
   22/01/2024 -    0.1.0       -    Initial release of sample script.

 *************************************************************************/


#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#define DHTPIN 33
#define DHTTYPE DHT11

LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x3F, 16 column and 2 rows
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  dht.begin();     
  lcd.begin();      
  lcd.backlight();
  lcd.print("Refillbot");
  lcd.print("Bharth Pi") ;
  delay(1000);

void loop()
{
  delay(2000); 

  float humi  = dht.readHumidity();    // read humidity
  float tempC = dht.readTemperature(); // read temperature

  lcd.clear();
  if (isnan(humi) || isnan(tempC)) {
    lcd.setCursor(0, 0);
    lcd.print("Failed");
  } else {
    lcd.setCursor(0, 0);  
    lcd.print("Temp: ");
    lcd.print(tempC);    
    lcd.print((char)223); 
    lcd.print("C");

    lcd.setCursor(0, 1); 
    lcd.print("Humi: ");
    lcd.print(humi);     
    lcd.print("%");
  }
}
