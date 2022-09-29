
#include "prikey-to-wif.h"

#include <vector>
#include <algorithm>
#include <iterator>

#include "TokenCommon.h"
#include "crypto/utility_tools.h"
#include "crypto/hash.h"
#include "crypto/base58.h"

//static size_t wif_uncompressed_size = 37u;
//static size_t wif_compressed_size = wif_uncompressed_size + 1u;
//const uint8_t compressed_sentinel = 0x01;

//static uint16_t to_version(uint8_t address, uint8_t wif) {
//    return uint16_t(wif) << 8 | address;
//}

//const uint8_t mainnet_wif = 0x80;
//const uint8_t mainnet_p2kh = 0x00;
//const uint16_t mainnet = to_version(mainnet_p2kh, mainnet_wif);

//const uint8_t testnet_wif = 0xef;
//const uint8_t testnet_p2kh = 0x6f;
//const uint16_t testnet = to_version(testnet_p2kh, testnet_wif);

std::string ec_to_wif(std::string str_prikey, bool compress, uint8_t version)
{
	if (str_prikey.size() != 64) {
		return std::string("");
	}

	std::string str_check = uint8_t_to_string(version) + str_prikey;
	if (compress) {
		str_check += "01";
	}
	Binary sz_check = Binary::decode(str_check);
	Binary sz_checksum = bitcoin256(sz_check).left(4);	
	sz_check += sz_checksum;
	return encode_base58(sz_check);
}
