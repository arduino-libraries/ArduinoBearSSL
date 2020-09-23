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

#include "Encryption.h"

EncryptionClass::EncryptionClass(int blockSize, int digestSize) :
  _blockSize(blockSize),
  _digestSize(digestSize)
{
  _data = (uint8_t*)malloc(_digestSize);
  _secret = (uint8_t*)malloc(_blockSize);
  _ivector = (uint8_t*)malloc(_blockSize);
}

EncryptionClass::~EncryptionClass()
{
  if (_secret) {
    free(_secret);
    _secret = NULL;
  }

  if (_data) {
    free(_data);
    _data = NULL;
  }

  if (_ivector) {
    free(_ivector);
    _ivector = NULL;
  }
}

int EncryptionClass::runEnc(uint8_t *_secret, size_t _secretLength, uint8_t *_data, size_t _dataLength, uint8_t *_ivector)
{
  return runEncryption(_secret, _secretLength, _data, _dataLength, _ivector);
}

int EncryptionClass::runDec(uint8_t *_secret, size_t _secretLength, uint8_t *_data, size_t _dataLength, uint8_t *_ivector)
{
  return runDecryption(_secret, _secretLength, _data, _dataLength, _ivector);
}