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

#include "MD5.h"

MD5Class::MD5Class() :
  SHAClass(MD5_BLOCK_SIZE, MD5_DIGEST_SIZE)
{
}

MD5Class::~MD5Class()
{
}

int MD5Class::begin()
{
  br_md5_init(&_ctx);

  return 1;
}

int MD5Class::update(const uint8_t *buffer, size_t size)
{
  br_md5_update(&_ctx, buffer, size);

  return 1;
}

int MD5Class::end(uint8_t *digest)
{
  br_md5_out(&_ctx, digest);

  return 1;
}

#ifndef ARDUINO_ARCH_MEGAAVR
MD5Class MD5;
#endif