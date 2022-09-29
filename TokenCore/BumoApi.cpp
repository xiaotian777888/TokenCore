#include "BumoApi.h"

#include "TokenCommon.h"
#include "crypto/hash.h"
#include "crypto/base58.h"
#include "json/json.h"
#include "Bumo/ed25519/ed25519.h"
#include "Bumo/chain.pb.h"
#include "Bumo/BumoRequest.h"

namespace BUMOAPI {

	bool validate_address(const std::string& str_address) {
		if (str_address.size() != 36) {
			return false;
		}

		Binary sz_address = decode_base58_bumo(str_address);
		Binary sz_checksum = sz_address.right(4);
		Binary sz_data = sz_address.left(sz_address.size() - 4);
		Binary sz_checksum_tmp = bitcoin256(sz_data).left(4);
		if (sz_checksum == sz_checksum_tmp) {
			return true;
		}
		else {
			return false;
		}
	}

	std::string get_private_key() {
		return gen_seed(256);
	}

	std::string get_bumo_private_key(const std::string& str_prikey) {
		if (str_prikey.size() != 64) {
			return std::string("");
		}

		std::string str_prefix = "da379f";
		std::string str_version = "01";
		std::string str_fill = "00";
		std::string str_data = str_prefix + str_version + str_prikey + str_fill;

		Binary sz_data = Binary::decode(str_data);
		Binary sz_checksum = bitcoin256(sz_data).left(4);
		sz_data.insert(sz_data.end(), sz_checksum.begin(), sz_checksum.end());
		std::string str_bumo_prikey = encode_base58_bumo(sz_data);
		return str_bumo_prikey;
	}

	std::string get_public_key(const std::string& str_prikey) {
		if (str_prikey.size() != 64) {
			return std::string("");
		}

		unsigned char sz_pubkey[32] = { 0 };
		Binary sz_prikey = Binary::decode(str_prikey);
		ed25519_public_key(sz_pubkey, sz_prikey.data());
		std::string str_pubkey = Binary::encode(Binary(sz_pubkey, sizeof(sz_pubkey)));
		return str_pubkey;
	}

	std::string get_bumo_public_key(const std::string& str_pubkey){
		if (str_pubkey.size() != 64) {
			return std::string("");
		}

		std::string str_prefix = "b0", str_version = "01";
		std::string str_bumo_pubkey = str_prefix + str_version + str_pubkey;
		Binary sz_bumo_pubkey = Binary::decode(str_bumo_pubkey);
		Binary sz_checksum = bitcoin256(sz_bumo_pubkey).left(4);
		sz_bumo_pubkey += sz_checksum;
		str_bumo_pubkey = Binary::encode(sz_bumo_pubkey);
		return str_bumo_pubkey;
	}

	std::string get_address(const std::string& str_pubkey) {
		if (str_pubkey.size() != 64) {
			return std::string("");
		}

		Binary sz_raw_pubkey = Binary::decode(str_pubkey);
		Binary sz_address = sha256_hash(sz_raw_pubkey).right(20);
		Binary sz_prefix = Binary::decode(string("0156"));
		Binary sz_version = Binary::decode(string("01"));
		sz_address = sz_prefix + sz_version + sz_address;
		Binary sz_checksum = bitcoin256(sz_address).left(4);
		sz_address += sz_checksum;

		std::string str_address = encode_base58_bumo(sz_address);
		return str_address;
	}

