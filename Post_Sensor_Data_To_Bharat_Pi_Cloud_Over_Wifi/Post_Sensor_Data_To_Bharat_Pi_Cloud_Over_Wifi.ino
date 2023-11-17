/*
  Name: Bharat Pi Firmware for WiFi based water/liquid flow monitoring
  Description: This firmware syncs the data to Bharat Pi Cloud 
  flow sensors and send to Bharat Pi Cloud backend.
  Author: Bharat Pi
  Version: 1.0.2
  Created Date: 27/10/2023
  
  Revision History Table: 
  ------------------------------------
  Date      | Version | Comments
  ------------------------------------
  27/10/2023 - 1001 - Added Simple data sync settings to Cloud 


*/


#include <HTTPClient.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>

const char* ssid = "<YOUR_WIFI_SSID>";
const char* password = "<YOUR_WIFI_PASSWORD>";


WiFiClientSecure* client = new WiFiClientSecure;

// Bharat Pi Webhook URL 
static const char *syncUrl = "https://api.refillbot.in/api/webhook/high/device";

int dataSyncInterval = 60000;
int dataSyncCurrentMillis = 0;
int dataSyncPreviousMillis = 0;
int LED_WIFI = 2;
String devID = "";
// Bharat Pi Cloud Organization ID, Contact Bharatpi.net for this ID
String orgID = "ORG_ID";
String sensorName = "FL1"; // Sensor Identification Name.
String secID = "";
//Sensor Data which is changing 
String sensorData = "00";
String payload = "";

///////FLOW SENSOR SPECIFIC VARIABLES//////// CHange this for your sensor
volatile byte pulseCount = 0;
float flowRate = 0.0;
unsigned int flowMilliLitres = 0;
unsigned long totalMilliLitres = 0;
long previousMillis = 0;
long currentMillis = 0;
int interval = 1000;
byte pulse1Sec = 0;
byte totalPulseCount = 0;
float calibrationFactor = 2.5;

//Bharat Pi Cloud Public Certificate
const char* rootCACertificate =
  "-----BEGIN CERTIFICATE-----\n"
  "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\n"
  "-----END CERTIFICATE-----\n";

#define SENSOR 4

void IRAM_ATTR pulseCounter() {
  pulseCount++;
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_WIFI, OUTPUT);
  pinMode(SENSOR, INPUT_PULLUP);
  digitalWrite(LED_WIFI, LOW);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    digitalWrite(LED_WIFI, HIGH);
  }


  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;

  dataSyncPreviousMillis = millis();

  Serial.println();
  Serial.println();
  Serial.println("Booting...");
  WiFi.setHostname("BharatPi-Node-Wifi-001");
  Serial.println("Hostname set to: ");
  Serial.println(WiFi.getHostname());

  devID = WiFi.macAddress(); //Set device ID
  Serial.print("Device ID set to: ");
  Serial.println(devID);
  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);

}

void loop() {
  // put your main code here, to run repeatedly:
  flowCompute();
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_WIFI, LOW);
    Serial.print("WiFi Connection lost. Status code: ");
    Serial.println(WiFi.status());
  } else {
    //WiFi is connected sync data and show WiFi LED ON
    digitalWrite(LED_WIFI, HIGH);
    //Serial.println("LED ON");
    //    Serial.println("WiFi Connection Restored! ");
    String dataToSync = String(totalMilliLitres / 1000); //+ "," + String(totalPulseCount);
    postData(String(dataToSync));
  }
}


void flowCompute(){
    currentMillis = millis();
    if (currentMillis - previousMillis > interval) {

      pulse1Sec = pulseCount;
      totalPulseCount += pulseCount;
      pulseCount = 0;

      // Because this loop may not complete in exactly 1 second intervals we calculate
      // the number of milliseconds that have passed since the last execution and use
      // that to scale the output. We also apply the calibrationFactor to scale the output
      // based on the number of pulses per second per units of measure (litres/minute in
      // this case) coming from the sensor.
      flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
      previousMillis = millis();

      // Divide the flow rate in litres/minute by 60 to determine how many litres have
      // passed through the sensor in this 1 second interval, then multiply by 1000 to
      // convert to millilitres.
      flowMilliLitres = (flowRate / 60) * 1000;

      // Add the millilitres passed in this second to the cumulative total
      totalMilliLitres += flowMilliLitres;

      //Cache locally last read flow data
//      preferences.putString("LAST_DATA", String(totalMilliLitres));
//      preferences.putString("LAST_DATA_SYNC", lastEMDataCacheTime);

      // Print the flow rate for this second in litres / minute
      Serial.print("Flow rate: ");
      Serial.print(int(flowRate));  // Print the integer part of the variable
      Serial.print("L/min");
      Serial.print("\t");  // Print tab space

      // Print the cumulative total of litres flowed since starting
      Serial.print("Output Liquid Quantity: ");
      Serial.print(totalMilliLitres);
      Serial.print("mL / ");
      Serial.print(totalMilliLitres / 1000);
      Serial.println("L");
    }
}


// Data Post Demo 
void postData(String dataTobePushed) {

  dataSyncCurrentMillis = millis();

  if (dataSyncCurrentMillis - dataSyncPreviousMillis > dataSyncInterval) {
    dataSyncPreviousMillis = dataSyncCurrentMillis;
    digitalWrite(LED_WIFI, LOW);
    Serial.println("LED OFF");
    Serial.println("Cloud sync inprogress....");
    //Secure wifi client to make https calls
    WiFiClientSecure* client = new WiFiClientSecure;

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

    if (client) {
      client->setCACert(rootCACertificate);

      {
        // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
        HTTPClient https;

        Serial.print("[HTTPS] begin...\n");
        if (https.begin(*client, syncUrl)) {  // TODO: HTTPS dev server to be changed for production
          https.addHeader("Content-Type", "application/json");
          Serial.print("[HTTPS] POST...\n");
          // start connection and send HTTP header
          int httpCode = https.POST(sensorData);

          // httpCode will be negative on error
          if (httpCode == 200) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTPS] POST return code: %d\n", httpCode);

            // Get return code
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
              payload = https.getString();
              Serial.println(payload);
            }
            digitalWrite(LED_WIFI, HIGH);
            Serial.println("LED ON");
          } else {
            Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
            digitalWrite(LED_WIFI, LOW);
            Serial.println("LED OFF [HTTPS] POST... failed, error");
          }
          https.end();
        } else {
          Serial.printf("[HTTPS] Unable to connect\n");
          digitalWrite(LED_WIFI, LOW);
          Serial.println("LED OFF Unable to connect");
        }
        // End extra scoping block not for now
      }
      delete client;
    } else {
      Serial.println("ERROR: Unable to create secure WiFi client. Check certificate.");
    }
    Serial.println();
  }
}
