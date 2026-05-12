/*
  Copyright (C) Arduino s.r.l. and/or its affiliated companies
  SPDX-License-Identifier: MPL-2.0
*/

#if defined(ARDUINO_AVR_UNO_WIFI_REV2) ||\
  defined(ARDUINO_SAMD_MKRWAN1300)  || defined(ARDUINO_SAMD_MKRWAN1310) ||\
  defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) ||\
  defined(ARDUINO_SAMD_MKRGSM1400)  || defined(ARDUINO_SAMD_MKR1000) ||\
  defined(ARDUINO_SAMD_MKRNB1500)   || defined(ARDUINO_PORTENTA_H7_M7) ||\
  defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_OPTA) ||\
  defined(ARDUINO_GIGA)
  #define BSSLSECURE_ELEMENT_IS_ECCX08
#endif

#if (defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_PORTENTA_C33)) &&\
  !defined(ARDUINO_ARCH_ZEPHYR)
  #define BSSLSECURE_ELEMENT_IS_SE050
#endif

#if defined(BSSLSECURE_ELEMENT_IS_SE050)
#include <SE05X.h>
using BSSLSecureElementClass = SE05XClass;
extern BSSLSecureElementClass &BSSLSecureElement;
#elif defined(BSSLSECURE_ELEMENT_IS_ECCX08)
#include <ECCX08.h>
using BSSLSecureElementClass = ECCX08Class;
extern BSSLSecureElementClass &BSSLSecureElement;
#endif
