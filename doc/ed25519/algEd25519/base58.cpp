
#include "base58.h"

#include <algorithm>
#include <vector>
#include <iterator>

#include "base16.h"
// #include "hash.h"

typedef std::vector<unsigned char> data_slice;

const std::string base58_chars = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
const std::string base58_chars_bumo = "123456789AbCDEFGHJKLMNPQRSTuVWXYZaBcdefghijkmnopqrstUvwxyz";

/*bool is_base58(const char ch)
{
	// This works because the base58 characters happen to be in sorted order
	return std::binary_search(base58_chars.begin(), base58_chars.end(), ch);
}

bool is_base58(const std::string& text)
{
	const auto test = [](const char ch)
	{
		return is_base58(ch);
	};

	return std::all_of(text.begin(), text.end(), test);
}*/

template <typename Data>
auto search_first_nonzero(const Data& data) -> decltype(data.cbegin())
{
	auto first_nonzero = data.cbegin();
	while (first_nonzero != data.end() && *first_nonzero == 0)
		++first_nonzero;

	return first_nonzero;
}

size_t count_leading_zeros(data_slice unencoded)
{
	// Skip and count leading '1's.
	size_t leading_zeros = 0;
	for (const uint8_t byte : unencoded)
	{
		if (byte != 0)
			break;

		++leading_zeros;
	}

	return leading_zeros;
}

void pack_value(data_slice& indexes, size_t carry)
{
	// Apply "b58 = b58 * 256 + ch".
	for (auto it = indexes.rbegin(); it != indexes.rend(); ++it)
	{
		carry += 256 * (*it);
		*it = carry % 58;
		carry /= 58;
	}
}

std::string encode_base58_(data_slice unencoded)
{
	size_t leading_zeros = count_leading_zeros(unencoded);

	// size = log(256) / log(58), rounded up.
	const size_t number_nonzero = unencoded.size() - leading_zeros;
	const size_t indexes_size = number_nonzero * 138 / 100 + 1;

	// Allocate enough space in big-endian base58 representation.
	data_slice indexes(indexes_size);

	// Process the bytes.
	for (auto it = unencoded.begin() + leading_zeros;
		it != unencoded.end(); ++it)
	{
		pack_value(indexes, *it);
	}

	// Skip leading zeroes in base58 result.
	auto first_nonzero = search_first_nonzero(indexes);

	// Translate the result into a string.
	std::string encoded;
	const size_t estimated_size = leading_zeros +
		(indexes.end() - first_nonzero);
	encoded.reserve(estimated_size);
	encoded.assign(leading_zeros, '1');

	// Set actual main bytes.
	for (auto it = first_nonzero; it != indexes.end(); ++it)
	{
		const size_t index = *it;
		encoded += base58_chars[index];
	}

	return encoded;
}

std::string encode_base58_bumo_(data_slice unencoded)
{
    size_t leading_zeros = count_leading_zeros(unencoded);

    // size = log(256) / log(58), rounded up.
    const size_t number_nonzero = unencoded.size() - leading_zeros;
    const size_t indexes_size = number_nonzero * 138 / 100 + 1;

    // Allocate enough space in big-endian base58 representation.
    data_slice indexes(indexes_size);

    // Process the bytes.
    for (auto it = unencoded.begin() + leading_zeros;
        it != unencoded.end(); ++it)
    {
        pack_value(indexes, *it);
    }

    // Skip leading zeroes in base58 result.
    auto first_nonzero = search_first_nonzero(indexes);

    // Translate the result into a string.
    std::string encoded;
    const size_t estimated_size = leading_zeros +
        (indexes.end() - first_nonzero);
    encoded.reserve(estimated_size);
    encoded.assign(leading_zeros, '1');

    // Set actual main bytes.
    for (auto it = first_nonzero; it != indexes.end(); ++it)
    {
        const size_t index = *it;
        encoded += base58_chars_bumo[index];
    }

    return encoded;
}

std::string encode_base58(const std::string& str_unencoded)
{
	data_slice data_unencoded;
	std::string str_base16_decode;
	decode_base16(str_base16_decode, str_unencoded);
	std::copy(str_base16_decode.begin(), str_base16_decode.end(), back_inserter(data_unencoded));

	std::string strencoded = encode_base58_(data_unencoded);
	return strencoded;
}

std::string encode_base58_bumo(const std::string& str_unencoded)
{
    data_slice data_unencoded;
    std::string str_base16_decode;
    decode_base16(str_base16_decode, str_unencoded);
    std::copy(str_base16_decode.begin(), str_base16_decode.end(), back_inserter(data_unencoded));

    std::string strencoded = encode_base58_bumo_(data_unencoded);
    return strencoded;
}


