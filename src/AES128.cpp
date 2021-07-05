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

#include "AES128.h"

AES128Class::AES128Class() :
  EncryptionClass(AES128_BLOCK_SIZE, AES128_DIGEST_SIZE)
{
}

AES128Class::~AES128Class()
{
}

int AES128Class::runEncryption(uint8_t *key, size_t size, uint8_t *input, size_t block_size, uint8_t *iv)
{
  br_aes_ct_cbcenc_init(&cbcenc_ctx, key, size);
  br_aes_ct_cbcenc_run(&cbcenc_ctx, iv, input, block_size); // block_size must be multiple of 16

  return 1;
}

int AES128Class::runDecryption(uint8_t *key, size_t size, uint8_t *input, size_t block_size, uint8_t *iv)
{
  br_aes_ct_cbcdec_init(&cbcdec_ctx, key, size);
  br_aes_ct_cbcdec_run(&cbcdec_ctx, iv, input, block_size); // block_size must be multiple of 16

  return 1;
}

#ifndef ARDUINO_ARCH_MEGAAVR
AES128Class AES128;
#endif