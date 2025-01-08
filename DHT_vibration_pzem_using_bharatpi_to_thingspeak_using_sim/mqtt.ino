#define TINY_GSM_MODEM_SIM7600 // TINY_GSM_MODEM compatible for 7672 as well
#define TINY_GSM_RX_BUFFER 1024
//#define DUMP_AT_COMMANDS true

#define SerialAT Serial1
#define SerialMon Serial

#define UART_BAUD   115200 
#define PIN_DTR     25
#define PIN_TX      17
#define PIN_RX      16
#define PWR_PIN     32
#define DHTPIN      23 // Define the GPIO pin for the DHT sensor
#define LED_PIN 2
#define VIBRATION_PIN 5 // Define the GPIO pin for the vibration sensor
#define BUTTON_PIN 14
#define PZEM_RX_PIN 16 // Define the RX pin for PZEM
#define PZEM_TX_PIN 17  // Define the TX pin for PZEM

#include <TinyGsmClient.h> // Lib version - 0.12.0
#include "SSLClient.h" // Lib version - 2.1.7
#include <PubSubClient.h> // Lib version - 2.8
#include <DHT.h>
#include <PZEM004Tv30.h> // Include the PZEM library
#include <WiFi.h>
#include <WiFiManager.h> // Lib version - 2.0.4-beta
#include <stdlib.h>

#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
PZEM004Tv30 pzem(Serial1, PZEM_RX_PIN, PZEM_TX_PIN);

char mqtt_broker[40] = "mqtt3.thingspeak.com";
char mqtt_port[6] = "1883";
char mqtt_client_id[40] = "Ow0UFh47FzsqHj00JAAuGgo";
char mqtt_username[40] = "Ow0UFh47FzsqHj00JAAuGgo";
char mqtt_password[40] = "7kq9ySBZd7oxWZln3v58/7R8";

WiFiManager wm;

const char apn[]  = "jionet"; // Replace with your actual APN


#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

TinyGsmClient client(modem);
//SSLClient secure_client(&client);
PubSubClient mqtt(client);

void modemPowerOn() {
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(1000);
  digitalWrite(PWR_PIN, HIGH);
}

void modemPowerOff() {
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(1500);
  digitalWrite(PWR_PIN, HIGH);
}

void modemRestart() {
  modemPowerOff();
  delay(1000);
  modemPowerOn();
}

