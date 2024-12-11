/*************************************************************************
   PROJECT: Bharat Pi 4G Board Sample Code for data push to Thingspeak Cloud
   AUTHOR: Bharat Pi
   CREATED DATE: 06/05/2024
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   version: 0.1.0
 
   FUNC: 4G testing with HTTP call to Thingspeak cloud server.
   
   SIMCARD: 4G sim cards from Airtel/Vodaphone/Jio/BSNL can be used. 
            To test HTTP request ensure you have data plan. 
   
   IMPORTANT: Configure the APN accordingly as per your Sim provider
   
   GPS: If you have bought Bharat Pi wtih GPS board then you need to connect 
        the GPS antenna to the UFL connector and antenna should have clear sky visibility
        preferrably on the terrace or open field.

   TODO: (Before you upload the code to Bharat Pi board) 
   1) Change APN as per your Sim provider
   2) Change the Thingspeak API key as per your account/setup
   3) Use a power adapter 9V 2amps as the 4G module requires enough power to operate
   
   DESC: This script will connect using 4G sim and makes a http call 
   to a Thingspeak cloud server setup as per your Thingspeak API key.
 
   REVISION HISTORY TABLE:
   ------------------------------------------
   Date      | Firmware Version | Comments
   ------------------------------------------
   06/05/2024 -    0.1.0       -    Initial release of sample script.
 *************************************************************************/



#define TINY_GSM_MODEM_SIM7600 //TINY_GSM_MODEM compatible for 7672 as well
#define TINY_GSM_RX_BUFFER 1024

#define SerialAT Serial1
#define SerialMon Serial

#define UART_BAUD   115200
#define PIN_DTR     25
#define PIN_TX      17
#define PIN_RX      16
#define PWR_PIN     32

#define LED_PIN 2

#include <TinyGsmClient.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include <Ticker.h>

/*********************************************
  SECTION: Set APN based on your sim card
    AIRTEL: "airtelgprs.com" 
    BSNL: "bsnlnet" 
    VI: "portalnmms"
    JIO: "jionet"
*********************************************/
const char apn[]  = "airtelgprs.com"; //Change this as per your Sim card operator

/*********************************************
  SECTION: Set Thingspeak cloud and respective API key
*********************************************/
String cloud_url = "https://api.thingspeak.com/update.json"; //Thingspeak update URL for sending data as JSON object
String apiKey = "20MNHFQKV1NFW83I";  // change this to the thingspeak API key as per your account


StaticJsonDocument<200> payloadObj; //for testing http request

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

/*********************************************
  SECTION: Modem operation functions
*********************************************/
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

