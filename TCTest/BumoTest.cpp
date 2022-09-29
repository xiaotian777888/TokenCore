
#include "BumoApi.h"

#include "Coin.h"
#include "HttpAPI\BtxonAPI.h"
#include "CosExt.h"

#include <iostream>

static void test_validate_address() {
	bool ret = BUMOAPI::validate_address("buQWJce6vcaRvRmoifX27Zt7MXGJqJfkT99P");
	VF("validate Bumo address", ret);
}

static void test_get_private_key() {
	std::string str_prikey = BUMOAPI::get_private_key();
	if (str_prikey.size() != 64) {
		VF("test get Bumo private key", false);
	}
	else {
		VF("test get Bumo private key", true);
	}
}

static void test_get_bumo_private_key() {
	std::string str_prikey = "11ec18b7cffacfb46c57e027bd63f6558a78ec4ee4e929c07c12345678abcdef";
	std::string str_bumo_prikey = BUMOAPI::get_bumo_private_key(str_prikey);
	VF("test get Bumo private key", (str_bumo_prikey == "privbsGZFUoRv8aXZbSGd3bwzZWFn3L5QKq74RX8gVzbh1qsA5Rttdqj"));
}

static void test_get_public_key() {
	std::string str_prikey = "11ec18b7cffacfb46c57e027bd63f6558a78ec4ee4e929c07c12345678abcdef";
	std::string str_pubkey = BUMOAPI::get_public_key(str_prikey);
	VF("test get public key", (str_pubkey == "d342fb43be2c61e425cd3448d457de446d87e518a865878a737755600c9076fb"));
}

static void test_get_bumo_public_key() {
	std::string str_pubkey = "d342fb43be2c61e425cd3448d457de446d87e518a865878a737755600c9076fb";
	std::string str_bumo_pubkey = BUMOAPI::get_bumo_public_key(str_pubkey);
	VF("test get Bumo public key", (str_bumo_pubkey == "b001d342fb43be2c61e425cd3448d457de446d87e518a865878a737755600c9076fba89e5910"));
}

static void test_get_address() {
	std::string str_pubkey = "d342fb43be2c61e425cd3448d457de446d87e518a865878a737755600c9076fb";
	std::string str_address = BUMOAPI::get_address(str_pubkey);
	VF("test get Bumo address", (str_address == "buQWJce6vcaRvRmoifX27Zt7MXGJqJfkT99P"));
}

static void test_bumo_coin() {
	std::string str_source_address = "buQWJce6vcaRvRmoifX27Zt7MXGJqJfkT99P";
	std::string str_source_prikey = "11ec18b7cffacfb46c57e027bd63f6558a78ec4ee4e929c07c12345678abcdef";
	std::string str_source_pubkey = "d342fb43be2c61e425cd3448d457de446d87e518a865878a737755600c9076fb";
	std::string str_source_bumo_pubkey = "b001d342fb43be2c61e425cd3448d457de446d87e518a865878a737755600c9076fba89e5910";

	std::string str_dest_address = "buQnGKQnLZ4tRjoGK6AoWvmKCephAXSE8bdw";
	uint64_t amount = 1;
	uint64_t nonce = 0;
	uint64_t gas_price = 1000;
	uint64_t fee_limit = 1000000;

	std::string str_unsign_tx = BUMOAPI::make_unsigned_tx_bumo_coin(
		str_source_address, str_dest_address, amount, nonce, gas_price, fee_limit);
	std::cout << "str_unsign_tx:\n" << str_unsign_tx << std::endl;

	std::string str_tx_hash = BUMOAPI::tx_hash(str_unsign_tx);
	std::cout << "str_tx_hash:\n" << str_tx_hash << std::endl;

	std::string str_sign = BUMOAPI::sign_tx(str_unsign_tx, str_source_prikey, str_source_pubkey);
	std::cout << "str_sign:\n" << str_sign << std::endl;

	std::string str_sign_tx_json = BUMOAPI::make_sign_tx(
		str_unsign_tx, str_sign, str_source_bumo_pubkey);
	std::cout << "str_sign_tx_json:\n" << str_sign_tx_json << std::endl;

	bool is_broadcast_ok = false;//  ::broadcast_transaction_json(str_sign_tx_json);
	if (is_broadcast_ok)
		std::cout << "broadcast tx ok." << std::endl;
	else
		std::cout << "broadcast tx failed." << std::endl;
}

