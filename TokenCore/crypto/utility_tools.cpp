#include <algorithm>
#include "utility_tools.h"
#include "base58.h"

// These #includes are for the byte swap functions declared later on.
#ifdef _MSC_VER
#include <stdlib.h>  // NOLINT(build/include)
#include <intrin.h>
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#elif defined(__GLIBC__) || defined(__BIONIC__) || defined(__CYGWIN__)
#include <byteswap.h>  // IWYU pragma: export
#endif

// long long macros to be used because gcc and vc++ use different suffixes,
// and different size specifiers in format strings
#undef LONGLONG
#undef ULONGLONG

#ifdef _MSC_VER
#define LONGLONG(x) x##I64
#define ULONGLONG(x) x##UI64
#else
// By long long, we actually mean int64.
#define LONGLONG(x) x##LL
#define ULONGLONG(x) x##ULL
#endif

string reverse_big_little_endian(const string& str)
{
	if (0 != str.size() % 2)
		return string("");

	Binary bin = Binary::decode(str);
	bin.reverse();
	return Binary::encode(bin);
}

string get_pubkey_hash_from_base58check(const string& str_encoded)
{
	string str_decoded = Binary::encode(decode_base58(str_encoded));
	str_decoded = str_decoded.substr(0, str_decoded.size() - 8);
	str_decoded = str_decoded.substr(2, str_decoded.size());
	return str_decoded;
}

string uint8_t_to_string(uint8_t value)
{
	Binary bin(1);
	*((uint8_t*)bin.data()) = value;
	return Binary::encode(bin);
}

string uint16_t_to_little_endian(uint16_t value)
{
	Binary bin(2);
	*((uint16_t*)bin.data()) = value;
	return Binary::encode(bin);
}

string uint32_t_to_little_endian(uint32_t value)
{
	Binary bin(4);
	*((uint32_t*)bin.data()) = value;
	return Binary::encode(bin);
}

string uint64_t_to_little_endian(uint64_t value)
{
	Binary bin(8);
	*((uint64_t*)bin.data()) = value;
	return Binary::encode(bin);
}

string uint16_t_to_big_endian(uint16_t value)
{
	return uint16_t_to_little_endian(bswap_16(value));
}
string uint32_t_to_big_endian(uint32_t value)
{
	return uint32_t_to_little_endian(bswap_32(value));
}

string uint64_t_to_big_endian(uint64_t value)
{
	return uint64_t_to_little_endian(bswap_64(value));
}

uint8_t string_to_uint8_t(const string& str_data)
{
	Binary vec_data = Binary::decode(str_data);
	uint8_t u8 = vec_data[0];
	return u8;
}

uint16_t little_endian_to_uint16_t(const string& str_data)
{
	Binary vec_data = Binary::decode(str_data);
	return *((uint16_t*)vec_data.data());
}

uint16_t big_endian_to_uint16_t(const string& str_data)
{
	Binary vec_data = Binary::decode(str_data);
	return bswap_16(*((uint16_t*)vec_data.data()));
}

uint32_t little_endian_to_uint32_t(const string& str_data)
{
	Binary vec_data = Binary::decode(str_data);
	return *((uint32_t*)vec_data.data());
}

uint32_t big_endian_to_uint32_t(const string& str_data)
{
	Binary vec_data = Binary::decode(str_data);
	return bswap_32(*((uint32_t*)vec_data.data()));
}

uint64_t little_endian_to_uint64_t(const string& str_data)
{
	Binary vec_data = Binary::decode(str_data);
	return *((uint64_t*)vec_data.data());
}

uint64_t big_endian_to_uint64_t(const string& str_data)
{
	Binary vec_data = Binary::decode(str_data);
	return bswap_64(*((uint64_t*)vec_data.data()));
}
