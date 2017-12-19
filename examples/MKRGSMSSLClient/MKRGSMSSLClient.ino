/*
  This example creates a client object that connects and transfers
  data using always SSL.

  It is compatible with the methods normally related to plain
  connections, like client.connect(host, port).

  Written by Arturo Guadalupi
  last revision November 2015

*/

#include <MKRGSM.h>
#include <ArduinoBearSSL.h>

const char pin[] = "";
const char apn[] = "apn";
const char login[] = "login";
const char password[] = "pass";

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "www.google.com";    // name address for Google (using DNS)

GPRS gprs;
GSM gsmAccess;

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
GSMClient client;
BearSSLClient sslClient(client);

unsigned long getTime() {
  return gsmAccess.getTime();
}

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

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

  Serial.println("Connected to GPRS");

  ArduinoBearSSL.onGetTime(getTime);

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (sslClient.connect(server, 443)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    sslClient.println("GET /search?q=arduino HTTP/1.1");
    sslClient.println("Host: www.google.com");
    sslClient.println("Connection: close");
    sslClient.println();
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (sslClient.available()) {
    char c = sslClient.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (!sslClient.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    sslClient.stop();

    // do nothing forevermore:
    while (true);
  }
}

