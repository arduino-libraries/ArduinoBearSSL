/*
  Copyright (C) Arduino s.r.l. and/or its affiliated companies
  SPDX-License-Identifier: MPL-2.0
*/

#include "BSSLSecureElement.h"


#if defined(BSSLSECURE_ELEMENT_IS_SE050)
BSSLSecureElementClass &BSSLSecureElement(SE05X);
#elif defined(BSSLSECURE_ELEMENT_IS_ECCX08)
BSSLSecureElementClass &BSSLSecureElement(ECCX08);
#endif
