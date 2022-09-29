#ifndef __EOSAPI_UTILS_H__
#define __EOSAPI_UTILS_H__

#include <TokenCommon.h>
#include "crypto/bx_str.h"

bool bx_hex2bin(const char* str_hex, unsigned char* buf_data, int buf_size);
void bx_memrevert(unsigned char* buf_data, int buf_size);

void bx_log_bin(const u8* dat, u16 dataLen);

bool bx_eos_wif2bin(const string& wif, Binary& bin);

#endif // __EOSAPI_UTILS_H__
