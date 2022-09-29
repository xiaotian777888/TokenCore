#ifndef BUMO_API_H
#define BUMO_API_H

#include <string>

namespace BUMOAPI {
	bool validate_address(const std::string& str_address);
	std::string get_private_key();
	std::string get_bumo_private_key(const std::string& str_prikey);
	std::string get_public_key(const std::string& str_prikey);
	std::string get_bumo_public_key(const std::string& str_pubkey);
	std::string get_address(const std::string& str_pubkey);	

	bool broadcast_transaction_json(const std::string &str_tx_json);

	std::string make_unsigned_tx_bumo_coin(
		const std::string &str_source_address,
		const std::string &str_dest_address,
		uint64_t amount,
		uint64_t nonce,
		uint64_t gas_price,
		uint64_t fee_limit
	);

	std::string make_unsigned_tx_ATP_token(
		const std::string &str_source_address,
		const std::string &str_dest_address,
		const std::string &str_issuer_address,
		const std::string &str_token_name,
		uint64_t amount,
		uint64_t nonce,
		uint64_t gas_price,
		uint64_t fee_limit
	);

	std::string make_unsigned_tx_CTP_token(
		const std::string &str_source_address,
		const std::string &str_dest_address,
		uint64_t coin_amount,
		const std::string &str_contract_address,
		uint64_t token_amount,
		uint64_t nonce,
		uint64_t gas_price,
		uint64_t fee_limit
	);

	std::string tx_hash(const std::string& str_unsign_tx);

	std::string sign_tx(
		const std::string &str_unsign_tx, 
		const std::string &str_raw_prikey,
		const std::string &str_raw_pubkey);

	std::string make_sign_tx(
		const std::string &str_unsign_tx,
		const std::string &str_sign_data,
		const std::string &str_bumo_pubkey);

}


#endif // BUMO_API_H