/*********************************************
  SECTION: Main setup
*********************************************/
void setup(){
  // Set console baud rate
  SerialMon.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  delay(100);

  modemPowerOn();
  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
  Serial.clearWriteError();
  Serial.println();
  Serial.println();
  Serial.println("/**********************************************************/");
  Serial.println("  Bharat Pi 4G/LTE Board Test Program");
  Serial.println("  To initialize the network test, please make sure the antenna has been");
  Serial.println("  connected and SIM card is inserted in the SIM slot (back side of the board).");
  Serial.println("/**********************************************************/\n\n");

  delay(5000);

  String res;
  Serial.println("Initializing Modem...");

  if (!modem.init()) {
    digitalWrite(LED_PIN, HIGH);
    modemRestart();
    delay(2000);
    Serial.println("Failed to restart modem, continue without restarting");
    digitalWrite(LED_PIN, LOW);
    return;
  }

  //Blue LED on the board use as an indicator
  //If blinking: Modem not able to boot
  //If turned ON: connected to network
  //If turned OFF: Modem booted successfully but not connected to network, check your SIM, network coverage etc.

  digitalWrite(LED_PIN, LOW); 

  Serial.println("Running SIMCOMATI command...");
  modem.sendAT("+SIMCOMATI"); //Get the module information
  modem.waitResponse(1000L, res);
  res.replace(AT_NL "OK" AT_NL, "");
  Serial.println(res);
  res = "";
  Serial.println();

  Serial.println("Preferred mode selection (GSM/LTE)...");
  modem.sendAT("+CNMP?");
  if (modem.waitResponse(1000L, res) == 1) {
    res.replace(AT_NL "OK" AT_NL, "");
    Serial.println(res);
  }
  res = "";
  Serial.println();

  //This section is only applicable for testing modules with NBIoT, 
  //for other modules the command doesnt return anything and can be ignored while testing
  Serial.println("Preferred selection between CAT-M and NB-IoT...");
  modem.sendAT("+CMNB?");
  if (modem.waitResponse(1000L, res) == 1) {
    res.replace(AT_NL "OK" AT_NL, "");
    Serial.println(res);
  }
  res = "";
  Serial.println();

  //Get module manufacturer details
  String modemName = modem.getModemName();
  Serial.println("Modem Name : " + modemName);
  delay(1000);

  String modemInfo = modem.getModemInfo();
  Serial.println("Modem Info : " + modemInfo);
  delay(1000);

  /*********************************************
    SECTION: Connect to Sim network 
  *********************************************/
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
      String netoworkOerator = modem.getOperator();
      Serial.print("Operator: ");
      Serial.println(netoworkOerator);
      int16_t signal =  modem.getSignalQuality();
      Serial.print("Signal: ");
      Serial.println(signal);
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
  digitalWrite(LED_PIN, HIGH); //Modem connected to network

  Serial.println();
  Serial.println("Yehhh....Device is connected to Sim network.");
  Serial.println();

  delay(1000);
  Serial.println("Checking UE (User Equipment) system information...");
  Serial.println();
  modem.sendAT("+CPSI?");
  if (modem.waitResponse(1000L, res) == 1) {
    res.replace(AT_NL "OK" AT_NL, "");
    Serial.println(res);
  }

  delay(1000);  
  Serial.println("");
  Serial.println("");  

  delay(2000);

  /******************************************************************
  SECTION: Set dummpy variable for pushing data to cloud.
           If you want to dynamically capture data from 
           sensors or GPIOs then move this section to Loop 
           and set variables accordingly to push data to Thingspeak.
  *******************************************************************/
  String payload; 
  double temperature1 = 56.9; //Dummy vairable to send temperature
  double temperature2 = 23.9; //Dummy vairable to send temperature

  payloadObj["api_key"] = apiKey;
  payloadObj["field1"] = temperature1;
  payloadObj["field2"] = temperature2;

  serializeJson(payloadObj, payload); //Convert data to json format

  Serial.print("Paylod length: ");
  Serial.println(payload.length());  

  Serial.println();
  Serial.println(">>>>> Begin Thingspeak Cloud Sync <<<<<<");
  Serial.println();
  Serial.println("NOTE: Please ensure to have a Thingspeak account and API is setup.");
  Serial.println("Thingspeak URL: " + cloud_url);
  Serial.println("Thingspeak API Key: " + apiKey);
  Serial.println();
  Serial.println();
  delay(2000);

  Serial.println("Initiating HTTP call to Thingspeak");
  modem.sendAT("+HTTPINIT"); //init HTTP
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: HTTP INIT FAILED!");
    DBG("+HTTPINIT");
  }
  delay(5000);

  /******************************************************************
  SECTION: Make an API call to Thingspeak server with sensor data as a Json object
  *******************************************************************/  

  modem.sendAT("+HTTPPARA=\"URL\"," + cloud_url); //set call back URL to test 
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: HTTP URL SETTING FAILED!");
    DBG("+HTTPPARA=\"URL\"," + cloud_url);
  }

  modem.sendAT("+HTTPPARA=\"CONTENT\",\"application/json\"");   //For test purpose we are sending plain text data. If sending json data then replace: text/plain -> application/json
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: HTTP CONTENT TYPE SETTING FAILED");
    DBG("+HTTPPARA=\"CONTENT\",\"application/json\"");
  }
  modem.sendAT("+HTTPDATA=" + String(payload.length()) + ",20000"); //length of the body (data) and max time required in milliseconds to input the data
  //HTTPDATA responds with DOWNLOAD so need to wait for this and then send the body/data of the post call
  while (modem.waitResponse(1000UL, "DOWNLOAD") != 1) {
      Serial.print(".");
  }
  delay(2000);

  //Send the body/data of the post call
  //JSON payload or it can even be a plain text. Read the Thingspeak API reference manual. https://in.mathworks.com/help/thingspeak/rest-api.html
  modem.streamWrite(payload); 
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: SENDING POST DATA");
  }
  delay(2000);
  
  Serial.println("Executing HTTP POST call");
  modem.sendAT("+HTTPACTION=1"); //Execute post
  if (modem.waitResponse(10000UL) != 1) {
    Serial.println("ERROR: HTTP POST CALL FAILED!");
    DBG("+HTTPACTION=1");
  } else {
    Serial.print("Data sent to API: ");
    Serial.println(payload);    
    Serial.print("Post call response:");
    modem.sendAT("+HTTPREAD");
    if(modem.waitResponse(60000L) !=1 ){
      DBG(GF("+HTTPREAD"));
    }
    delay(2000);
    modem.sendAT("+HTTPHEAD");
    delay(2000);
    Serial.println();
    Serial.println("Terminating HTTP");
    modem.sendAT("+HTTPTERM");
    delay(2000);
  }

  Serial.println(">>>>> END OF Thingspeak Cloud Sync <<<<<<");
}

/******************************************************************
SECTION: Loop call for capturing synamic data from sensors/GPIOs
*******************************************************************/  

void loop(){
  //Print data to Arduino serial monitor
  while (1) {
    while (SerialAT.available()) {
      SerialMon.write(SerialAT.read());
    }
    while (SerialMon.available()) {
      SerialAT.write(SerialMon.read());
    }
  }    
}
