/*************************************************************************
   PROJECT: Bharat Pi 4G Board Sample Code for data push to Thingspeak Cloud
   AUTHOR: Bharat Pi
 
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
 
   COPYRIGHT: BharatPi @MIT license for usage on Bharat Pi boards
 *************************************************************************/

#define TINY_GSM_MODEM_SIM7600 //TINY_GSM_MODEM compatible for 7672 as well
#define TINY_GSM_RX_BUFFER 1024
//#define DUMP_AT_COMMANDS true

#define SerialAT Serial1
#define SerialMon Serial

#define UART_BAUD   115200
#define PIN_DTR     25
#define PIN_TX      17
#define PIN_RX      16
#define PWR_PIN     32

#define LED_PIN 2

#include <TinyGsmClient.h> //Lib version - 0.12.0
#include "SSLClient.h" //Lib version - 2.1.7
#include <PubSubClient.h> //Lib version - 2.8

// certificate for https://mqtt3.thingspeak.com
// DigiCert Global Root G2, valid until Sun Mar 30 2031, size: 1720 bytes 
// const char* rootCACertificate = \
// "-----BEGIN CERTIFICATE-----\n" \
// "MIIEyDCCA7CgAwIBAgIQDPW9BitWAvR6uFAsI8zwZjANBgkqhkiG9w0BAQsFADBh\n" \
// "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
// "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n" \
// "MjAeFw0yMTAzMzAwMDAwMDBaFw0zMTAzMjkyMzU5NTlaMFkxCzAJBgNVBAYTAlVT\n" \
// "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxMzAxBgNVBAMTKkRpZ2lDZXJ0IEdsb2Jh\n" \
// "bCBHMiBUTFMgUlNBIFNIQTI1NiAyMDIwIENBMTCCASIwDQYJKoZIhvcNAQEBBQAD\n" \
// "ggEPADCCAQoCggEBAMz3EGJPprtjb+2QUlbFbSd7ehJWivH0+dbn4Y+9lavyYEEV\n" \
// "cNsSAPonCrVXOFt9slGTcZUOakGUWzUb+nv6u8W+JDD+Vu/E832X4xT1FE3LpxDy\n" \
// "FuqrIvAxIhFhaZAmunjZlx/jfWardUSVc8is/+9dCopZQ+GssjoP80j812s3wWPc\n" \
// "3kbW20X+fSP9kOhRBx5Ro1/tSUZUfyyIxfQTnJcVPAPooTncaQwywa8WV0yUR0J8\n" \
// "osicfebUTVSvQpmowQTCd5zWSOTOEeAqgJnwQ3DPP3Zr0UxJqyRewg2C/Uaoq2yT\n" \
// "zGJSQnWS+Jr6Xl6ysGHlHx+5fwmY6D36g39HaaECAwEAAaOCAYIwggF+MBIGA1Ud\n" \
// "EwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFHSFgMBmx9833s+9KTeqAx2+7c0XMB8G\n" \
// "A1UdIwQYMBaAFE4iVCAYlebjbuYP+vq5Eu0GF485MA4GA1UdDwEB/wQEAwIBhjAd\n" \
// "BgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwdgYIKwYBBQUHAQEEajBoMCQG\n" \
// "CCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5jb20wQAYIKwYBBQUHMAKG\n" \
// "NGh0dHA6Ly9jYWNlcnRzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RH\n" \
// "Mi5jcnQwQgYDVR0fBDswOTA3oDWgM4YxaHR0cDovL2NybDMuZGlnaWNlcnQuY29t\n" \
// "L0RpZ2lDZXJ0R2xvYmFsUm9vdEcyLmNybDA9BgNVHSAENjA0MAsGCWCGSAGG/WwC\n" \
// "ATAHBgVngQwBATAIBgZngQwBAgEwCAYGZ4EMAQICMAgGBmeBDAECAzANBgkqhkiG\n" \
// "9w0BAQsFAAOCAQEAkPFwyyiXaZd8dP3A+iZ7U6utzWX9upwGnIrXWkOH7U1MVl+t\n" \
// "wcW1BSAuWdH/SvWgKtiwla3JLko716f2b4gp/DA/JIS7w7d7kwcsr4drdjPtAFVS\n" \
// "slme5LnQ89/nD/7d+MS5EHKBCQRfz5eeLjJ1js+aWNJXMX43AYGyZm0pGrFmCW3R\n" \
// "bpD0ufovARTFXFZkAdl9h6g4U5+LXUZtXMYnhIHUfoyMo5tS58aI7Dd8KvvwVVo4\n" \
// "chDYABPPTHPbqjc1qCmBaZx2vN4Ye5DUys/vZwP9BFohFrH/6j/f3IL16/RZkiMN\n" \
// "JCqVJUzKoZHm1Lesh3Sz8W2jmdv51b2EQJ8HmA==\n" \
// "-----END CERTIFICATE-----\n" \
// "";


