/*
  ArduinoCrypto AES128 Example

  This sketch demonstrates how to run AES128 encryption and decryption for an input string.

  Circuit:
  - Nano 33 IoT board

  created 13 July 2020
  by Luigi Gubello

  This example code is in the public domain.
*/

#include <ArduinoBearSSL.h>
#include "AES128.h"

#ifdef ARDUINO_ARCH_MEGAAVR
// Create the object
AES128Class AES128;
#endif

uint8_t key[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02};
uint8_t enc_iv[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01};
uint8_t dec_iv[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01};
uint8_t input[16] = "ArduinoArduino"; // {0x41,0x72,0x64,0x75,0x69,0x6E,0x6F,0x41,0x72,0x64,0x75,0x69,0x6E,0x6F,0x00,0x00}

void setup() {
  Serial.begin(9600);
  while (!Serial);
}

void loop() {

  Serial.print("Key: ");
  printHex(key, 16);
  Serial.println(" ");
  Serial.print("IV: ");
  printHex(enc_iv, 16);
  Serial.println(" ");
  Serial.print("AES128 Encryption of '");
  printHex(input, 16);
  Serial.print("' is 0x");
  AES128.runEnc(key, 16, input, 16, enc_iv);   // expect 0x65D0F7758B094114AFA6D33A5EA0716A
  printHex(input, 16);
  Serial.println(" ");
  Serial.println(" ");
  Serial.print("Key: ");
  printHex(key, 16);
  Serial.println(" ");
  Serial.print("IV: ");
  printHex(dec_iv, 16);
  Serial.println(" ");
  Serial.print("AES128 Decryption of '");
  printHex(input, 16);
  Serial.print("' is 0x");
  AES128.runDec(key, 16, input, 16, dec_iv);
  printHex(input, 16);
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
