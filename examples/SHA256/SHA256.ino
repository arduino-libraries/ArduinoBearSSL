/*
  ArduinoBearSSL SHA256

  This sketch demonstrates how to create a SHA256 hash and HMAC
  for an input string.

  This example code is in the public domain.
*/

#include <ArduinoBearSSL.h>
#include "SHA256.h"

#ifdef ARDUINO_ARCH_MEGAAVR
// Create the object
SHA256Class SHA256;
#endif

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // expect 0xE3B0C44298FC1C149AFBF4C8996FB92427AE41E4649B934CA495991B7852B855
  printSHA256("");

  // expect 0xD7A8FBB307D7809469CA9ABCB0082E4F8D5651E46D3CDB762D02D0BF37C9E592
  printSHA256("The quick brown fox jumps over the lazy dog");

  // expect 0xB613679A0814D9EC772F95D778C35FC5FF1697C493715653C6C712144292C5AD
  printHMACSHA256("", "");

  // expect 0xF7BC83F430538424B13298E6AA6FB143EF4D59A14946175997479DBC2D1A3CD8
  printHMACSHA256("key", "The quick brown fox jumps over the lazy dog");
}

void loop() {
}

void printSHA256(const char* str) {
  Serial.print("SHA256 of '");
  Serial.print(str);
  Serial.print("' is 0x");

  SHA256.beginHash();
  SHA256.print(str);
  SHA256.endHash();

  printResult();
}

void printHMACSHA256(const char* secret, const char* str) {
  Serial.print("HMAC-SHA256 of '");
  Serial.print(str);
  Serial.print("' with secret '");
  Serial.print(secret);
  Serial.print("' is 0x");

  SHA256.beginHmac(secret);
  SHA256.print(str);
  SHA256.endHmac();

  printResult();
}

void printResult()
{
  while (SHA256.available()) {
    byte b = SHA256.read();

    if (b < 16) {
      Serial.print("0");
    }

    Serial.print(b, HEX);
  }
  Serial.println();
}
