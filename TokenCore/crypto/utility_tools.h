#ifndef UTILITY_TOOLS_H
#define UTILITY_TOOLS_H

#include <TokenCommon.h>

// The following guarantees declaration of the byte swap functions.
#ifdef _MSC_VER
#define bswap_16(x) _byteswap_ushort(x)
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)
// Mac OS X / Darwin features
#define bswap_16(x) __DARWIN_OSSwapInt16(x)
#define bswap_32(x) __DARWIN_OSSwapInt32(x)
#define bswap_64(x) __DARWIN_OSSwapInt64(x)

#elif !defined(bswap_16)

static inline uint16_t bswap_16(uint16_t x)
{
	return static_cast<uint16_t>(((x & 0xFF) << 8) | ((x & 0xFF00) >> 8));
}
#define bswap_16(x) bswap_16(x)

static inline uint32_t bswap_32(uint32_t x)
{
	return (((x & 0xFF) << 24) |
		((x & 0xFF00) << 8) |
		((x & 0xFF0000) >> 8) |
		((x & 0xFF000000) >> 24));
}
#define bswap_32(x) bswap_32(x)

static inline uint64_t bswap_64(uint64_t x)
{
	return (((x & uint64_t(0xFF)) << 56) |
		((x & uint64_t(0xFF00)) << 40) |
		((x & uint64_t(0xFF0000)) << 24) |
		((x & uint64_t(0xFF000000)) << 8) |
		((x & uint64_t(0xFF00000000)) >> 8) |
		((x & uint64_t(0xFF0000000000)) >> 24) |
		((x & uint64_t(0xFF000000000000)) >> 40) |
		((x & uint64_t(0xFF00000000000000)) >> 56));
}
#define bswap_64(x) bswap_64(x)

#endif

string reverse_big_little_endian(const string& str);
string get_pubkey_hash_from_base58check(const string& str_encoded);

string uint8_t_to_string(uint8_t value);
string uint16_t_to_little_endian(uint16_t value);
string uint32_t_to_little_endian(uint32_t value);
string uint64_t_to_little_endian(uint64_t value);
string uint16_t_to_big_endian(uint16_t value);
string uint32_t_to_big_endian(uint32_t value);
string uint64_t_to_big_endian(uint64_t value);

uint8_t string_to_uint8_t(const string& str_data);
uint16_t little_endian_to_uint16_t(const string& str_data);
uint16_t big_endian_to_uint16_t(const string& str_data);
uint32_t little_endian_to_uint32_t(const string& str_data);
uint32_t big_endian_to_uint32_t(const string& str_data);
uint64_t little_endian_to_uint64_t(const string& str_data);
uint64_t big_endian_to_uint64_t(const string& str_data);

#endif // UTILITY_TOOLS_H
