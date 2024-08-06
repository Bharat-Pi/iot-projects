/*******************************************************************************
  NAME: Bharat Pi NavIC GPS Tracker using SIMCOM 4G Module A7672S test firmware 
  DESCRIPTION: NavIC get latlong and other navigation parameters using Bharat Pi NavIC 
               tracker shield and push to cloud using SIMCOM A7672 4G Module.
  AUTHOR: Bharat Pi
  VERSION: 2.0
  CEATED: 25/03/2024
  
  RIVISION: 
  ------------------------------------------
  DATE:      |  VERSION:  |  DESC:
  ------------------------------------------
  25/03/2024      1.0        Initial release with Lat long and 
                             other nav parameters capture and push to cloud.

  05/08/2024 -    2.0        Switched to Hardware serial as the software serial had issues with baud rates,
                             Added Google maps link so that lat long can be viewed from URL printed on serial monitor                            

*******************************************************************************/

#define TINY_GSM_MODEM_SIM7600 //TINY_GSM_MODEM compatible for 7672 as well
#define TINY_GSM_RX_BUFFER 1024

#define TINY_GSM_TEST_SMS true
#define SMS_TARGET  "9880721666" //Enter you phone number to which you would like to recevied SMS

#define SerialAT Serial1
#define SerialMon Serial

#define GSM_PIN "" //In case if you have a password protection for your simcard

#include <Arduino.h> //Arduino base lib
#include <TinyGsmClient.h> // GSM library for 4G module
#include <SPI.h> //SPI interface
#include <ArduinoJson.h> //Convert to json object
#include <WiFi.h> // Wifi client
#include <Preferences.h> //Local cache

#include <TinyGPS++.h>
#include <HardwareSerial.h>


#define GPS_RX_PIN 33
#define GPS_TX_PIN 32 


TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

const char apn[]  = "airtelgprs.com"; 
const char gprsUser[] = "";
const char gprsPass[] = "";

String send_data_to_url = "https://eogas6eaag50nu2.m.pipedream.net"; // Test URL for demo purposes, Pipedream is a free api test service, you can check if the data is been sent to cloud.
String gpsLatLong = ""; //Get gps lat long 
String dataToSync = "";
String simOperator = "";

String cloudUrl = "<<YOUR_CLOUD_URL>>"; //API or a Callback url to send your data to your production cloud.
int firmWareVersion = 1000;  //Firmware version

//Local cache 
Preferences preferences;
String lastLatLong = "";
String lastSyncTime = "";

long currentMillis = 0;
long previousMillis = 0;
boolean ledState = LOW;

// Node specific variables
String orgID = "3c701cf4-7b2b-42a4-9814-b393bb1628e6"; //Suzuki
String devID = "";
String sensorName = "lat_long_1";
String secID = "";

String sensorData = "00"; 
String payload = "";

//GPS parsing variables
char *gpsValues[20];

int timeout = 60; //GPS latch timeout duration. You can increase this value based on your application/need.
int interval = 10000; //Fetch GPS data upon every 60 seconds (1 min)

StaticJsonDocument<500> payloadObj; //for testing http request

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

void sendSMS(String phoneNumber, String msg){
  Serial.print("Sending SMS to number:");
  Serial.println(phoneNumber);

  #if TINY_GSM_TEST_SMS && defined TINY_GSM_MODEM_HAS_SMS 
    bool res = modem.sendSMS(phoneNumber, String(msg));
    DBG("SMS sent status:", res ? "SUCCESS" : "FAILED");
    delay(2000);
  #endif
}

void postData(String dataTobePushed) {

    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF"); 
    Serial.println("Cloud sync inprogress....");

    String sensorData = "{\"did\":";
    sensorData = sensorData + "\"" + devID + "\",\"oid\":";
    sensorData = sensorData + "\"" + orgID + "\",\"data\":";
    sensorData = sensorData + "[{\"data\":";
    sensorData = sensorData + "\"" + dataTobePushed + "\",\"sid\":";
    sensorData = sensorData + "\"" + sensorName + "\"}";
    sensorData = sensorData + "],\"secret\":";
    sensorData = sensorData + "\"" + secID + "\"}";

    int dataSize = sensorData.length();
    Serial.print("Device json data set to: ");
    Serial.print(sensorData);

    //Push to cloud
    httpPost(sensorData);
    Serial.println();
}

