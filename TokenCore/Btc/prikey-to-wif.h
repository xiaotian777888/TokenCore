#ifndef ECTOWIF_H
#define ECTOWIF_H

#include <stdint.h>
#include <string>

// prikey to wif(wallet import format) format
// version 0x80:mainnet, 0xef:testnet
std::string ec_to_wif(std::string str_prikey, bool compress, uint8_t version);

#endif // ECTOWIF_H
