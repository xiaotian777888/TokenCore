#include <algorithm>
#include <vector>
#include <iterator>

#include "base58.h"
#include "hash.h"

const std::string base58_chars = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
const std::string base58_chars_bumo = "123456789AbCDEFGHJKLMNPQRSTuVWXYZaBcdefghijkmnopqrstUvwxyz";

bool is_base58(const string& text)
{
	return all_of(text.begin(), text.end(), [](const char ch) { return binary_search(base58_chars.begin(), base58_chars.end(), ch); });
}

template <typename Data>
auto search_first_nonzero(const Data& data) -> decltype(data.cbegin())
{
	auto first_nonzero = data.cbegin();
	while (first_nonzero != data.end() && *first_nonzero == 0)
		++first_nonzero;

	return first_nonzero;
}

size_t count_leading_zeros(const Binary& unencoded)
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

void pack_value(Binary& indexes, size_t carry)
{
	// Apply "b58 = b58 * 256 + ch".
	for (auto it = indexes.rbegin(); it != indexes.rend(); ++it)
	{
		carry += 256 * (*it);
		*it = carry % 58;
		carry /= 58;
	}
}

string encode_base58(const Binary& bin)
{
	size_t leading_zeros = count_leading_zeros(bin);

	// size = log(256) / log(58), rounded up.
	const size_t number_nonzero = bin.size() - leading_zeros;
	const size_t indexes_size = number_nonzero * 138 / 100 + 1;

	// Allocate enough space in big-endian base58 representation.
	Binary indexes(indexes_size);

	// Process the bytes.
	for (auto it = bin.begin() + leading_zeros;
		it != bin.end(); ++it)
	{
		pack_value(indexes, *it);
	}

	// Skip leading zeroes in base58 result.
	auto first_nonzero = search_first_nonzero(indexes);

	// Translate the result into a string.
	string encoded;
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

std::string encode_base58_bumo(const Binary& unencoded) {
	size_t leading_zeros = count_leading_zeros(unencoded);

	// size = log(256) / log(58), rounded up.
	const size_t number_nonzero = unencoded.size() - leading_zeros;
	const size_t indexes_size = number_nonzero * 138 / 100 + 1;

	// Allocate enough space in big-endian base58 representation.
	Binary indexes(indexes_size);

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

// decode_base58
size_t count_leading_zeros(const string& encoded)
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

void unpack_char(Binary& data, size_t carry)
{
	for (auto it = data.rbegin(); it != data.rend(); it++)
	{
		carry += 58 * (*it);
		*it = carry % 256;
		carry /= 256;
	}
}

// decode_base58
Binary decode_base58(const string& str)
{
	Binary decoded;

	// Trim spaces and newlines around the string.
	const auto leading_zeros = count_leading_zeros(str);

	// log(58) / log(256), rounded up.
	const size_t data_size = str.size() * 733 / 1000 + 1;

	// Allocate enough space in big-endian base256 representation.
	Binary data(data_size);

	// Process the characters.
	//for (auto it = str.begin() + leading_zeros; it != str.end(); ++it)
	for (size_t i = leading_zeros; i < str.size(); i++)
	{
		const auto carry = base58_chars.find(str[i]);
		if (carry == string::npos)
			return decoded;

		unpack_char(data, carry);
	}

	// Skip leading zeroes in data.
	auto first_nonzero = search_first_nonzero(data);

	// Copy result into output vector.
	const size_t estimated_size = leading_zeros + (data.end() - first_nonzero);
	decoded.reserve(estimated_size);
	decoded.assign(leading_zeros, 0x00);
	decoded.insert(decoded.end(), first_nonzero, data.cend());

	return decoded;
}

Binary decode_base58_bumo(const std::string& in) {
	// Trim spaces and newlines around the string.
	const auto leading_zeros = count_leading_zeros(in);

	// log(58) / log(256), rounded up.
	const size_t data_size = in.size() * 733 / 1000 + 1;

	// Allocate enough space in big-endian base256 representation.
	Binary data(data_size);

	// Process the characters.
	for (auto it = in.begin() + leading_zeros; it != in.end(); ++it)
	{
		const auto carry = base58_chars_bumo.find(*it);
		if (carry == std::string::npos)
			return Binary();

		unpack_char(data, carry);
	}

	// Skip leading zeroes in data.
	auto first_nonzero = search_first_nonzero(data);

	// Copy result into output vector.
	Binary decoded;
	const size_t estimated_size = leading_zeros + (data.end() - first_nonzero);
	decoded.reserve(estimated_size);
	decoded.assign(leading_zeros, 0x00);
	decoded.insert(decoded.end(), first_nonzero, data.cend());

	return decoded;
}

string encode_base58check(const Binary& bin, uint8_t version /* = 0 */)
{
	Binary all;
	all.push_back(version);
	all += bin;

	// 计算4字节校验和
	Binary checksum = bitcoin256(all);
	all += checksum.left(4);

	return encode_base58(all);
}
