/*************************************************************************
   PROJECT NAME: Bharat Pi AT_Debug_4G
   AUTHOR: Bharat Pi
   CREATED DATE: 22/01/2024
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   VERSION: 0.1.1

   DESCRIPTION:  AT Debug example, This script tries to auto-detect the baud rate
   and allows direct AT commands access.

   REVISION HISTORY TABLE:
   ------------------------------------------
   Date      | Firmware Version | Comments
   ------------------------------------------
   22/01/2024 -    0.1.0       -    Initial release of sample script.
                                    (Used Arduino IDE version - 2.3.2,
                                    Used board manager - esp32 by Espressif version - 2.0.16)
                                    
   o6/06/2024 -    0.1.1       -    Added the Updated version of board manager and lib 
                                    (esp32 by Espressif version - 3.0.0)

 *************************************************************************/


 #define TINY_GSM_MODEM_A7

// Set serial for debug console (to the Serial Monitor, speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on BharatPi 4G, Mega, Leonardo, Micro
#ifndef __AVR_ATmega328P__
#define SerialAT Serial2


#else
// To use Software Serial on Uno, Nano
//  #include <SoftwareSerial.h>
//  SoftwareSerial SerialAT(16, 17);  // RX, TX
#endif

#define TINY_GSM_DEBUG SerialMon
//Refer for the TinyGsmClient:  https://tiny.cc/tinygsm-readme
#include <TinyGsmClient.h>
#define POWER 32
#define NRESET 33
// Module baud rate
uint32_t rate = 0; // Set to 0 for Auto-Detect

void setup() {
  // Set console baud rate
   pinMode(2, OUTPUT);
  //Send low pulse to power on the module
  pinMode(POWER,OUTPUT);
  pinMode(NRESET,OUTPUT);
  digitalWrite(NRESET,HIGH);
  digitalWrite(2,LOW);
  delay(100);
  digitalWrite(POWER,LOW);
  delay(100);
  digitalWrite(POWER,HIGH);
  digitalWrite(2,HIGH);
  SerialMon.begin(115200);
  delay(6000);
}

void loop() {

  if (!rate) {
    rate = TinyGsmAutoBaud(SerialAT);
  }

  if (!rate) {
    SerialMon.println(F("***********************************************************"));
    SerialMon.println(F(" Module does not respond!"));
    SerialMon.println(F("   Check your Serial wiring"));
    SerialMon.println(F("   Check the module is correctly powered and turned on"));
    SerialMon.println(F("***********************************************************"));
    delay(30000L);
    return;
  }

  SerialAT.begin(rate);

  // Access AT commands from Serial Monitor
  SerialMon.println(F("***********************************************************"));
  SerialMon.println(F(" You can now send AT commands"));
  SerialMon.println(F(" Enter \"AT\" (without quotes), and you should see \"OK\""));
  SerialMon.println(F(" If it doesn't work, select \"Both NL & CR\" in Serial Monitor"));
  SerialMon.println(F("***********************************************************"));

  while(true) {
    if (SerialAT.available()) {
      SerialMon.write(SerialAT.read());
    }
    if (SerialMon.available()) {
      SerialAT.write(SerialMon.read());
    }
    delay(0);
  }
}
