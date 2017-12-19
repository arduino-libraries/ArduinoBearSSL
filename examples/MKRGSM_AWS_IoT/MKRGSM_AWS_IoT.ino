// This example uses an Arduino/Genuino Zero together with
// a WiFi101 Shield or a MKR1000 to connect to shiftr.io.
//
// IMPORTANT: This example uses the new WiFi101 library.
//
// IMPORTANT: You need to install/update the SSL certificates first:
// https://github.com/arduino-libraries/WiFi101-FirmwareUpdater#to-update-ssl-certificates
//
// You can check on your device after a successful
// connection here: https://shiftr.io/try.
//
// by Gilberto Conti
// https://github.com/256dpi/arduino-mqtt

#include <MKRGSM.h>
#include <MQTTClient.h>
#include <ArduinoBearSSL.h>

const char pin[] = "";
const char apn[] = "apn";
const char login[] = "login";
const char password[] = "pass";

const char server[] = "xxxxxxxxxxxxxx.iot.xx-xxxx-x.amazonaws.com";

// Get the cert data by:
// 1) Creating a CSR using the ArduinoBearSSL -> Tools -> ECC508CSR example for key slot 0
// 2) Creating a new thing and uploading the CSR for it
// 3) Downloading the public key for the thing in AWS IoT
// 4) Convert the base64 encoded cert to binary
const byte cert[] = {
  // ...
};

GPRS gprs;
GSM gsmAccess;

GSMClient gsmClient;
BearSSLClient net(gsmClient, 0, cert, sizeof(cert));
MQTTClient client;

unsigned long lastMillis = 0;

unsigned long getTime() {
  return gsmAccess.getTime();
}

void setup() {
  Serial.begin(115200);
  
  // connection state
  boolean notConnected = true;

  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (notConnected) {
    if ((gsmAccess.begin(pin) == GSM_READY) &
        (gprs.attachGPRS(apn, login, password) == GPRS_READY)) {
      notConnected = false;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("GPRS connected");

  ArduinoBearSSL.onGetTime(getTime);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported by Arduino.
  // You need to set the IP address directly.
  //
  // MQTT brokers usually use port 8883 for secure connections.
  client.begin(server, 8883, net);
  client.onMessage(messageReceived);

  connect();
}

void connect() {
  Serial.print("\nconnecting...");
  while (!client.connect("arduino")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/hello");
  // client.unsubscribe("/hello");
}

void loop() {
  client.loop();

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish("/hello", "world");
  }
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}
