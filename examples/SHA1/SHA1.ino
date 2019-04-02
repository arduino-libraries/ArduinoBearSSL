/*
  ArduinoBearSSL SHA1

  This sketch demonstrates how to create a SHA1 hash and HMAC
  for an input string.

  This example code is in the public domain.
*/

#include <ArduinoBearSSL.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // expect 0xDA39A3EE5E6B4B0D3255BFEF95601890AFD80709
  printSHA1("");

  // expect 0x2FD4E1C67A2D28FCED849EE1BB76E7391B93EB12
  printSHA1("The quick brown fox jumps over the lazy dog");

  // expect 0xFBDB1D1B18AA6C08324B7D64B71FB76370690E1D
  printHMACSHA1("", "");

  // expect 0xDE7C9B85B8B78AA6BC8A7A36F70A90701C9DB4D9
  printHMACSHA1("key", "The quick brown fox jumps over the lazy dog");
}

void loop() {
}

void printSHA1(const char* str) {
  Serial.print("SHA1 of '");
  Serial.print(str);
  Serial.print("' is 0x");

  SHA1.beginHash();
  SHA1.print(str);
  SHA1.endHash();

  printResult();
}

void printHMACSHA1(const char* secret, const char* str) {
  Serial.print("HMAC-SHA1 of '");
  Serial.print(str);
  Serial.print("' with secret '");
  Serial.print(secret);
  Serial.print("' is 0x");

  SHA1.beginHmac(secret);
  SHA1.print(str);
  SHA1.endHmac();

  printResult();
}

void printResult()
{
  while (SHA1.available()) {
    byte b = SHA1.read();

    if (b < 16) {
      Serial.print("0");
    }

    Serial.print(b, HEX);
  }
  Serial.println();
}
