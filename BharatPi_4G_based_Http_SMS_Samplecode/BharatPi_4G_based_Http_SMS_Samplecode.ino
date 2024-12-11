/*************************************************************************
   PROJECT: Bharat Pi 4G Board Sample Code
   AUTHOR: Bharat Pi
   CREATED DATE: 04/09/2023
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
   version: 0.1.0
 
   FUNC: 4G testing with HTTP call to a url and send SMS.
   SIMCARD: 4G sim cards from Airtel/Vodaphone/Jio/BSNL can be used. To test HTTP request ensure you have data plan and sms pack. 
   
   IMPORTANT: Configure the APN accordingly as per your Sim provider
   TODO: (Before you upload the code to Bharat Pi board) 
   1) Add phone number(s) 
   2) Change APN as per your Sim provider
   3) Setup a test http URL in pipedream.com
   4) Use a power adapter 9V 2amps as the 4G module requires enough power while it operates.
   
   DESC: This script will connect using 4G sim and send a sms and make a http call 
   to a test url setup on Pipe Dream. You need to set your own Pipe dream url 
   and configure the same in this code to be able to receive a call.

   REVISION HISTORY TABLE:
   ------------------------------------------
   Date      | Firmware Version | Comments
   ------------------------------------------
   04/09/2023 -    0.1.0       -    Initial release of sample script.
   22/06/2024 -    0.1.1       -    TinyGSM lib updated to latest version,
                                    code changes to reflect new method names.
 *************************************************************************/


#define TINY_GSM_MODEM_SIM7600 //TINY_GSM_MODEM compatible for 7672 as well
#define TINY_GSM_RX_BUFFER 1024

#define TINY_GSM_TEST_SMS true
#define SMS_TARGET1  "9880721666" //Enter you phone number to which you would like to recevied SMS
//You can add multiple phone number to get SMS
//#define SMS_TARGET2  "xxxxxxxxxx" //Enter you phone number to which you would like to recevied SMS
//#define SMS_TARGET3  "xxxxxxxxxx" //Enter you phone number to which you would like to recevied SMS

#define SerialAT Serial1
#define SerialMon Serial

#define GSM_PIN "" //In case if you have a password protection for your simcard

//Set APN as per your sim card:
//Airtel -> "airtelgprs.com" 
//BSNL -> "bsnlnet" 
//Voda -> portalnmms
//jio -> jionet
const char apn[]  = "airtelgprs.com"; 
const char gprsUser[] = "";
const char gprsPass[] = "";

//API/Callback url to send your data to cloud. In this example we have used 
//Pipedream which will allow to create a dummy call back URL and send data for testing purpose. 
//Create your account in Pipedream and deploy a call back URL and assign that url to this variable.
String send_data_to_url = "https://eogas6eaag50nu2.m.pipedream.net";
String gpsLatLong = ""; //Get gps lat long 

int timeout = 60; //GPS latch timeout duration. You can increase this value based on your application/need.

#include <TinyGsmClient.h> //Library version compatible 0.12.0
#include <SPI.h> //ESP32 SPI Lib version - 2.0.16
#include <ArduinoJson.h> //Lib version - 7.0.3
#include <Ticker.h> //Lib version - 4.4.0

