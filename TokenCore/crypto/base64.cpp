
#include "base64.h"

const static char pad = '=';
const static char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string encode_base64(const std::string& str_unencoded){
    if ("" == str_unencoded) {
        return std::string("");
    }

    std::string encoded;
    const size_t size = str_unencoded.size();
    encoded.reserve(((size / 3) + (size % 3 > 0)) * 4);

    uint32_t value;
    std::string::const_iterator cursor = str_unencoded.begin();
    for (size_t position = 0; position < size / 3; position++)
    {
        // Convert to big endian.
        value = (*cursor++) << 16;

        value += (*cursor++) << 8;
        value += (*cursor++);
        encoded.append(1, table[(value & 0x00FC0000) >> 18]);
        encoded.append(1, table[(value & 0x0003F000) >> 12]);
        encoded.append(1, table[(value & 0x00000FC0) >> 6]);
        encoded.append(1, table[(value & 0x0000003F) >> 0]);
    }

    switch (size % 3)
    {
        case 1:
            // Convert to big endian.
            value = (*cursor++) << 16;

            encoded.append(1, table[(value & 0x00FC0000) >> 18]);
            encoded.append(1, table[(value & 0x0003F000) >> 12]);
            encoded.append(2, pad);
            break;
        case 2:
            // Convert to big endian.
            value = (*cursor++) << 16;

            value += (*cursor++) << 8;
            encoded.append(1, table[(value & 0x00FC0000) >> 18]);
            encoded.append(1, table[(value & 0x0003F000) >> 12]);
            encoded.append(1, table[(value & 0x00000FC0) >> 6]);
            encoded.append(1, pad);
            break;
    }

    return encoded;
}

std::string decode_base64(const std::string& str_encoded) {
	if ("" == str_encoded || str_encoded.length() % 4 != 0) {
		return std::string("");
	}

	const static uint32_t mask = 0x000000FF;
	const size_t length = str_encoded.length();
	size_t padding = 0;
	if (length > 0)
	{
		if (str_encoded[length - 1] == pad)
			padding++;
		if (str_encoded[length - 2] == pad)
			padding++;
	}

	std::string decoded;
	decoded.reserve(((length / 4) * 3) - padding);

	uint32_t value = 0;
	for (auto cursor = str_encoded.begin(); cursor < str_encoded.end();)
	{
		for (size_t position = 0; position < 4; position++)
		{
			value <<= 6;
			if (*cursor >= 0x41 && *cursor <= 0x5A)
				value |= *cursor - 0x41;
			else if (*cursor >= 0x61 && *cursor <= 0x7A)
				value |= *cursor - 0x47;
			else if (*cursor >= 0x30 && *cursor <= 0x39)
				value |= *cursor + 0x04;
			else if (*cursor == 0x2B)
				value |= 0x3E;
			else if (*cursor == 0x2F)
				value |= 0x3F;
			else if (*cursor == pad)
			{
				// Handle 1 or 2 pad characters.
				switch (str_encoded.end() - cursor)
				{
				case 1:
					decoded.push_back((value >> 16) & mask);
					decoded.push_back((value >> 8) & mask);
					return decoded;
				case 2:
					decoded.push_back((value >> 10) & mask);
					return decoded;
				default:
					return std::string("");
				}
			}
			else
				return std::string("");

			cursor++;
		}

		decoded.push_back((value >> 16) & mask);
		decoded.push_back((value >> 8) & mask);
		decoded.push_back((value >> 0) & mask);
	}

	return decoded;
}
