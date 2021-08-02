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

#include "ArduinoBearSSL.h"

#ifndef ARDUINO_DISABLE_ECCX08
#include <ArduinoECCX08.h>
#endif

#include "BearSSLTrustAnchors.h"
#include "utility/eccX08_asn1.h"

#include "BearSSLClient.h"

BearSSLClient::BearSSLClient(Client& client) :
  BearSSLClient(&client, TAs, TAs_NUM)
{
}

BearSSLClient::BearSSLClient(Client& client, const br_x509_trust_anchor* myTAs, int myNumTAs)
: BearSSLClient(&client, myTAs, myNumTAs)
{
}

BearSSLClient::BearSSLClient(Client* client, const br_x509_trust_anchor* myTAs, int myNumTAs) :
  _client(client),
  _TAs(myTAs),
  _numTAs(myNumTAs),
  _noSNI(false),
  _skeyDecoder(NULL),
  _ecChainLen(0)
{
#ifndef ARDUINO_DISABLE_ECCX08
  _ecVrfy = eccX08_vrfy_asn1;
  _ecSign = eccX08_sign_asn1;
#else
  _ecVrfy = br_ecdsa_vrfy_asn1_get_default();
  _ecSign = br_ecdsa_sign_asn1_get_default();
#endif

  _ecKey.curve = 0;
  _ecKey.x = NULL;
  _ecKey.xlen = 0;

  for (size_t i = 0; i < BEAR_SSL_CLIENT_CHAIN_SIZE; i++) {
    _ecCert[i].data = NULL;
    _ecCert[i].data_len = 0;
  }
  _ecCertDynamic = false;
}

BearSSLClient::~BearSSLClient()
{
  if (_ecCertDynamic && _ecCert[0].data) {
    free(_ecCert[0].data);
    _ecCert[0].data = NULL;
  }

  if (_skeyDecoder) {
    free(_skeyDecoder);
    _skeyDecoder = NULL;
  }
}

int BearSSLClient::connect(IPAddress ip, uint16_t port)
{
  if (!_client->connect(ip, port)) {
    return 0;
  }

  return connectSSL(NULL);
}

int BearSSLClient::connect(const char* host, uint16_t port)
{
  if (!_client->connect(host, port)) {
    return 0;
  }

  return connectSSL(_noSNI ? NULL : host);
}

size_t BearSSLClient::write(uint8_t b)
{
  return write(&b, sizeof(b));
}

size_t BearSSLClient::write(const uint8_t *buf, size_t size)
{
  size_t written = 0;

  while (written < size) {
    int result = br_sslio_write(&_ioc, buf, size);

    if (result < 0) {
      break;
    }

    buf += result;
    written += result;
  }

  if (written == size && br_sslio_flush(&_ioc) < 0) {
    return 0;
  }

  return written;
}

int BearSSLClient::available()
{
  int available = br_sslio_read_available(&_ioc);

  if (available < 0) {
    available = 0;
  }

  return available;
}

int BearSSLClient::read()
{
  byte b;

  if (read(&b, sizeof(b)) == sizeof(b)) {
    return b;
  }

  return -1;
}

int BearSSLClient::read(uint8_t *buf, size_t size)
{
  return br_sslio_read(&_ioc, buf, size);
}

int BearSSLClient::peek()
{
  byte b;

  if (br_sslio_peek(&_ioc, &b, sizeof(b)) == sizeof(b)) {
    return b;
  }

  return -1;
}

void BearSSLClient::flush()
{
  br_sslio_flush(&_ioc);

  _client->flush();
}

void BearSSLClient::stop()
{
  if (_client->connected()) {
    if ((br_ssl_engine_current_state(&_sc.eng) & BR_SSL_CLOSED) == 0) {
      br_sslio_close(&_ioc);
    }

    _client->stop();
  }
}

uint8_t BearSSLClient::connected()
{
  if (!_client->connected()) {
    return 0;
  }

  unsigned state = br_ssl_engine_current_state(&_sc.eng);

  if (state == BR_SSL_CLOSED) {
    return 0;
  }

  return 1;
}