	std::string make_unsigned_tx_bumo_coin(
		const std::string &str_source_address,
		const std::string &str_dest_address,
		uint64_t amount,
		uint64_t nonce,
		uint64_t gas_price,
		uint64_t fee_limit
	) {
		if (str_source_address.size() != 36 ||
			str_dest_address.size() != 36) {
			return std::string("");
		}

		nonce = get_nonce(str_source_address);
		//std::cout << nonce << std::endl;
		nonce++;
		gas_price = get_gas_price();
		//std::cout << gas_price << std::endl;

		bumoProtocol::Transaction tx;
		bumoProtocol::Operation *lp_operation = tx.add_operations();
		lp_operation->set_type(bumoProtocol::Operation_Type_PAY_COIN);
		lp_operation->set_source_address(str_source_address.c_str(), str_source_address.size());

		bumoProtocol::OperationPayCoin *lp_paycoin = lp_operation->mutable_pay_coin();
		lp_paycoin->set_amount(amount);
		lp_paycoin->set_dest_address(str_dest_address.c_str(), str_dest_address.size());

		tx.set_source_address(str_source_address.c_str(), str_source_address.size());
		tx.set_nonce(nonce);
		tx.set_fee_limit(fee_limit);		
		tx.set_gas_price(gas_price);

		int len = tx.ByteSize();
		Binary sz_unsign_tx(len);
		tx.SerializeToArray(sz_unsign_tx.data(), len);
		std::string str_unsign_tx = Binary::encode(sz_unsign_tx);
		return str_unsign_tx;
	}

	std::string make_unsigned_tx_ATP_token(
		const std::string &str_source_address,
		const std::string &str_dest_address,
		const std::string &str_issuer_address,
		const std::string &str_token_name,
		uint64_t amount,
		uint64_t nonce,
		uint64_t gas_price,
		uint64_t fee_limit
	) {
		if (str_source_address.size()	!= 36 ||
			str_dest_address.size()		!= 36 ||
			str_issuer_address.size()	!= 36 ||
			"" == str_token_name ||
			0 == amount) {
			return std::string("");
		}

		nonce = get_nonce(str_source_address);
		//std::cout << nonce << std::endl;
		nonce++;

		gas_price = get_gas_price();
		//std::cout << gas_price << std::endl;

		bumoProtocol::Transaction tx;
		bumoProtocol::Operation *lp_operation = tx.add_operations();
		lp_operation->set_type(bumoProtocol::Operation_Type_PAY_ASSET);
		lp_operation->set_source_address(str_source_address.c_str(), str_source_address.size());

		bumoProtocol::OperationPayAsset *lp_payasset = lp_operation->mutable_pay_asset();
		lp_payasset->set_dest_address(str_dest_address.c_str(), str_dest_address.size());

		bumoProtocol::Asset *lp_asset = lp_payasset->mutable_asset();
		lp_asset->set_amount(amount);

		bumoProtocol::AssetKey *lp_asset_key = lp_asset->mutable_key();
		lp_asset_key->set_issuer(str_issuer_address.c_str(), str_issuer_address.size());
		lp_asset_key->set_code(str_token_name.c_str(), str_token_name.size());

		tx.set_source_address(str_source_address.c_str(), str_source_address.size());
		tx.set_nonce(nonce);
		tx.set_fee_limit(fee_limit);
		tx.set_gas_price(gas_price);

		int len = tx.ByteSize();
		Binary sz_unsign_tx(len);
		tx.SerializeToArray(sz_unsign_tx.data(), len);
		std::string str_unsign_tx = Binary::encode(sz_unsign_tx);
		return str_unsign_tx;
	}