StaticJsonDocument<200> payloadObj; //for testing http request

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
    Serial.println("Failed to restart modem, attempting to continue without restarting");
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
  String modemName = modem.getModemModel();
  Serial.println("Modem Name : " + modemName);
  delay(1000);

  String modemInfo = modem.getModemInfo();
  Serial.println("Modem Info : " + modemInfo);
  delay(1000);

  String payload;
  payloadObj["bharat_pi_4g_module_testing"] = modemName;
  payloadObj["modemInfo"] = modemInfo;
  serializeJson(payloadObj, payload); //Convert data to json format
  //payload="Bharat Pi 4G Module Testing";

  // Unlock your SIM card with a PIN if needed (not applicable for regular sim testing)
  if ( 
    GSM_PIN && modem.getSimStatus() != 3 ) {
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
  Serial.println("Testing the a sample HTTPS Call to pipe drive server...");
  Serial.println("NOTE: Please ensure to deploy an end point on Pipe Dream (pipedream.com) to test. Example: " + send_data_to_url);
  Serial.println();
  Serial.println();
  Serial.println("MODEM TESTING IN PROGRESS....");
  Serial.println();
  delay(5000);
  Serial.println("Initiating HTTP");
  modem.sendAT("+HTTPINIT"); //init HTTP
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: HTTP INIT FAILED!");
    DBG("+HTTPINIT");
  }
  delay(5000);
  Serial.print("Paylog length: ");
  Serial.println(payload.length());
  Serial.println("Setting http call back URL");

  //modem.sendAT("+HTTPPARA=\"URL\",https://eogas6eaag50nu2.m.pipedream.net"); //set call back URL to test 
  modem.sendAT("+HTTPPARA=\"URL\"," + send_data_to_url); //set call back URL to test 
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: HTTP URL SETTING FAILED!");
    DBG("+HTTPPARA=\"URL\"," + send_data_to_url);
  }
  modem.sendAT("+HTTPPARA=\"CONTENT\",\"text/plain\"");   //For test purpose we are sending plain text data. If sending json data then replace: text/plain -> application/json
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: HTTP CONTENT TYPE SETTING FAILED");
    DBG("+HTTPPARA=\"CONTENT\",\"text/plain\"");
  }
  modem.sendAT("+HTTPDATA=" + String(payload.length()) + ",20000"); //length of the body (data) and max time required in milliseconds to input the data
  //HTTPDATA responds with DOWNLOAD so need to wait for this and then send the body/data of the post call
  while (modem.waitResponse(1000UL, "DOWNLOAD") != 1) {
      Serial.print(".");
  }
  delay(2000);

  //Send the body/data of the post call
  //JSON payload or it can even be a plain text. 
  //If sending json data then format payload accordingly before sending else it might result in post error
  modem.streamWrite(payload); 
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: SENDING POST DATA");
  }
  delay(2000);
  Serial.println("Executing POST call");
  modem.sendAT("+HTTPACTION=1"); //Execute post
  if (modem.waitResponse(1000UL) != 1) {
    Serial.println("ERROR: HTTP POST CALL FAILED!");
    DBG("+HTTPACTION=1");
  } else {
    Serial.print("Data sent to API: ");
    Serial.println(payload);    
    Serial.print("Post call response:");
    delay(2000);
    modem.sendAT("+HTTPHEAD");
    delay(2000);
    Serial.println();
    Serial.println("Terminating HTTP");
    modem.sendAT("+HTTPTERM");
    delay(2000);
  }

  Serial.println("Sending Test SMS to number(s):");
  Serial.println(SMS_TARGET1);
  
  //Uncomment below to print if you have set more than one phone numbers
  //Serial.println(SMS_TARGET2);
  //Serial.println(SMS_TARGET3);

  #if TINY_GSM_TEST_SMS && defined TINY_GSM_MODEM_HAS_SMS && defined SMS_TARGET1 
    res = modem.sendSMS(SMS_TARGET1, String(modemInfo));
    DBG("SMS sent status:", res ? "SUCCESS" : "FAILED");
    delay(2000);

    //Uncomment below code if you wish to send SMS to more than one number. 
    // res = modem.sendSMS(SMS_TARGET2, String("Hello from Bharat Pi! Board is working."));
    // DBG("SMS sent status:", res ? "SUCCESS" : "FAILED");
    // delay(2000);

    // res = modem.sendSMS(SMS_TARGET3, String("Hello from Bharat Pi! Board is working."));
    // DBG("SMS sent status:", res ? "SUCCESS" : "FAILED");
    // delay(2000);
  #endif

  Serial.println("END OF MODEM TESTING");

  //******************************************************************************************
  //
  //    GPS/GNSS TESTING SAMPLE CODE.
  //    The below code run only if the 4G module supports GPS/GNSS
  //    so it is safe to ignore if you are testing only 4G modem
  //    Bharat Pi 4G boards are available with and wihtout GPS. 
  //    Kindly check which one you have bought before testing GPS/GNSS code
  //******************************************************************************************

  //GPS/GNSS Configuration. Below AT commands will power ON the GNSS
  //Switch GNSS Ports and Assisted GPS, once successfully ready then it make a 
  //get GPS co-ordinates call.
  // For full set of GPS/GNSS AT commands refer the documents/manual for Simcom A7672S AT Commands here: https://www.simcom.com/product/A7672X.html
  
  //***** IMPORTANT NOTE *******
  //If you are inside a building or a covered area then you wont be able to get 
  //any GPS co-orindates, you need to be in open sky to request for GPS data (Lat/Long) 
  Serial.println("Enabling GPS/GNSS");
  Serial.println("Checking if modem supports GPS/GNSS...");
  if(modemName == "A7672S-LASC"){ //FASE->With GPS/GNSS module
    Serial.println("Modem doesnt support GPS/GNSS, so no need of GPS testing.");
    return;
  } else if (modemName == "A7672S-FASE"){
    Serial.println("GPS/GNSS is supported!");
    Serial.println("Powering on GPS/GNSS...");
    modem.sendAT("+CGNSSPWR=1"); //Power on the GNSS
    while (modem.waitResponse(1000UL, "+CGNSSPWR: READY!") != 1) {
      Serial.print(".");
    }
    Serial.println("GPS/GNSS Power ON READY!");
    Serial.println("Switching GPS/GNSS Port");
    modem.sendAT("+CGNSSPORTSWITCH=1,1"); //Port switch for GNSS. For more details on the AT commands refer the AT command manual for SimCom A7276S 4G Module
    while (modem.waitResponse(1000UL, "OK") != 1) {
      Serial.print(".");
    }
    Serial.println("GPS/GNSS port switch success");
    Serial.println("Enabling Assisted GPS");
    modem.sendAT("+CAGPS"); //Assisted GPS
    while (modem.waitResponse(1000UL, "+AGPS: success.") != 1) {
      Serial.print(".");
    }
    Serial.println("GNSS assisted GPS success");
    Serial.println("Fetching GPS co-ordinates...");
    modem.sendAT("+CGNSSINFO");
    modem.waitResponse(1000UL, gpsLatLong); //Wait and get the latlong response from the CGNSSINFO AT command. (NOTE: Do not attempt to use cold/hot boot AT commands for GPS/GNSS as this might send the GNSS into shutdown and will require you to do a hard power ON. Hard power ON of GNSS is currently not supported on this board so take care before using Cold/Hot boot AT commands.)
    Serial.print("CGPS INFO: ");
    Serial.println(gpsLatLong);
    Serial.println("Awaiting for GPS latch: "); //Awaiti until the GPS latches as we get the Lat Long info
    
    //Try GPS latch until timeout
    for(timeout=30; timeout != 0; timeout--){
      modem.sendAT("+CGNSSINFO");
      modem.waitResponse(1000UL, gpsLatLong);
      if(gpsLatLong.indexOf("+CGNSSINFO: ,,,,,,,,") > 0){
        Serial.print(".");
        Serial.print(timeout);
      } else{
        Serial.print("**** GPS Latched ****");
        modem.sendAT("+CGNSSINFO");
        modem.waitResponse(1000UL, gpsLatLong); //Wait and get the latlong response from the CGNSSINFO AT command. (NOTE: Do not attempt to use cold/hot boot AT commands for GPS/GNSS as this might send the GNSS into shutdown and will require you to do a hard power ON. Hard power ON of GNSS is currently not supported on this board so take care before using Cold/Hot boot AT commands.)
        Serial.println(gpsLatLong);
        return;
      }
      delay(1000);
    }
    if(timeout == 0){
      Serial.println(">>>>> TIME OUT: GPS Latch timedout. <<<<<<");
      Serial.println("Please ensure you are outside and have sky visibility in order for GPS to latch initially. Without this the GPS wont latch and latlong will not be available.");
    }    
    Serial.println("GPS testing ended.");
  } else {
    Serial.println("######### Could not find SIMCOM A7672S Module! ##########");
  }
  Serial.println(">>>>> End of 4G Module Testing <<<<<<");
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
