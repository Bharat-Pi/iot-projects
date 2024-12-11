/*************************************************************************
   PROJECT: Bharat Pi 4G BModule AT Command Test Program
   AUTHOR: Bharat Pi
   CREATED DATE: 10/07/2024
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   version: 1.0
 
   DESCRIPTION: AT command testing from serial monitor. 4G module boots up and awaits for AT commands. 
   Executes AT commands and response is printed on serial port.

   SIMCARD: 4G sim cards from Airtel/Vodaphone/Jio/BSNL can be used. 
   To test SMS sending, SMS receving, HTTP call request, please ensure you have data plan and sms pack. 
   
   IMPORTANT: Configure the APN accordingly as per your Sim provider as mentioned below.
    //Airtel -> "airtelgprs.com" 
    //BSNL -> "bsnlnet" 
    //Voda -> portalnmms
    //jio -> jionet

   TODO: Before you upload the code to Bharat Pi board
   Use a DC power adapter with rating 9V-2A as the 4G module requires power while it operates. 
   USB alone will not be able to power up the 4G module.

   REVISION:
   ------------------------------------------
   DATE:      | FIRMWAREVERSION: | COMMENTS:
   ------------------------------------------
   10/07/2024 -    1.0       -    Initial release of sample script.

*************************************************************************/

#define TINY_GSM_MODEM_SIM7600 //TINY_GSM_MODEM compatible for A7672S as well
#define SerialAT Serial1
#define SerialMon Serial

#include <TinyGsmClient.h> //Library version compatible 0.12.0
#include <Ticker.h> //Lib version - 4.4.0

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

#define UART_BAUD   115200
#define PIN_DTR     25 //4G module rest, refer datasheet
#define PIN_TX      17 //4G module tx, refer datasheet
#define PIN_RX      16 //4G module rx, refer datasheet
#define PWR_PIN     32 //4G module power pin, refer datasheet

#define LED_PIN 2

//Modem power ON function
void modemPowerOn(){
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(1000);
  digitalWrite(PWR_PIN, HIGH);
}

//Modem power OFF
void modemPowerOff(){
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(1500);
  digitalWrite(PWR_PIN, HIGH);
}

//Modem Restart function
void modemRestart(){
  modemPowerOff();
  delay(1000);
  modemPowerOn();
}

void setup(){
  // Set serial baud rate
  SerialMon.begin(115200);
  pinMode(LED_PIN, OUTPUT); //Enable LED indicator that the 4G module is ready for AT commands
  digitalWrite(LED_PIN, LOW);

  delay(500);
  Serial.flush();

  modemPowerOn(); //Power ON modem
  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
  Serial.clearWriteError();

  Serial.println();
  Serial.println();
  Serial.println("/**********************************************************/");
  Serial.println("  Bharat Pi 4G/LTE Board AT Command Test Program");
  Serial.println("  To power ON modem please ensure 9V-2A DC adapter is connected.");
  Serial.println("/**********************************************************/\n\n");

  delay(10000); //deliberate delay so that user reads the message and if he missed to connect power adater then he can connect.

  String res;
  Serial.println("Initializing Modem...");

  if (!modem.init()) {
    modemRestart();
    delay(2000);
    Serial.println("");
    Serial.println("Failed to initialize modem as there may not be a Simcard inserted.");
    Serial.println("Do not worry, to test AT commands simcard is not mandatory. :-)");
    Serial.println("");
  }

  delay(2000);
  digitalWrite(LED_PIN, HIGH);
  Serial.println("Running SIMCOMATI command...");
  modem.sendAT("+SIMCOMATI"); //Get the module information
  modem.waitResponse(1000L, res);
  res.replace(AT_NL "OK" AT_NL, "");
  Serial.println(res);
  res = "";
  Serial.println();

  delay(2000);
  Serial.println("Fetching some module information...");
  Serial.println("");
  //Get module manufacturer details
  String modemName = modem.getModemModel();
  Serial.println("Modem Name : " + modemName);
  delay(1000);

  String modemInfo = modem.getModemInfo();
  Serial.println("Modem Info : " + modemInfo);
  delay(1000);

  delay(1000);
  Serial.println("Checking UE (User Equipment) module information...");
  Serial.println();
  modem.sendAT("+CPSI?");
  if (modem.waitResponse(1000L, res) == 1) {
    res.replace(AT_NL "OK" AT_NL, "");
    Serial.println(res);
  }

  delay(1000);  
  Serial.println("");
  Serial.println("");    

  Serial.println();
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  Serial.println();
  Serial.println("  4G module is now ready to test AT commands");
  Serial.println();
  Serial.println("  Type a simple AT command  \"AT\" in Arduino IDE serial monitor"); 
  Serial.println("  and you should see a \"OK\" response on serial monitor.");
  Serial.println();
  Serial.println("  Download latest AT command manual for SIMCOM A7672S from below link:");
  Serial.println("  AT Manual: https://bharatpi.net/wp-content/uploads/2023/12/A7672S_Series_AT_Command_Manual_V1.09.pdf");
  Serial.println();
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  Serial.println();
  Serial.println();
  Serial.println();
}


void loop(){
  while (1) {
    while (SerialAT.available()) {
      SerialMon.write(SerialAT.read());
    }
    while (SerialMon.available()) {
      SerialAT.write(SerialMon.read());
    }
  }    
}
