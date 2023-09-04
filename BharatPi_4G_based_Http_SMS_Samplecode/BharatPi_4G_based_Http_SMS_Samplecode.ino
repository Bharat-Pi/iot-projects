/*************************************************************************
   PROJECT: Bharat Pi 4G Board Sample Code
   AUTHOR: Refillbot
 
   FUNC: 4G testing with HTTP call to a url and send SMS.
   SIMCARD: 4G sim cards from Airtel/Vodaphone/Jio/BSNL. 
   
   IMPORTANT: Configure the APN accordingly as per your Sim provider
   TODO: (Before you upload the code to Bharat Pi board) 
   1) Add phone number(s) 
   2) Change APN as per your Sim provider
   3) Setup a test http URL in pipedream.com
   
   DESC: This script will connect using 4G sim and send a sms and make a http call 
   to a test url setup on Pipe Dream. You need to set your own Pipe dream url 
   and configure the same in this code to be able to receive a call.
 
   COPYRIGHT: Refillbot @MIT license for usage on Bharat Pi boards
 *************************************************************************/


#define TINY_GSM_MODEM_SIM7600 //TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024

#define TINY_GSM_TEST_SMS true
#define SMS_TARGET1  "<Your phone number 1>" //Enter you phone number to which you would like to recevied SMS
#define SMS_TARGET2  "<Your phone number 2>" //Enter you phone number to which you would like to recevied SMS
#define SMS_TARGET3  "<Your phone number 3>" //Enter you phone number to which you would like to recevied SMS

#define SerialAT Serial1
#define SerialMon Serial

#define GSM_PIN ""

const char apn[]  = "jionet";
const char gprsUser[] = "";
const char gprsPass[] = "";

#include <TinyGsmClient.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <Ticker.h>

StaticJsonDocument<200> payloadObj; 

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

#define UART_BAUD   115200
#define PIN_DTR     25
#define PIN_TX      17
#define PIN_RX      16
#define PWR_PIN     32

#define LED_PIN 2

void modemPowerOn(){
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(1000);
  digitalWrite(PWR_PIN, HIGH);
}

void modemPowerOff(){
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(1500);
  digitalWrite(PWR_PIN, HIGH);
}

void modemRestart(){
  modemPowerOff();
  delay(1000);
  modemPowerOn();
}

void setup(){
  // Set console baud rate
  SerialMon.begin(115200);

  delay(10);

  // Set LED OFF
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  modemPowerOn();
  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
  Serial.clearWriteError();
  //Serial.flush();
  Serial.println();
  Serial.println();
  Serial.println("/**********************************************************/");
  Serial.println("  Bharat Pi 4G/LTE Board Test Program");
  Serial.println("  To initialize the network test, please make sure the antenna has been");
  Serial.println("  connected and SIM card is inserted in the SIM slot (back side of the board).");
  Serial.println("/**********************************************************/\n\n");

  delay(10000);
}


