/*
  This example creates a client object that connects and transfers
  data using always TLS but with Server Name Indication check disabled.

  WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
  WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
  WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
  WARNING                                                                         WARNING 
  WARNING This can be useful when testing initial server configurations but makes WARNING 
  WARNING it possible for a malicious third party to impersonate your server      WARNING 
  WARNING through DNS spoofing, for example.                                      WARNING 
  WARNING                                                                         WARNING 
  WARNING                  USE THIS OPTION IN TESTING ONLY                        WARNING 
  WARNING                                                                         WARNING 
  WARNING If you need to resort to using this option in a production environment, WARNING 
  WARNING your setup is at fault and there is no point using encryption.          WARNING  
  WARNING                                                                         WARNING 
  WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
  WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
  WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 

  It is compatible with the methods normally related to plain
  connections, like client.connect(host, port).

  Written by Arturo Guadalupi
  Modified by Giampaolo Mancini
  last revision May 2020

*/

#include <SPI.h>
#include <WiFi101.h>
#include <ArduinoBearSSL.h>

char ssid[] = "yourNetwork"; //  your network SSID (name)
char pass[] = "secretPassword";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(192,168,1,1);  // numeric IP for testing server (no DNS)
char server[] = "192.168.1.1";    // name address for testing server (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;
BearSSLClient sslClient(client);

unsigned long getTime() {
  return WiFi.getTime();
}

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWiFiStatus();

  ArduinoBearSSL.onGetTime(getTime);

  // Disable Server Name Indication:
  // for testing purposes only
  // DO NOT USE IN PRODUCTION
  sslClient.setInsecure(BearSSLClient::SNI::Insecure);

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


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
