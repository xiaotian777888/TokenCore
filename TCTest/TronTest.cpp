#include "TronApi.h"

#include <iostream>

#include "Coin.h"
#include "HttpAPI\BtxonAPI.h"
#include "CosExt.h"

static void test_validate_address()
{
	bool ret = TRONAPI::validate_address("TPiQeYwqPosuun4DB3UJ5sE7ajnray58nc");
	VF("validate Tron address", ret);
}

static void test_get_private_key()
{
	string private_key = TRONAPI::get_private_key("37f63c464ed1e319103598012d13b5f48f4712fdd55766390eeb58f3812d71ef3da4d5eec187b80bc3896d95e3e7aaead526bedd999c3dd74ba0b137a9b194ae");
	VF("get_private_key", (private_key == "cae3a51855b58a216fa3e73ff6f62c21b9029cccbf7147fed0248bffa13c164d"));
}

static void test_get_public_key()
{
	string str_prikey = "448e0a65c0950cb8cbf33418acdaa398be7929bc8093ad39dc88adf0e1a86290";
	string str_compres_pubkey = TRONAPI::get_public_key(str_prikey);
	VF("test get Tron compress public key", (str_compres_pubkey == "020974f34c5eaa4762dd8d24a4a2fdc4edc02640ee29a768352f96430f0ee73886"));

	string str_nocompress_pubkey = TRONAPI::get_public_key(str_prikey, false);
	VF("test get Tron uncompress public key", (str_nocompress_pubkey == "040974f34c5eaa4762dd8d24a4a2fdc4edc02640ee29a768352f96430f0ee73886a75e08caabcbef9c8d458e2a1cfec682de91ffb72f65f4083b0d2e037df64f0a"));
}

static void test_get_address()
{
	string str_uncompress_pubkey = "040974f34c5eaa4762dd8d24a4a2fdc4edc02640ee29a768352f96430f0ee73886a75e08caabcbef9c8d458e2a1cfec682de91ffb72f65f4083b0d2e037df64f0a";
	
	string str_mainnet_address = TRONAPI::get_address(str_uncompress_pubkey);
	VF("test get Tron mainnet address", (str_mainnet_address == "TMxbZ97qmYc9sqhKznrbsAeN2B1FMN3B6R"));

	string str_testnet_address = TRONAPI::get_address(str_uncompress_pubkey, false);
	VF("test get Tron testnet address", (str_testnet_address == "27b4u6TSDDgdPa1wZKeVsw5W6p6UroTvseo"));
}

static void test_decompress_pub_key()
{
	string str_compress_pubkey = "020974f34c5eaa4762dd8d24a4a2fdc4edc02640ee29a768352f96430f0ee73886";
	string str_uncompress_pubkey = TRONAPI::decompress_pub_key(str_compress_pubkey);
	VF("test tron decompress pubkey", (str_uncompress_pubkey) == "040974f34c5eaa4762dd8d24a4a2fdc4edc02640ee29a768352f96430f0ee73886a75e08caabcbef9c8d458e2a1cfec682de91ffb72f65f4083b0d2e037df64f0a");
}