BearSSLClient::operator bool()
{
  return (*_client);  
}

void BearSSLClient::setInsecure(SNI insecure)
{
  switch (insecure) {
    case SNI::Insecure : _noSNI = true; break;
    default: _noSNI = false;
  }
}

void BearSSLClient::setEccVrfy(br_ecdsa_vrfy vrfy)
{
  _ecVrfy = vrfy;
}

void BearSSLClient::setEccSign(br_ecdsa_sign sign)
{
  _ecSign = sign;
}

void BearSSLClient::setEccCert(br_x509_certificate cert)
{
  _ecCert[0] = cert;
  _ecChainLen = 1;
}

void BearSSLClient::setEccChain(br_x509_certificate* chain, size_t chainLen)
{
  if (chainLen > BEAR_SSL_CLIENT_CHAIN_SIZE)
    return;

  for (size_t i = 0; i < chainLen; i++) {
    _ecCert[i] = chain[i];
  }
  _ecChainLen = chainLen;
}

void BearSSLClient::setEccSlot(int ecc508KeySlot, const byte cert[], int certLength)
{
  // HACK: put the key slot info. in the br_ec_private_key structure
  _ecKey.curve = 23;
  _ecKey.x = (unsigned char*)ecc508KeySlot;
  _ecKey.xlen = 32;

  _ecCert[0].data = (unsigned char*)cert;
  _ecCert[0].data_len = certLength;
  _ecChainLen = 1;
  _ecCertDynamic = false;

#ifndef ARDUINO_DISABLE_ECCX08
  _ecVrfy = eccX08_vrfy_asn1;
  _ecSign = eccX08_sign_asn1;
#else
  _ecVrfy = br_ecdsa_vrfy_asn1_get_default();
  _ecSign = br_ecdsa_sign_asn1_get_default();
#endif
}

void BearSSLClient::setEccSlot(int ecc508KeySlot, const char cert[])
{
  // try to decode the cert
  br_pem_decoder_context pemDecoder;

  size_t certLen = strlen(cert);

  // free old data
  if (_ecCertDynamic && _ecCert[0].data) {
    free(_ecCert[0].data);
    _ecCert[0].data = NULL;
  }

  // assume the decoded cert is 3/4 the length of the input
  _ecCert[0].data = (unsigned char*)malloc(((certLen * 3) + 3) / 4);
  _ecCert[0].data_len = 0;
  _ecChainLen = 1;

  br_pem_decoder_init(&pemDecoder);

  while (certLen) {
    size_t len = br_pem_decoder_push(&pemDecoder, cert, certLen);

    cert += len;
    certLen -= len;

    switch (br_pem_decoder_event(&pemDecoder)) {
      case BR_PEM_BEGIN_OBJ:
        br_pem_decoder_setdest(&pemDecoder, BearSSLClient::clientAppendCert, this);
        break;

      case BR_PEM_END_OBJ:
        if (_ecCert[0].data_len) {
          // done
          setEccSlot(ecc508KeySlot, _ecCert[0].data, _ecCert[0].data_len);
          _ecCertDynamic = true;
          return;
        }
        break;

      case BR_PEM_ERROR:
        // failure
        free(_ecCert[0].data);
        setEccSlot(ecc508KeySlot, NULL, 0);
        return;
    }
  }
}