void loop(){
  String res;
  Serial.println("Initializing Modem...");

  if (!modem.init()) {
    modemRestart();
    delay(2000);
    Serial.println("Failed to restart modem, attempting to continue without restarting");
    return;
  }

  Serial.println("SIMCOMATI command...");
  modem.sendAT("+SIMCOMATI");
  modem.waitResponse(1000L, res);
  res.replace(GSM_NL "OK" GSM_NL, "");
  Serial.println(res);
  res = "";
  Serial.println();

  Serial.println("Preferred mode selection (GSM/LTE)...");
  modem.sendAT("+CNMP?");
  if (modem.waitResponse(1000L, res) == 1) {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }
  res = "";
  Serial.println();


  Serial.println("Preferred selection between CAT-M and NB-IoT...");
  modem.sendAT("+CMNB?");
  if (modem.waitResponse(1000L, res) == 1) {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }
  res = "";
  Serial.println();


  String name = modem.getModemName();
  Serial.println("Modem Name : " + name);
  delay(1000);

  String modemInfo = modem.getModemInfo();
  Serial.println("Modem Info : " + modemInfo);
  delay(1000);

  String payload;
  payloadObj["testerInfo"] = "Tester : Sinech" + modemInfo;
  payloadObj["modemInfo"] = "Board Info: " + modemInfo;
  serializeJson(payloadObj, payload);

  // Unlock your SIM card with a PIN if needed
  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
    modem.simUnlock(GSM_PIN);
  }

  Serial.println("Network mode connectivity testing (GSM, LTE or GSM/LTE)...");

  for (int i = 0; i <= 4; i++) {
    uint8_t network[] = {
        2,  /*Automatic*/
        13, /*GSM only*/
        38, /*LTE only*/
        51  /*GSM and LTE only*/
    };
    Serial.printf("Try %d method\n", network[i]);
    modem.setNetworkMode(network[i]);
    delay(3000);
    bool isConnected = false;
    int tryCount = 60;
    while (tryCount--) {
      int16_t signal =  modem.getSignalQuality();
      Serial.print("Signal: ");
      Serial.print(signal);
      Serial.print(" ");
      Serial.print("isNetworkConnected: ");
      isConnected = modem.isNetworkConnected();
      Serial.println( isConnected ? "CONNECTED" : "NOT CONNECTED YET");
      if (isConnected) {
        break;
      }
      delay(1000);
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
    if (isConnected) {
        break;
    }
  }
  digitalWrite(LED_PIN, HIGH);

  Serial.println();
  Serial.println("Yehhh....Device is connected to Sim network.");
  Serial.println();

  delay(1000);
  Serial.println("Inquiring UE (User Equipment) system information...");
  Serial.println();
  modem.sendAT("+CPSI?");
  if (modem.waitResponse(1000L, res) == 1) {
    res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }

  delay(2000);
  Serial.println("Testing the HTTPS Call to pipe drive server...");
  Serial.println("NOTE: Please ensure to deploy an end point on Pipe Dream (pipedream.com) to test: https://eoh623b6wcgwbw8.m.pipedream.net");
  Serial.println();
  Serial.println();
  Serial.println("MODEM TESTING....");
  Serial.println();
  delay(5000);
  Serial.println("Initiating HTTP");
  modem.sendAT("+HTTPINIT"); //init HTTP
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: HTTP INIT FAILED!");
    DBG("+HTTPINIT");
  }
  delay(5000);
  Serial.println("Setting call back URL");
  modem.sendAT("+HTTPPARA=\"URL\",https://eoh623b6wcgwbw8.m.pipedream.net"); //set call back URL to test 
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: HTTP URL SETTING FAILED!");
    DBG("+HTTPPARA=\"URL\",https://eoh623b6wcgwbw8.m.pipedream.net");
  }
  modem.sendAT("+HTTPPARA=\"CONTENT\",\"application/json\"");
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: HTTP CONTENT TYPE SETTING FAILED");
    DBG("+HTTPPARA=\"URL\",https://eoh623b6wcgwbw8.m.pipedream.net");
  }  
  modem.sendAT("+HTTPDATA=" + String(payload.length()) + ",100000");
  modem.sendAT(payload);
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: HTTP PAYLOAD SETTING FAILED");
    DBG("+HTTPPARA=\"URL\",https://eoh623b6wcgwbw8.m.pipedream.net");
  }   
  delay(5000);
  Serial.println("Executing POST call");
  modem.sendAT("+HTTPACTION=1");
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: HTTP POST CALL FAILED!");
    DBG("+HTTPPARA=\"URL\",\"https://eoh623b6wcgwbw8.m.pipedream.net\"");
  } else {
    Serial.print("Post call response:");
    delay(5000);
    modem.sendAT("+HTTPHEAD");
    delay(2000);
    Serial.println();
    Serial.println("Terminating HTTP");
    modem.sendAT("+HTTPTERM");
    delay(2000);
  }

  Serial.println("Sending Test SMS to a number...");

  #if TINY_GSM_TEST_SMS && defined TINY_GSM_MODEM_HAS_SMS && defined SMS_TARGET1 
    res = modem.sendSMS(SMS_TARGET1, String(modemInfo));
    DBG("SMS sent status:", res ? "SUCCESS" : "FAILED");
    delay(2000);

    res = modem.sendSMS(SMS_TARGET2, String("Hello from Bharat Pi! Board is working."));
    DBG("SMS sent status:", res ? "SUCCESS" : "FAILED");
    delay(2000);

    res = modem.sendSMS(SMS_TARGET3, String("Hello from Bharat Pi! Board is working."));
    DBG("SMS sent status:", res ? "SUCCESS" : "FAILED");
    delay(2000);

  // This is only supported on SIMxxx series
  // res = modem.sendSMS_UTF8_begin(SMS_TARGET);
  // if (res) {
  //   auto stream = modem.sendSMS_UTF8_stream();
  //   stream.print(F("Привіііт! Print number: "));
  //   stream.print(595);
  //   res = modem.sendSMS_UTF8_end();
  // }
  // DBG("UTF8 SMS:", res ? "OK" : "fail");
  #endif

  Serial.println("END OF MODEM TESTING");

  while (1) {
    while (SerialAT.available()) {
      SerialMon.write(SerialAT.read());
    }
    while (SerialMon.available()) {
      SerialAT.write(SerialMon.read());
    }
  }
}
