/*
 * Copyright (c) 2018 Arduino SA. All rights reserved.
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

#ifndef _ARDUINO_BEAR_SSL_H_
#define _ARDUINO_BEAR_SSL_H_

#if ARDUINO_BEAR_SSL_OPTIMIZE_FOR_ARDUINO_CLOUD
  #define BEAR_SSL_CLIENT_OPTIMIZE_FOR_ARDUINO_CLOUD (1)
#else
  #define BEAR_SSL_CLIENT_OPTIMIZE_FOR_ARDUINO_CLOUD (0)
#endif

#include "BearSSLClient.h"
#include "SHA1.h"
#include "SHA256.h"

class ArduinoBearSSLClass {
public:
  ArduinoBearSSLClass();
  virtual ~ArduinoBearSSLClass();

  unsigned long getTime();
  void onGetTime(unsigned long(*)(void));

private:
  unsigned long (*_onGetTimeCallback)(void);
};

extern ArduinoBearSSLClass ArduinoBearSSL;

#endif
