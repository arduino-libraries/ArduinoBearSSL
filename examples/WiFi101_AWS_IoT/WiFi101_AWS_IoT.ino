// This example uses an Arduino/Genuino Zero together with
// a WiFi101 Shield or a MKR1000 to connect to AWS IOT.
//
// IMPORTANT: This example uses the new WiFi101 library.
//
// IMPORTANT: You need to install/update the SSL certificates first:
// https://github.com/arduino-libraries/WiFi101-FirmwareUpdater#to-update-ssl-certificates
//
//
// by Gilberto Conti
// https://github.com/256dpi/arduino-mqtt

#include <WiFi101.h>
#include <MQTTClient.h>
#include <ArduinoBearSSL.h>

// ssid and pass are the wifi settings
const char ssid[] = "ssid";
const char pass[] = "pass";

// server is the url of aws IOT
const char server[] = "xxxxxxxxxxxxxx.iot.xx-xxxx-x.amazonaws.com";

// id is the ThingName in aws IOT
const char id[] = "XXX"

// Get the cert data by:
// 1) Creating a CSR using the ArduinoBearSSL -> Tools -> ECC508CSR example for key slot 0
// 2) Creating a new thing and uploading the CSR for it
// 3) Downloading the public key for the thing in AWS IoT
// 4) Convert the base64 encoded cert to binary
const byte cert[] = {
  // ...
};

WiFiClient wifiClient;
BearSSLClient net(wifiClient, 0, cert, sizeof(cert));
MQTTClient client;

unsigned long lastMillis = 0;

unsigned long getTime() {
  return WiFi.getTime();
}

void setup() {
  Serial.begin(115200);

  ArduinoBearSSL.onGetTime(getTime);
  
  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
  // You need to set the IP address directly.
  //
  // MQTT brokers usually use port 8883 for secure connections.
  client.begin(server, 8883, net);
  client.onMessage(messageReceived);

  connect();
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect(id)) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("$aws/things/" + id + "/test");
}

void loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish("$aws/things/" + id + "/status", "hello world");
  }
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}
