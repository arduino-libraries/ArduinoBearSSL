/*
  ArduinoBearSSL MD5

  This sketch demonstrates how to create a MD5 hash
  for an input string.

  This example code is in the public domain.
*/

#include <ArduinoBearSSL.h>
#include "MD5.h"

#ifdef ARDUINO_ARCH_MEGAAVR
// Create the object
MD5Class MD5;
#endif

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // expect 9e107d9d372bb6826bd81d3542a419d6
  printMD5("The quick brown fox jumps over the lazy dog");

  // expect 80070713463e7749b90c2dc24911e275
  printHMACMD5("key", "The quick brown fox jumps over the lazy dog");
}

void loop() {
}

void printMD5(const char* str) {
  Serial.print("MD5 of '");
  Serial.print(str);
  Serial.print("' is 0x");

  MD5.beginHash();
  MD5.print(str);
  MD5.endHash();

  printResult();
}

void printHMACMD5(const char* secret, const char* str) {
  Serial.print("HMAC-MD5 of '");
  Serial.print(str);
  Serial.print("' with secret '");
  Serial.print(secret);
  Serial.print("' is 0x");

  MD5.beginHmac(secret);
  MD5.print(str);
  MD5.endHmac();

  printResult();
}

void printResult()
{
  while (MD5.available()) {
    byte b = MD5.read();

    if (b < 16) {
      Serial.print("0");
    }

    Serial.print(b, HEX);
  }
  Serial.println();
}