void BearSSLClient::setKey(const char key[], const char cert[])
{
  // try to decode the key and cert
  br_pem_decoder_context pemDecoder;

  size_t keyLen = strlen(key);
  size_t certLen = strlen(cert);

  br_pem_decoder_init(&pemDecoder);

  if (_skeyDecoder == NULL) {
    _skeyDecoder = (br_skey_decoder_context*)malloc(sizeof(br_skey_decoder_context));
  }

  br_skey_decoder_init(_skeyDecoder);

  while (keyLen) {
    size_t len = br_pem_decoder_push(&pemDecoder, key, keyLen);

    key += len;
    keyLen -= len;

    switch (br_pem_decoder_event(&pemDecoder)) {
      case BR_PEM_BEGIN_OBJ:
        br_pem_decoder_setdest(&pemDecoder, BearSSLClient::clientAppendKey, this);
        break;

      case BR_PEM_END_OBJ:
        if (br_skey_decoder_last_error(_skeyDecoder) != 0) {
          return;
        }
        break;

      case BR_PEM_ERROR:
        return;
    }
  }

  // assume the decoded cert is 3/4 the length of the input
  _ecCert[0].data = (unsigned char*)malloc(((certLen * 3) + 3) / 4);
  _ecCert[0].data_len = 0;
  _ecChainLen = 1;

  br_pem_decoder_init(&pemDecoder);

  while (certLen) {
    size_t len = br_pem_decoder_push(&pemDecoder, cert, certLen);

    cert += len;
    certLen -= len;

    switch (br_pem_decoder_event(&pemDecoder)) {
      case BR_PEM_BEGIN_OBJ:
        br_pem_decoder_setdest(&pemDecoder, BearSSLClient::clientAppendCert, this);
        break;

      case BR_PEM_END_OBJ:
        if (_ecCert[0].data_len) {
          // done
          _ecCertDynamic = true;
          return;
        }
        break;

      case BR_PEM_ERROR:
        // failure
        free(_ecCert[0].data);
        _ecCert[0].data = NULL;
        return;
    }
  }
}

void BearSSLClient::setEccCertParent(const char cert[])
{
  // try to decode the cert
  br_pem_decoder_context pemDecoder;

  size_t certLen = strlen(cert);

  // free old data
  if (_ecCertDynamic && _ecCert[1].data) {
    free(_ecCert[1].data);
    _ecCert[1].data = NULL;
  }

  // assume the decoded cert is 3/4 the length of the input
  _ecCert[1].data = (unsigned char*)malloc(((certLen * 3) + 3) / 4);
  _ecCert[1].data_len = 0;
  _ecChainLen = 2;

  br_pem_decoder_init(&pemDecoder);

  while (certLen) {
    size_t len = br_pem_decoder_push(&pemDecoder, cert, certLen);

    cert += len;
    certLen -= len;

    switch (br_pem_decoder_event(&pemDecoder)) {
      case BR_PEM_BEGIN_OBJ:
        br_pem_decoder_setdest(&pemDecoder, BearSSLClient::parentAppendCert, this);
        break;

      case BR_PEM_END_OBJ:
        if (_ecCert[1].data_len) {
          // done
          _ecCertDynamic = true;
          return;
        }
        break;

      case BR_PEM_ERROR:
        // failure
        free(_ecCert[1].data);
        return;
    }
  }
}

int BearSSLClient::errorCode()
{
  return br_ssl_engine_last_error(&_sc.eng);
}