void httpPost(String data){
  Serial.println("Initiating HTTP");
  modem.sendAT("+HTTPINIT"); //init HTTP
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: HTTP INIT FAILED!");
    DBG("+HTTPINIT");
  }
  delay(2000);
  Serial.print("Payload length: ");
  Serial.println(data.length());
  Serial.println("Setting http call back URL");

  modem.sendAT("+HTTPPARA=\"URL\"," + send_data_to_url /*cloudUrl*/); //set call back URL. You can set the cloudUrl variable and uncomment to start using production. 
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: HTTP URL SETTING FAILED!");
    DBG("+HTTPPARA=\"URL\"," + send_data_to_url /*cloudUrl*/);
  }
  modem.sendAT("+HTTPPARA=\"CONTENT\",\"application/json\"");   //For test purpose we are sending plain text data. If sending json data then replace: text/plain -> application/json
  if (modem.waitResponse(10000L) != 1) {
    Serial.println("ERROR: HTTP CONTENT TYPE SETTING FAILED");
    DBG("+HTTPPARA=\"CONTENT\",\"text/plain\"");
  }
  modem.sendAT("+HTTPDATA=" + String(data.length()) + ",20000"); //length of the body (data) and max time required in milliseconds to input the data
  //HTTPDATA responds with DOWNLOAD so need to wait for this and then send the body/data of the post call
  while (modem.waitResponse(1000UL, "DOWNLOAD") != 1) {
      Serial.print(".");
  }
  delay(2000);

  //Send the body/data of the post call
  //JSON payload or it can even be a plain text. 
  //If sending json data then format payload accordingly before sending else it might result in post error
  modem.streamWrite(data); 
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
    Serial.println(data);    
    Serial.print("Post call response:");
    delay(2000);
    modem.sendAT("+HTTPHEAD");
    delay(2000);
    Serial.println();
    Serial.println("Terminating HTTP");
    modem.sendAT("+HTTPTERM");
    delay(2000);
  }  
  data = "";
  sensorData = "";
}

