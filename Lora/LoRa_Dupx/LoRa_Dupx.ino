/*
  LoRa Duplex communication

  Sends a message every half second, and polls continually
  for new incoming messages. Implements a one-byte addressing scheme,
  with 0xFF as the broadcast address.

  Uses readString() from Stream class to read payload. The Stream class'
  timeout may affect other functuons, like the radio's callback. For an

  created 28 April 2017
  by Tom Igoe
*/
#include <SPI.h>              // include libraries
#include <LoRa.h>

//define the pins used by the transceiver module
#define ss 4
#define rst 14
#define dio0 2

String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0xFE;     // address of this device (0xFE -> 254)
byte destination = 0xFD;      // destination to send to (0xFD -> 253)
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

String incomingMsg = "";

void setup() {
  Serial.begin(115200);                   // initialize serial
  while (!Serial);

  Serial.println("LoRa Duplex");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(ss, rst, dio0); 

  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
//  while (!LoRa.begin(866E6)) {
  while(!LoRa.begin(866E6)){
    Serial.println(".");
    delay(500);
  }

  Serial.println("LoRa init succeeded.");
  Serial.println("Awaiting for incomming messages...");
}

void loop() {
  onReceive(LoRa.parsePacket());
}

void sendMessage(String outgoing) {
  Serial.print("Sending data further to next receiver: ");
  Serial.println("\"" + outgoing + "\"");
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

    while (LoRa.available()) {
      Serial.println("");
      int receviedFromAdd = LoRa.read();           // get the transmitter address from received message
      byte receiverAddress = receviedFromAdd;       // convert to byte
      Serial.print("Recevied data from address: ");
      Serial.println(receiverAddress);

      if(receiverAddress == localAddress){          //Received data from known transmitter
        Serial.println("Received from intended transmitter!");
        byte sender = LoRa.read();            // sender address
        byte incomingMsgId = LoRa.read();     // incoming msg ID
        byte incomingLength = LoRa.read();    // incoming msg length   
        String LoRaData = LoRa.readString();  // incoming msg body (actual message)

        Serial.print("Sender-");
        Serial.print(sender);
        Serial.print(", ");
        Serial.print("Msg ID-");
        Serial.print(incomingMsgId);
        Serial.print(", ");
        Serial.print("Msg length-");
        Serial.print(incomingLength);
        Serial.print(", ");
        Serial.print("Msg body-");
        Serial.println("\"" + LoRaData + "\"");
        
        sendMessage(LoRaData + " + Hello from LoRa device 2."); //Take data from Tx and add some more data and transmit futher to next receiver

      } else {
        Serial.println("Recevied random message, ignoring.");
      }
    }



  // read packet header bytes:
  // int recipient = LoRa.read();          // recipient address
  // byte sender = LoRa.read();            // sender address
  // byte incomingMsgId = LoRa.read();     // incoming msg ID
  // byte incomingLength = LoRa.read();    // incoming msg length

  // String incoming = "";

  // while (LoRa.available()) {
  //   incoming += (char)LoRa.read();
  // }

  // if (incomingLength != incoming.length()) {   // check length for error
  //   Serial.println("error: message length does not match length");
  //   return;                             // skip rest of function
  // }

  // // if the recipient isn't this device or broadcast,
  // if (recipient != localAddress && recipient != 0xFF) {
  //   Serial.println("This message is not for me.");
  //   return;                             // skip rest of function
  // }

  // // if message is for this device, or broadcast, print details:
  // Serial.println("Received from: 0x" + String(sender, HEX));
  // Serial.println("Sent to: 0x" + String(recipient, HEX));
  // Serial.println("Message ID: " + String(incomingMsgId));
  // Serial.println("Message length: " + String(incomingLength));
  // Serial.println("Message: " + incoming);
  // Serial.println("RSSI: " + String(LoRa.packetRssi()));
  // Serial.println("Snr: " + String(LoRa.packetSnr()));
  // Serial.println();
}