void mqttConnect(String payload) {
  SerialMon.print("Setting MQTT broker: ");
  SerialMon.println(mqtt_broker);

  mqtt.setServer(mqtt_broker, atoi(mqtt_port));
  mqtt.setBufferSize(2048);

  SerialMon.println("Connecting to MQTT broker");
  if (!mqtt.connect(mqtt_client_id, mqtt_username, mqtt_password)) {
    Serial.println("MQTT Client connect failed");
  } else {
    Serial.println("MQTT Client connect success...");
    const char* topicString = "channels/2456524/publish"; 
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

bool connectMQTT() {
  if (!mqtt.connected()) {
    Serial.println("MQTT client not connected, reconnecting...");
    if (mqtt.connect(mqtt_client_id, mqtt_username, mqtt_password)) {
      Serial.println("MQTT client connected");
      return true;
    } else {
      Serial.print("Failed to connect MQTT client, rc=");
      int mqttState = mqtt.state();
      Serial.println(mqttState);
      Serial.print("Error description: ");
      switch (mqttState) {
        case -2:
          Serial.println("Connection timeout");
          break;
        case -3:
          Serial.println("Connection lost");
          break;
        case -4:
          Serial.println("Connect failed");
          break;
        case -5:
          Serial.println("Disconnected");
          break;
        case -6:
          Serial.println("Bad client ID");
          break;
        case -7:
          Serial.println("Unauthorized");
          break;
        default:
          Serial.println("Unknown error");
          break;
      }
      delay(5000); // Wait before retrying
      return false;
    }
  }
  return true;
}

bool sendToThingspeak(float humidity, float temperature, int vibration, int buttonState) {
  // Generate random values for PZEM data
  float voltage = random(210, 220); // Simulate voltage between 210V to 240V
  float current = random(1, 10) / 10.0; // Simulate current between 0.1A to 1.0A
  float power = voltage * current; // Simulate power based on voltage and current
  float energy = random(100, 300); // Simulate energy in Wh
  float frequency = random(49, 51); // Simulate frequency between 49Hz to 51Hz
  float powerFactor = random(80, 100) / 100.0; // Simulate power factor between 0.80 to 1.00

  String payload = "field1=" + String(temperature) +
                    "&field2=" + String(humidity) +
                    "&field3=" + String(vibration) +
                    "&field4=" + String(buttonState) +
                    "&field5=" + String(current) +
                    "&field6=" + String(frequency) +
                    "&field7=" + String(energy) +
                    "&field8=" + String(powerFactor) +
                    "&status=MQTTPUBLISH";

  if (connectMQTT()) {
    Serial.println("Publishing data to ThingSpeak...");
    if (mqtt.publish("channels/2456524/publish", payload.c_str())) {
      Serial.println("Data published successfully");
      return true;
    } else {
      Serial.println("Failed to publish data");
      return false;
    }
  }
  return false;
}
int readVibration() {
  return digitalRead(VIBRATION_PIN);
}

void setup() {
  SerialMon.begin(115200);
  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(5, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pzem.setAddress(0x01);
  delay(100);

  // Start WiFi connection with WiFiManager
  WiFi.mode(WIFI_STA);
  wm.setSaveConfigCallback([]() {
    Serial.println("MQTT configuration saved.");
  });

  WiFiManagerParameter custom_mqtt_broker("server", "MQTT Broker", mqtt_broker, 40);
  WiFiManagerParameter custom_mqtt_port("port", "MQTT Port", mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_client_id("client_id", "MQTT Client ID", mqtt_client_id, 40);
  WiFiManagerParameter custom_mqtt_username("username", "MQTT Username", mqtt_username, 40);
  WiFiManagerParameter custom_mqtt_password("password", "MQTT Password", mqtt_password, 40);

  wm.addParameter(&custom_mqtt_broker);
  wm.addParameter(&custom_mqtt_port);
  wm.addParameter(&custom_mqtt_client_id);
  wm.addParameter(&custom_mqtt_username);
  wm.addParameter(&custom_mqtt_password);

  if (!wm.autoConnect("Max_byte")) {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
    delay(5000);
  }

  strcpy(mqtt_broker, custom_mqtt_broker.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(mqtt_client_id, custom_mqtt_client_id.getValue());
  strcpy(mqtt_username, custom_mqtt_username.getValue());
  strcpy(mqtt_password, custom_mqtt_password.getValue());

  Serial.println("WiFi connected, setting up APN for GPRS...");

  // Initialize the modem after WiFi is connected
  modemPowerOn();
  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
  Serial.clearWriteError();

  delay(2000);

  if (!modem.init()) {
    digitalWrite(LED_PIN, HIGH);
    modemRestart();
    delay(2000);
    Serial.println("Failed to restart modem, continue without restarting");
    digitalWrite(LED_PIN, LOW);
    return;
  }

  digitalWrite(LED_PIN, LOW); 
  Serial.println("Modem initialized");

  // Now, set up the GPRS connection
  if (!modem.gprsConnect(apn, "", "")) {
    Serial.println("APN connect failed");
    delay(10000);
    return;
  }

  Serial.println("APN connect success");

  if (modem.isGprsConnected()) { 
    Serial.println("GPRS network is connected");
  }

  Serial.println("Connecting to ThingSpeak MQTT Broker...");
  mqtt.setServer(mqtt_broker, atoi(mqtt_port));

  Serial.println("Device setup complete.");
}

void loop() {
  wm.process();

  delay(2000); // For testing, delay before sending data
  
  // Read sensor data
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int vibration = readVibration();
  int buttonState = digitalRead(BUTTON_PIN);

  // Send data to Thingspeak
  sendToThingspeak(h, t, vibration, buttonState);
}