	std::string make_unsigned_tx_CTP_token(
		const std::string &str_source_address,
		const std::string &str_dest_address,
		uint64_t coin_amount,
		const std::string &str_contract_address,
		uint64_t token_amount,
		uint64_t nonce,
		uint64_t gas_price,
		uint64_t fee_limit
	) {
		if (str_source_address.size() != 36 ||
			str_dest_address.size() != 36 ||
			str_contract_address.size() != 36) {
			return std::string("");
		}

		nonce = get_nonce(str_source_address);
		//std::cout << nonce << std::endl;
		nonce++;

		gas_price = get_gas_price();
		//std::cout << gas_price << std::endl;

		bumoProtocol::Transaction tx;
		bumoProtocol::Operation *lp_operation = tx.add_operations();
		lp_operation->set_type(bumoProtocol::Operation_Type_PAY_COIN);
		lp_operation->set_source_address(str_source_address.c_str(), str_source_address.size());

		bumoProtocol::OperationPayCoin *lp_paycoin = lp_operation->mutable_pay_coin();
		lp_paycoin->set_amount(coin_amount);
		lp_paycoin->set_dest_address(str_contract_address.c_str(), str_contract_address.size());

		std::string str_token_amount = std::to_string(token_amount);
		std::string str_input = "{\"method\":\"transfer\",\"params\":{\"to\":\"";
		str_input += str_dest_address;
		str_input += "\",\"value\":\"";
		str_input += str_token_amount;
		str_input += "\"}}";

		lp_paycoin->set_input(str_input.c_str(), str_input.size());

		tx.set_source_address(str_source_address.c_str(), str_source_address.size());
		tx.set_nonce(nonce);
		tx.set_fee_limit(fee_limit);
		tx.set_gas_price(gas_price);

		int len = tx.ByteSize();
		Binary sz_unsign_tx(len);
		tx.SerializeToArray(sz_unsign_tx.data(), len);
		std::string str_unsign_tx = Binary::encode(sz_unsign_tx);
		return str_unsign_tx;
	}

	std::string tx_hash(const std::string& str_unsign_tx) {
		if ("" == str_unsign_tx) {
			return std::string("");
		}

		Binary sz_unsign_tx = Binary::decode(str_unsign_tx);
		Binary sz_sha256 = sha256_hash(sz_unsign_tx);
		std::string str_sha256 = Binary::encode(sz_sha256);
		return str_sha256;
	}

	std::string sign_tx(
		const std::string &str_unsign_tx,
		const std::string &str_prikey,
		const std::string &str_pubkey) {
		if ("" == str_unsign_tx ||
			str_prikey.size() != 64 ||
			str_pubkey.size() != 64) {
			return std::string("");
		}			

		Binary sz_unsign_tx = Binary::decode(str_unsign_tx);
		Binary sz_prikey = Binary::decode(str_prikey);
		Binary sz_pubkey = Binary::decode(str_pubkey);

		Binary sz_sign_prikey(64);
		ed25519_get_sign_private(sz_sign_prikey.data(), sz_prikey.data());
		//std::string str_sign_prikey = Binary::encode(sz_sign_prikey);
		//std::cout << str_sign_prikey << std::endl;

		Binary sz_sign(64);
		ed25519_sign(sz_sign.data(),
			sz_unsign_tx.data(), sz_unsign_tx.size(), 
			sz_pubkey.data(), sz_sign_prikey.data());
		std::string str_sign = Binary::encode(sz_sign);
		//std::cout << str_sign << std::endl;

		if (!ed25519_verify(sz_sign.data(), sz_unsign_tx.data(), sz_unsign_tx.size(), sz_pubkey.data())) {
			str_sign = "";
		}
		
		return str_sign;
	}

	std::string make_sign_tx(
		const std::string &str_unsign_tx,
		const std::string &str_sign_data,
		const std::string &str_pubkey) {
		if ("" == str_unsign_tx ||
			str_sign_data.size() != 128 ||
			str_pubkey.size() != 76) {
			return std::string("");
		}

		Json::Value jItems, jItem;		

		Json::Value jSignature, jSignItem;
		jSignItem["sign_data"] = str_sign_data;
		std::string str_bumo_pubkey = get_bumo_public_key(str_pubkey);
		jSignItem["public_key"] = str_bumo_pubkey;
		jSignature.append(jSignItem);

		jItem["signatures"] = jSignature;
		jItem["transaction_blob"] = str_unsign_tx;
		jItems.append(jItem);

		Json::Value jRoot;
		jRoot["items"] = jItems;

		std::string str_sign_tx = jRoot.toStyledString();
		return str_sign_tx;
	}

	bool broadcast_transaction_json(const std::string &str_tx_json) {
		return broadcast_tx_json(str_tx_json);
	}

}
