#ifndef BASE64_H
#define BASE64_H

#include <string>

std::string encode_base64(const std::string& str_unencoded);
std::string decode_base64(const std::string& str_encoded);

#endif // BASE64_H
