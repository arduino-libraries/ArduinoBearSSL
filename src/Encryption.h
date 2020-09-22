/*
 * Copyright (c) 2019 Arduino SA. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining 
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <Arduino.h>

class EncryptionClass {

public:
  EncryptionClass(int blockSize, int digestSize);
  virtual ~EncryptionClass();

  int runEnc(uint8_t *_secret, size_t _secretLength, uint8_t *_data, size_t _dataLength, uint8_t *_ivector);
  int runDec(uint8_t *_secret, size_t _secretLength, uint8_t *_data, size_t _dataLength, uint8_t *_ivector);

protected:
  virtual int runEncryption(uint8_t *key, size_t size, uint8_t *input, size_t block_size, uint8_t *iv) = 0;
  virtual int runDecryption(uint8_t *key, size_t size, uint8_t *input, size_t block_size, uint8_t *iv) = 0;

private:
  int _blockSize;
  int _digestSize;

  uint8_t* _secret;
  int _secretLength;
  uint8_t* _ivector;
  int _ivectorLength;
  uint8_t* _data;
  int _dataLength;
};

#endif