static void test_make_unsigned_tx_from_json() {
	std::string str_unsigned_tx_trx_json, str_unsign_tx;

	//str_unsigned_tx_trx_json = "{\"txID\":\"e8235c4751ff9102e59d2cb315bea13c5c500590ff4ba703c92e4969f88ef5ab\",\"raw_data\":{\"contract\":[{\"parameter\":{\"value\":{\"amount\":1000000,\"owner_address\":\"418be260aa4fd9cc74d08029faf4f42b7c8d0983e9\",\"to_address\":\"415417087862157ef7f77aa44ddb5d24463daf018c\"},\"type_url\":\"type.googleapis.com/protocol.TransferContract\"},\"type\":\"TransferContract\"}],\"ref_block_bytes\":\"5fd8\",\"ref_block_hash\":\"228e1f651d9ea3bf\",\"expiration\":1548058839000,\"timestamp\":1548058782434}}";
	//str_unsign_tx = TRONAPI::make_unsigned_tx_trx_from_json(str_unsigned_tx_trx_json);
	//std::cout << "str_unsign_tx:\n" << str_unsign_tx << std::endl;

	//str_unsigned_tx_trx_json = "{\"txID\":\"e8235c4751ff9102e59d2cb315bea13c5c500590ff4ba703c92e4969f88ef5ab\",\"raw_data\":{\"contract\":[{\"parameter\":{\"value\":{\"owner_address\":\"415bdb6c08021a83a86dabc6d50ed2b97e5ab1aa3b\"},\"type_url\":\"type.googleapis.com/protocol.UnfreezeBalanceContract\"},\"type\":\"UnfreezeBalanceContract\"}],\"ref_block_bytes\":\"5fd8\",\"ref_block_hash\":\"228e1f651d9ea3bf\",\"expiration\":1548058839000,\"timestamp\":1548058782434}}";
	//str_unsign_tx = TRONAPI::make_unsigned_tx_unfreeze_from_json(str_unsigned_tx_trx_json);
	//std::cout << "str_unsign_tx:\n" << str_unsign_tx << std::endl;

	// freeze bandwidth
	//str_unsigned_tx_trx_json = "{\"txID\":\"e20a34b9c2e30b89182d092e250a3ce0313fc9919ca9878ae0db74721fa8621f\",\"raw_data\":{\"contract\":[{\"parameter\":{\"value\":{\"frozen_duration\":3,\"frozen_balance\":100000000, \"owner_address\":\"4196c53e5572a128541a8d573b717d9d6b161b2a7e\"},\"type_url\":\"type.googleapis.com/protocol.FreezeBalanceContract\"},\"type\":\"FreezeBalanceContract\"}],\"ref_block_bytes\":\"b537\",\"ref_block_hash\":\"851838cbe3077f98\",\"expiration\":1548324529000,\"timestamp\":1548323472657}}";
	//str_unsign_tx = TRONAPI::make_unsigned_tx_freeze_from_json(str_unsigned_tx_trx_json);
	//std::cout << "str_unsign_tx:\n" << str_unsign_tx << std::endl;

	// freeze energy
	str_unsigned_tx_trx_json = "{\"txID\":\"72eba522a22d3608137da36e76d59258177b10a5837d503adfe39177b938909d\",\"raw_data\":{\"contract\":[{\"parameter\":{\"value\":{\"frozen_duration\":3,\"frozen_balance\":100000000, \"owner_address\":\"4196c53e5572a128541a8d573b717d9d6b161b2a7e\", \"resource\":\"ENERGY\"},\"type_url\":\"type.googleapis.com/protocol.FreezeBalanceContract\"},\"type\":\"FreezeBalanceContract\"}],\"ref_block_bytes\":\"b648\",\"ref_block_hash\":\"96ad32015be6f3c3\",\"expiration\":1548324348000,\"timestamp\":1548324291419}}";
	str_unsign_tx = TRONAPI::make_unsigned_tx_freeze_from_json(str_unsigned_tx_trx_json);
	std::cout << "str_unsign_tx:\n" << str_unsign_tx << std::endl;

	string str_prikey = "d30aecd5c437684be9d942c12ac0d003cf5abf752f0e3f94ebbfbe698823cb39";
	string str_sign = TRONAPI::sign_tx(str_unsign_tx, str_prikey);
	std::cout << "str_sign:\n" << str_sign << std::endl;

	string str_sign_tx = TRONAPI::make_sign_tx(str_unsign_tx, str_sign);
	std::cout << "str_sign_tx:\n" << str_sign_tx << std::endl;
}


void TronTest()
{
	//test_validate_address();
	//test_get_private_key();
	//test_get_public_key();
	//test_get_address();
	//test_decompress_pub_key();
	test_make_unsigned_tx_from_json();

	/*
	int ret;
	char* result;

	ret = TRX_API::TxGetPriKey("{\"seed\":\"42f9da6b087f0ecca42dd51e4149974a50a27ea07090efb6ac60f8b7b68d02b0fd02366b01be1bf035d0a633812c0e32a1c0f72a3e279f9c5b084f31a72d8687\"}", &result);
	ret = TRX_API::TxGetPubKey("{\"compress\":false,\"pri_key\":\"1b75649f3d5d2d23bcb9fb5d9a29a8ceaa101c992d67881ed625ecd9939d8605\"}", &result);
	ret = TRX_API::TxGetAddress("{\"main_net\":true,\"pub_key\":\"044e85195e3be22398a12698db4efbff09cae1c2193d779fba92d6454e89f419247f3b1c55a71ab023756e6d4ec2c9c9d700ed55a0475b4868cf7965cab0525520\"}", &result);
	*/
}
