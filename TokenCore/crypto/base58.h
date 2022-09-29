
#ifndef BASE58_H
#define BASE58_H

#include <TokenCommon.h>

string encode_base58(const Binary& bin);
Binary decode_base58(const string& str);

string encode_base58check(const Binary& bin, uint8_t version = 0);
Binary decode_base58check(const string& str, uint8_t version = 0);

std::string encode_base58_bumo(const Binary& unencoded);
Binary decode_base58_bumo(const std::string& str);

#endif // BASE58_H
