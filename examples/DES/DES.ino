/*
  ArduinoCrypto DES Example

  This sketch demonstrates how to run DES encryption and decryption for an input string.

  Circuit:
  - Nano 33 IoT board

  created 13 July 2020
  by Luigi Gubello

  This example code is in the public domain.
*/

#include <ArduinoBearSSL.h>
#include "DES.h"

#ifdef ARDUINO_ARCH_MEGAAVR
// Create the object
DESClass DES;
#endif

uint8_t key[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02};
uint8_t enc_iv[8] = {0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01};
uint8_t dec_iv[8] = {0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01};
uint8_t input[8] = "Arduino"; // {0x41,0x72,0x64,0x75,0x69,0x6E,0x6F,0x00}

void setup() {
  Serial.begin(9600);
  while (!Serial);
}

void loop() {

  Serial.print("Key: ");
  printHex(key, 8);
  Serial.println(" ");
  Serial.print("IV: ");
  printHex(enc_iv, 8);
  Serial.println(" ");
  Serial.print("DES Encryption of '");
  printHex(input, 8);
  Serial.print("' is 0x");
  DES.runEnc(key, 8, input, 8, enc_iv);   // expect 0x3C21EB6A62D372DB
  printHex(input, 8);
  Serial.println(" ");
  Serial.println(" ");
  Serial.print("Key: ");
  printHex(key, 8);
  Serial.println(" ");
  Serial.print("IV: ");
  printHex(dec_iv, 8);
  Serial.println(" ");
  Serial.print("DES Decryption of '");
  printHex(input, 8);
  Serial.print("' is 0x");
  DES.runDec(key, 8, input, 8, dec_iv);
  printHex(input, 8);
  Serial.println(" ");
  while (1);
}

void printHex(uint8_t *text, size_t size) {
  for (byte i = 0; i < size; i = i + 1) {
    if (text[i] < 16) {
      Serial.print("0");
    }
    Serial.print(text[i], HEX);
  }
}