// decode_base58

size_t count_leading_zeros(const std::string& encoded)
{
	// Skip and count leading '1's.
	size_t leading_zeros = 0;
	for (const uint8_t digit : encoded)
	{
		if (digit != base58_chars[0])
			break;

		++leading_zeros;
	}

	return leading_zeros;
}

void unpack_char(data_slice& data, size_t carry)
{
	for (auto it = data.rbegin(); it != data.rend(); it++)
	{
		carry += 58 * (*it);
		*it = carry % 256;
		carry /= 256;
	}
}

bool decode_base58(data_slice& out, const std::string& in)
{
	// Trim spaces and newlines around the string.
	const auto leading_zeros = count_leading_zeros(in);

	// log(58) / log(256), rounded up.
	const size_t data_size = in.size() * 733 / 1000 + 1;

	// Allocate enough space in big-endian base256 representation.
	data_slice data(data_size);

	// Process the characters.
	for (auto it = in.begin() + leading_zeros; it != in.end(); ++it)
	{
		const auto carry = base58_chars.find(*it);
		if (carry == std::string::npos)
			return false;

		unpack_char(data, carry);
	}

	// Skip leading zeroes in data.
	auto first_nonzero = search_first_nonzero(data);

	// Copy result into output vector.
	data_slice decoded;
	const size_t estimated_size = leading_zeros + (data.end() - first_nonzero);
	decoded.reserve(estimated_size);
	decoded.assign(leading_zeros, 0x00);
	decoded.insert(decoded.end(), first_nonzero, data.cend());

	out = decoded;
	return true;
}

bool decode_base58_bumo(data_slice& out, const std::string& in)
{
    // Trim spaces and newlines around the string.
    const auto leading_zeros = count_leading_zeros(in);

    // log(58) / log(256), rounded up.
    const size_t data_size = in.size() * 733 / 1000 + 1;

    // Allocate enough space in big-endian base256 representation.
    data_slice data(data_size);

    // Process the characters.
    for (auto it = in.begin() + leading_zeros; it != in.end(); ++it)
    {
        const auto carry = base58_chars_bumo.find(*it);
        if (carry == std::string::npos)
            return false;

        unpack_char(data, carry);
    }

    // Skip leading zeroes in data.
    auto first_nonzero = search_first_nonzero(data);

    // Copy result into output vector.
    data_slice decoded;
    const size_t estimated_size = leading_zeros + (data.end() - first_nonzero);
    decoded.reserve(estimated_size);
    decoded.assign(leading_zeros, 0x00);
    decoded.insert(decoded.end(), first_nonzero, data.cend());

    out = decoded;
    return true;
}

/* For support of template implementation only, do not call directly.
bool decode_base58_private(uint8_t* out, size_t out_size, const char* in)
{
	data_slice buffer;
	if (!decode_base58(buffer, in) || buffer.size() != out_size)
		return false;

	for (size_t i = 0; i < out_size; ++i)
		out[i] = buffer[i];

	return true;
}*/

std::string decode_base58(const std::string& str_encoded)
{
	data_slice vec_decoded;
	bool b_ok = decode_base58(vec_decoded, str_encoded);
	if (b_ok)
	{
		std::string str_decoded;
		encode_base16(str_decoded, vec_decoded);
		return str_decoded;
	}
	else
		return std::string("");
}

std::string decode_base58_bumo(const std::string& str_encoded)
{
    data_slice vec_decoded;
    bool b_ok = decode_base58_bumo(vec_decoded, str_encoded);
    if (b_ok)
    {
        std::string str_decoded;
        encode_base16(str_decoded, vec_decoded);
        return str_decoded;
    }
    else
        return std::string("");
}


//std::string encode_base58check(const std::string& str_unencoded, uint8_t version /* = 0 */)
/*{
	if (0 != str_unencoded.size() % 2)
		return std::string("");

	unsigned char szversion = version;
	std::vector<unsigned char> vec_version, vec_version_base16;
	vec_version.push_back(szversion);
	encode_base16(vec_version_base16, vec_version);

	std::string str_wrapped;
	std::copy(vec_version_base16.begin(), vec_version_base16.end(), std::back_inserter(str_wrapped));
	str_wrapped += str_unencoded;

	// 计算4字节校验和
	std::string str_checksum = bitcoin256(str_wrapped).substr(0, 8);
	str_wrapped += str_checksum;

	std::string str_res = encode_base58(str_wrapped);
	return str_res;
}*/
