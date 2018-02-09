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
#include <utility/ECC508.h>
#include <utility/ECC508Cert.h>

// ssid and pass are the wifi settings
const char ssid[] = "XXX";
const char pass[] = "XXX";

// server is the url of aws IOT
const char server[] = "xxxxxxxxxxxxxx.iot.xx-xxxx-x.amazonaws.com";

// id is the ThingName in aws IOT
const String id = "XXX";

// Get the cert data by:
// 1) Creating a CSR using the ArduinoBearSSL -> Tools -> ECC508CSR example for key slot 0
// 2) Use the "Go tool" to generate a public cert from the CSR
// 3) Store the cert params in 1)
// 4) Activate the cert in AWS IoT and attach policy + thing

WiFiClient wifiClient;
BearSSLClient net(wifiClient);
MQTTClient client;

unsigned long lastMillis = 0;

unsigned long getTime() {
  return WiFi.getTime();
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!ECC508.begin()) {
    Serial.println("No ECC508 present!");
    while (1);
  }

  ArduinoBearSSL.onGetTime(getTime);

  ECC508Cert.begin(0, 9, 10);
  ECC508Cert.setIssuerCountryName("US");
  ECC508Cert.setIssuerOrganizationName("Arduino LLC US");
  ECC508Cert.setIssuerOrganizationalUnitName("IT");
  ECC508Cert.setIssuerCommonName("Arduino");
  ECC508Cert.setSubjectCommonName(ECC508.serialNumber());
  ECC508Cert.uncompress();

  const byte* certData = ECC508Cert.bytes();
  int certLength = ECC508Cert.length();

  for (int i = 0; i < certLength; i++) {
    byte b = certData[i];

    if (b < 16) {
      Serial.print('0');
    }
    Serial.print(b, HEX);
  }
  Serial.println();

  net.setEccSlot(0, ECC508Cert.bytes(), ECC508Cert.length());
  
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
  while (!client.connect(id.c_str())) {
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