void setup() {
  Serial.begin(115200);  
  gpsSerial.begin(115200, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN); 
  Serial.setDebugOutput(true); 
  delay(5000); //dummy delay for 5 seconds so that serial monitor settles down

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  modemPowerOn();
  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
  Serial.clearWriteError();
  Serial.println();
  Serial.println();
  Serial.println("/**********************************************************/");
  Serial.println("  Bharat Pi 4G/LTE Board NavIC GPS Tracker Test Program");
  Serial.println("  To initialize the network test, please make sure the 4G and NavIC antenna are been");
  Serial.println("  connected and SIM card is inserted in the SIM slot (back side of the board).");
  Serial.println();
  Serial.println("  Please wait while the NavIC module latches to satellites.\n");
  Serial.println("  You will notice the \"Red LED\" (PPS) on NavIC shield blinking once it latches.\n\n");
  Serial.println("  IMPORTANT: Ensure that the antenna has sky visibility and its not cloudy else it may not latch.");
  Serial.println("/**********************************************************/\n\n");

  delay(2000); //Allow some time for modem to power ON.

  Serial.print("Firmware version: ");
  Serial.print(firmWareVersion);
  Serial.println();  
  Serial.println("Booting...");
  WiFi.setHostname("BharatPi-NavIC-Tracker"); //Later if you wish to use WiFi as AP or Station mode
  Serial.print("Hostname set to: ");
  Serial.println(WiFi.getHostname());

  WiFi.begin(); //Initialize wifi so that you can get a Mac address of the device
  devID = WiFi.macAddress(); //Set device ID
  Serial.print("Device ID set to: "); //Use device ID for tagging data to this specific device.
  Serial.println(devID);

  //Set preferences for local storage, this is a persistance memory and remains even after device reset/reboot.
  preferences.begin("BP_CACHE", false);
  
  //If cached previous values then retrive them
  if(preferences.isKey("LAST_LAT_LONG")){
    lastLatLong = preferences.getString("LAST_LAT_LONG", "NA");
  }
  
  if(preferences.isKey("LAST_SYNC_TIME")){
    lastSyncTime = preferences.getString("LAST_SYNC_TIME", "NA");
  }

  Serial.println();
  Serial.println("***** Last Cached Data *****");
  Serial.print("Lat long - ");
  Serial.println(lastLatLong);
  Serial.print("Sync Time - ");
  Serial.println(lastSyncTime);
  Serial.println();
  Serial.println();
  
  //TODO: Need to implement device power ON stat capture. Need a seperate API for this.

  delay(1000);

  String res;
  Serial.println("Initializing Modem...");

  if (!modem.init()) {
    digitalWrite(LED_PIN, HIGH);
    modemRestart();
    delay(2000);
    Serial.println("Failed to restart modem, attempting to continue without restarting");
    digitalWrite(LED_PIN, LOW);
    //return;
  }

  //Blue LED on the board use as an indicator
  //If blinking: Modem not able to boot
  //If turned ON: connected to network
  //If turned OFF: Modem booted successfully but not connected to network, check your SIM, network coverage etc.

  digitalWrite(LED_PIN, LOW); 

  Serial.println("Running SIMCOMATI command...");
  modem.sendAT("+SIMCOMATI"); //Get the module information
  modem.waitResponse(1000L, res);
  //res.replace(GSM_NL "OK" GSM_NL, "");
  Serial.println(res);
  res = "";
  Serial.println();

  Serial.println("Preferred mode selection (GSM/LTE)...");
  modem.sendAT("+CNMP?");
  if (modem.waitResponse(1000L, res) == 1) {
    //res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }
  res = "";
  Serial.println();

  //This section is only applicable for testing modules with NBIoT, 
  //for other modules the command doesnt return anything and can be ignored while testing
  Serial.println("Preferred selection between CAT-M and NB-IoT...");
  modem.sendAT("+CMNB?");
  if (modem.waitResponse(1000L, res) == 1) {
    //res.replace(GSM_NL "OK" GSM_NL, "");
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

  String payload;
  payloadObj["bharat_pi_4g_module_testing"] = modemName;
  payloadObj["modemInfo"] = modemInfo;
  serializeJson(payloadObj, payload); //Convert data to json format
  //payload="Bharat Pi 4G Module Testing";

  // Unlock your SIM card with a PIN if needed (not applicable for regular sim testing)
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
    //res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.println(res);
  }

  //Get SIM operator name
  modem.sendAT("+CSPN?");
  if (modem.waitResponse(1000L, simOperator) == 1) {
    //res.replace(GSM_NL "OK" GSM_NL, "");
    Serial.print("Sim Operator: ");
    Serial.println(simOperator);
  }
  Serial.println();

  delay(1000);  
  Serial.println("");
  Serial.println("");  

  delay(2000);
  Serial.println("Testing a sample HTTPS Call to pipe drive server...");
  Serial.println("NOTE: You can use your own choice of cloud or you can deploy an end point \n on Pipe Dream (https://pipedream.com) which is a free server to test API calls. Example: " + send_data_to_url);
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

  modemInfo += ", ID: ";
  modemInfo += devID;
  modemInfo += ", SIM: ";
  modemInfo += simOperator;

  sendSMS(SMS_TARGET, modemInfo); //Send test SMS
  Serial.println("End of 4G Modem Power ON and testing.");   
  Serial.println();
  Serial.println("Awaiting for NavIC satellite signal latching...");
  Serial.println();
  //getGPSInfo(); 
  //pushGPSDataToCloud();
}

void loop() {
  // Read data from NavIC module and push to a configured cloud URL
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        // Get latitude and longitude
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();
        String  date = gps.date.day() + "/" + gps.date.month();
        String time = gps.time.hour() + ":" + gps.time.minute();
        double speedKnots = gps.speed.knots();
        float  courseDeg = gps.course.deg();
        double altitudeMts = gps.altitude.meters();
        uint32_t satellites = gps.satellites.value();
        double hdop = gps.hdop.hdop();

        // Print latitude and longitude, you can uncommment other parameter if you like to view those as well like date/time etc.
        Serial.print("Lat: ");
        Serial.println(latitude, 7);
        Serial.print("Long: ");
        Serial.println(longitude, 7);
        Serial.print("View lat long on Google Maps ===>>> ");
        Serial.println("http://maps.google.com/maps?q=" + String(latitude, 7) + "," + String(longitude, 7));
        // Serial.print("Date: ");
        // Serial.println(date);
        // Serial.print("Time: ");
        // Serial.println(time);
        Serial.print("Speed (knots): ");
        Serial.println(speedKnots, 7);                                                
        Serial.print("Course (degree): ");
        Serial.println(courseDeg, 7);
        Serial.print("Altitude (Meters): ");
        Serial.println(altitudeMts, 7);    
        Serial.print("Satellites:");
        Serial.println(satellites, 7);  
        Serial.print("HDOP:");
        Serial.println(hdop, 7);    
        Serial.println();
        Serial.println();      
        Serial.print("Pushing NavIC lat long data to cloud....");     
        delay(2000);
        postData(String(latitude, 7) + "," + String(longitude, 7)); //You can create a json object with all the NavIC data and push to cloud
        delay(20000); //Push lat long to cloud every 10 seconds
      }
    }
  }   
}
