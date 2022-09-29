
#ifndef BASE58_H
#define BASE58_H

#include <string>
#include <stdint.h>

std::string encode_base58(const std::string& str_unencoded);
std::string decode_base58(const std::string& str_encoded);

std::string encode_base58check(const std::string& str_unencoded, uint8_t version = 0);
std::string decode_base58check(const std::string& str_encoded, uint8_t version = 0);

std::string encode_base58_bumo(const std::string& str_unencoded);
std::string decode_base58_bumo(const std::string& str_encoded);

#endif // BASE58_H