/*********************************************
  SECTION: Set APN based on your sim card
    AIRTEL: "airtelgprs.com" 
    BSNL: "bsnlnet" 
    VI: "portalnmms"
    JIO: "jionet"
*********************************************/
const char apn[]  = "airtelgprs.com"; //Change this as per your Sim card operator

/*********************************************
  SECTION: Set Thingspeak cloud specific
*********************************************/
const char broker[] = "mqtt3.thingspeak.com"; //change this to as per your server MQTT broker 
const int port = 1883; //MQTT port 
const char* channelID = "2456524"; //Thingspeak channel ID, set this to as per your channels on thingspeak

//thingspeak MQTT Client ID settings. Refer thingspeak MQTT settings
const char* mqttClientID = "Ow0UFh47FzsqHj00JAAuGgo"; 
const char* mqttUsername = "Ow0UFh47FzsqHj00JAAuGgo";
const char* mqttPassword = "7kq9ySBZd7oxWZln3v58/7R8";

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

TinyGsmClient client(modem);
//SSLClient secure_client(&client);
PubSubClient  mqtt(client);


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

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void mqttConnect(String payload) {
  SerialMon.print("Setting MQTT broker: ");
  SerialMon.println(broker);

  mqtt.setServer(broker, port);
  mqtt.setBufferSize(2048);

  // Connect to MQTT Broker
  SerialMon.println("Connecting to MQTT broker");
  if (!mqtt.connect(mqttClientID, mqttUsername, mqttPassword)) {
    Serial.println("MQTT Client connect failed");
  } else {
    Serial.println("MQTT Client connect success...");
    const char* topicString = "channels/2456524/publish"; //Refer thingspeak on how to set the MQTT topics
    const char* message = payload.c_str();
    Serial.println("MQTT publish in progress...");
    if(mqtt.connected()){
      if(mqtt.publish(topicString, message)){
        Serial.println("Data published to MQTT server: " + payload);
      } else {
        Serial.println("ERROR: Data publish failed to MQTT server!");
      }
    } else {
      Serial.println("MQTT is disconnected!");
    }

  }
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
  //secure_layer.setCACert(root_ca);
  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
  Serial.clearWriteError();
  Serial.println();
  Serial.println();
  Serial.println("/*******************************************************************************/");
  Serial.println("  Bharat Pi Thingspeak Cloud Sync using MQTT over 4G/LTE ");
  Serial.println("");
  Serial.println("  IMPORTANT: To initialize/latch the 4G/LTE network, please make sure the antenna has been");
  Serial.println("  connected, SIM is inserted in the SIM slot (back side of the board) and 9V 2A power adapter is connected.");
  Serial.println("/******************************************************************************/\n\n");

  delay(2000);

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
  String modemName = modem.getModemModel();
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

  if(modem.isNetworkConnected()){
    Serial.println("Mobile Network is connected.......");
  }  
  // GPRS connection parameters are usually set after network registration
  SerialMon.print(F("DATA Connection: Connecting to APN: "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, "", "")) {
    Serial.println("APN connect failed");
    delay(10000);
    return;
  }
  Serial.println("APN connect success");

  if (modem.isGprsConnected()) { 
    Serial.println("");
    Serial.println("GPRS network is connected");
  }
  Serial.println("");

  /******************************************************************
  SECTION: Set variables and values for pushing data to cloud via MQTT.
           If you want to dynamically capture data from 
           sensors or GPIOs then move this section to Loop 
           and set variables accordingly to push data to Thingspeak.
  *******************************************************************/
  String payload; 
  double temperature1 = 32.9; //Dummy vairable to send temperature
  double temperature2 = 36.2; //Dummy vairable to send temperature

  //Prepare paylod if using the MQTT 
  payload = "field1=" + String(temperature1) + "&field2=" + String(temperature2) + "&status=MQTTPUBLISH";

  Serial.print("Paylod length: ");
  Serial.println(payload.length());  

  Serial.println();
  Serial.println(">>>>> Begin Thingspeak Cloud Sync using MQTT <<<<<<");
  Serial.println();
  
  Serial.println("NOTE: Please ensure to have a Thingspeak MQTT setup for your channel.\n"
  "You can find the steps on Thingspeak portal on how to set MQTT for a channel and use\n"
  "the channel IDs to publish the fields (sensors) data.");

  //MQTT settings
  Serial.println("");
  Serial.println("  Thingspeak MQTT: " + String(broker));
  Serial.println("  Thingspeak Channel: " + String(channelID));

  Serial.println();
  Serial.println();
  delay(2000);

  Serial.println("Initiating MQTT connect to Thingspeak");

  /******************************************************************
  SECTION: Make an API call to Thingspeak server with sensor data as a Json object
  *******************************************************************/  

  Serial.println("...........MQTT connect and publish in progress..............");
  //secure_client.setCACert(rootCACertificate);
  //delay(5000);
  //Serial.println("Root certificate set successfully................");
  mqttConnect(payload);

  Serial.println("");
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