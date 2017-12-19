#include <ArduinoBearSSL.h>
#include <utility/ECC508.h>
#include <utility/ECC508CSR.h>
#include <utility/ECC508TLSConfig.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!ECC508.begin()) {
    Serial.println("No ECC508 present!");
    while (1);
  }

  if (!ECC508.locked()) {
    String lock = promptAndReadLine("The ECC508 on your board is not locked, would you like to configure and lock it now? (y/N): ");

    if (!lock.startsWith("y")) {
      Serial.println("Unfortunately you can't proceed without locking it :(");
      while (1);
    }

    if (!ECC508.writeConfiguration(DEFAULT_ECC508_TLS_CONFIG)) {
      Serial.println("Writing ECC508 configuration failed!");
      while (1);
    }

    if (!ECC508.lock()) {
      Serial.println("Locking ECC508 configuration failed!");
      while (1);
    }

    Serial.println("ECC508 locked successfully");
    Serial.println();
  }

  Serial.println("Hi there, in order to generate a new CSR for your board, we'll need the following information ...");
  Serial.println();

  String country            = promptAndReadLine("Country Name (2 letter code): ");
  String stateOrProvince    = promptAndReadLine("State or Province Name (full name): ");
  String locality           = promptAndReadLine("Locality Name (eg, city): ");
  String organization       = promptAndReadLine("Organization Name (eg, company): ");
  String organizationalUnit = promptAndReadLine("Organizational Unit Name (eg, section): ");
  String common             = promptAndReadLine("Common Name (e.g. server FQDN or YOUR name): ");
  String slot               = promptAndReadLine("What ECC508 slots would you like to use? (0 - 4): ");
  String generateNewKey     = promptAndReadLine("Would you like to generate a new private key? (y/N): ");

  Serial.println();

  generateNewKey.toLowerCase();

  if (!ECC508CSR.begin(slot.toInt(), generateNewKey.startsWith("y"))) {
    Serial.println("Error starting CSR generation!");
    while (1);
  }

  ECC508CSR.setCountryName(country);
  ECC508CSR.setStateProvinceName(stateOrProvince);
  ECC508CSR.setLocalityName(locality);
  ECC508CSR.setOrganizationName(organization);
  ECC508CSR.setOrganizationalUnitName(organizationalUnit);
  ECC508CSR.setCommonName(common);

  String csr = ECC508CSR.end();

  if (!csr) {
    Serial.println("Error generating CSR!");
    while (1);
  }

  Serial.println("Here's your CSR, enjoy!");
  Serial.println();
  Serial.println(csr);
}

void loop() {
  // do nothing
}


String promptAndReadLine(const char* prompt) {
  Serial.print(prompt);
  String s = readLine();
  Serial.println(s);

  return s;
}

String readLine() {
  String line;

  while (1) {
    if (Serial.available()) {
      char c = Serial.read();

      if (c == '\r') {
        // ignore
      } else if (c == '\n') {
        break;
      }

      line += c;
    }
  }

  return line;
}
