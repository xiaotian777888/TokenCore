#ifndef BUMOREQUEST_H_
#define BUMOREQUEST_H_

#include <string>

uint64_t get_nonce(const std::string &str_address);
uint64_t get_gas_price();
bool broadcast_tx_json(const std::string &str_tx_json);

#endif // BUMOREQUEST_H_