int BearSSLClient::connectSSL(const char* host)
{
  // initialize client context with all algorithms and hardcoded trust anchors
  br_ssl_client_init_full(&_sc, &_xc, _TAs, _numTAs);

  br_ssl_engine_set_buffers_bidi(&_sc.eng, _ibuf, sizeof(_ibuf), _obuf, sizeof(_obuf));

  // inject entropy in engine
  unsigned char entropy[32];

#ifndef ARDUINO_DISABLE_ECCX08
  if (!ECCX08.begin() || !ECCX08.locked() || !ECCX08.random(entropy, sizeof(entropy))) {
#endif
    // no ECCX08 or random failed, fallback to pseudo random
    for (size_t i = 0; i < sizeof(entropy); i++) {
      entropy[i] = random(0, 255);
    }
#ifndef ARDUINO_DISABLE_ECCX08
  }
#endif
  br_ssl_engine_inject_entropy(&_sc.eng, entropy, sizeof(entropy));

  // add custom ECDSA vfry and EC sign
  br_ssl_engine_set_ecdsa(&_sc.eng, _ecVrfy);
  br_x509_minimal_set_ecdsa(&_xc, br_ssl_engine_get_ec(&_sc.eng), br_ssl_engine_get_ecdsa(&_sc.eng));

  // enable client auth
  if (_ecCert[0].data_len) {
    if (_skeyDecoder) {
      int skeyType = br_skey_decoder_key_type(_skeyDecoder);

      if (skeyType == BR_KEYTYPE_EC) {
        br_ssl_client_set_single_ec(&_sc, _ecCert, _ecChainLen, br_skey_decoder_get_ec(_skeyDecoder), BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN, BR_KEYTYPE_EC, br_ec_get_default(), br_ecdsa_sign_asn1_get_default());
      } else if (skeyType == BR_KEYTYPE_RSA) {
        br_ssl_client_set_single_rsa(&_sc, _ecCert, _ecChainLen, br_skey_decoder_get_rsa(_skeyDecoder), br_rsa_pkcs1_sign_get_default());
      }
    } else {
      br_ssl_client_set_single_ec(&_sc, _ecCert, _ecChainLen, &_ecKey, BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN, BR_KEYTYPE_EC, br_ec_get_default(), _ecSign);
    }
  }

  // set the hostname used for SNI
  br_ssl_client_reset(&_sc, host, 0);

  // get the current time and set it for X.509 validation
  uint32_t now = ArduinoBearSSL.getTime();
  uint32_t days = now / 86400 + 719528;
  uint32_t sec = now % 86400;

  br_x509_minimal_set_time(&_xc, days, sec);

  // use our own socket I/O operations
  br_sslio_init(&_ioc, &_sc.eng, BearSSLClient::clientRead, _client, BearSSLClient::clientWrite, _client);

  br_sslio_flush(&_ioc);

  while (1) {
    unsigned state = br_ssl_engine_current_state(&_sc.eng);

    if (state & BR_SSL_SENDAPP) {
      break;
    } else if (state & BR_SSL_CLOSED) {
      return 0;
    }
  }

  return 1;
}

// #define DEBUGSERIAL Serial

int BearSSLClient::clientRead(void *ctx, unsigned char *buf, size_t len)
{
  Client* c = (Client*)ctx;

  if (!c->connected()) {
    return -1;
  }

  int result = c->read(buf, len);
  if (result == -1) {
    return 0;
  }

#ifdef DEBUGSERIAL
  DEBUGSERIAL.print("BearSSLClient::clientRead - ");
  DEBUGSERIAL.print(result);
  DEBUGSERIAL.print(" - ");  
  for (size_t i = 0; i < result; i++) {
    byte b = buf[i];

    if (b < 16) {
      DEBUGSERIAL.print("0");
    }
    DEBUGSERIAL.print(b, HEX);
  }
  DEBUGSERIAL.println();
#endif

  return result;
}

int BearSSLClient::clientWrite(void *ctx, const unsigned char *buf, size_t len)
{
  Client* c = (Client*)ctx;

#ifdef DEBUGSERIAL
  DEBUGSERIAL.print("BearSSLClient::clientWrite - ");
  DEBUGSERIAL.print(len);
  DEBUGSERIAL.print(" - ");
  for (size_t i = 0; i < len; i++) {
    byte b = buf[i];

    if (b < 16) {
      DEBUGSERIAL.print("0");
    }
    DEBUGSERIAL.print(b, HEX);
  }
  DEBUGSERIAL.println();
#endif

  if (!c->connected()) {
    return -1;
  }

  int result = c->write(buf, len);
  if (result == 0) {
    return -1;
  }

  return result;
}

void BearSSLClient::clientAppendCert(void *ctx, const void *data, size_t len)
{
  BearSSLClient* c = (BearSSLClient*)ctx;

  memcpy(&c->_ecCert[0].data[c->_ecCert[0].data_len], data, len);
  c->_ecCert[0].data_len += len;
}

void BearSSLClient::clientAppendKey(void *ctx, const void *data, size_t len)
{
  BearSSLClient* c = (BearSSLClient*)ctx;

  br_skey_decoder_push(c->_skeyDecoder, data, len);
}

void BearSSLClient::parentAppendCert(void *ctx, const void *data, size_t len)
{
  BearSSLClient* c = (BearSSLClient*)ctx;

  memcpy(&c->_ecCert[1].data[c->_ecCert[1].data_len], data, len);
  c->_ecCert[1].data_len += len;
}