static void test_bumo_ATP_token() {
	string str_source_address = "buQWJce6vcaRvRmoifX27Zt7MXGJqJfkT99P";
	string str_source_prikey = "11ec18b7cffacfb46c57e027bd63f6558a78ec4ee4e929c07c12345678abcdef";
	string str_source_pubkey = "d342fb43be2c61e425cd3448d457de446d87e518a865878a737755600c9076fb";
	string str_source_bumo_pubkey = "b001d342fb43be2c61e425cd3448d457de446d87e518a865878a737755600c9076fba89e5910";

	string str_dest_address = "buQnGKQnLZ4tRjoGK6AoWvmKCephAXSE8bdw";
	string str_issuer_address = "buQsurH1M4rjLkfjzkxR9KXJ6jSu2r9xBNEw";
	string str_code = "GLA";
	uint64_t amount = 1;
	uint64_t nonce = 0;
	uint64_t gas_price = 1000;
	uint64_t fee_limit = 500000;

	string str_unsign_tx = BUMOAPI::make_unsigned_tx_ATP_token(
		str_source_address, str_dest_address, str_issuer_address,
		str_code, amount, nonce, gas_price, fee_limit);
	cout << "str_unsign_tx:\n" << str_unsign_tx << endl;

	string str_sign = BUMOAPI::sign_tx(str_unsign_tx, str_source_prikey, str_source_pubkey);
	cout << "str_sign:\n" << str_sign << endl;

	string str_sign_tx_json = BUMOAPI::make_sign_tx(
		str_unsign_tx, str_sign, str_source_bumo_pubkey);
	cout << "str_sign_tx_json:\n" << str_sign_tx_json << endl;

	bool is_broadcast_ok = BUMOAPI::broadcast_transaction_json(str_sign_tx_json);
	if (is_broadcast_ok)
		cout << "broadcast tx ok." << endl;
	else
		cout << "broadcast tx failed." << endl;
}

static void test_bumo_CTP_token() {
	string str_source_address = "buQWJce6vcaRvRmoifX27Zt7MXGJqJfkT99P";
	string str_source_prikey = "11ec18b7cffacfb46c57e027bd63f6558a78ec4ee4e929c07c12345678abcdef";
	string str_source_pubkey = "d342fb43be2c61e425cd3448d457de446d87e518a865878a737755600c9076fb";
	string str_source_bumo_pubkey = "b001d342fb43be2c61e425cd3448d457de446d87e518a865878a737755600c9076fba89e5910";

	string str_dest_address = "buQnGKQnLZ4tRjoGK6AoWvmKCephAXSE8bdw";
	string str_contract_address = "buQpqMjJDMi9ULzGBc3wfvzzBreeQ7Q6ERGy";
	uint64_t coin_amount = 0;
	uint64_t token_amount = 1;
	uint64_t nonce = 0;
	uint64_t gas_price = 1000;
	uint64_t fee_limit = 1000000;

	string str_unsign_tx = BUMOAPI::make_unsigned_tx_CTP_token(
		str_source_address, str_dest_address, coin_amount,
		str_contract_address, token_amount,
		nonce, gas_price, fee_limit);
	cout << "str_unsign_tx:\n" << str_unsign_tx << endl;

	string str_sign = BUMOAPI::sign_tx(str_unsign_tx, str_source_prikey, str_source_pubkey);
	cout << "str_sign:\n" << str_sign << endl;

	string str_sign_tx_json = BUMOAPI::make_sign_tx(
		str_unsign_tx, str_sign, str_source_bumo_pubkey);
	cout << "str_sign_tx_json:\n" << str_sign_tx_json << endl;

	bool is_broadcast_ok = BUMOAPI::broadcast_transaction_json(str_sign_tx_json);
	if (is_broadcast_ok)
		cout << "broadcast tx ok." << endl;
	else
		cout << "broadcast tx failed." << endl;
}



void BumoTest() {
	test_validate_address();
	test_get_private_key();
	test_get_bumo_private_key();
	test_get_public_key();
	test_get_bumo_public_key();
	test_get_address();
	test_bumo_coin();
	test_bumo_ATP_token();
	test_bumo_CTP_token();
}
