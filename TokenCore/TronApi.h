#ifndef TRON_API_H
#define TRON_API_H

#include "TokenCommon.h"

namespace TRONAPI
{
	bool validate_address(const string& str_address);
	string get_private_key(const string& seed);
	string get_public_key(const string str_prikey, bool is_compress = true);
	string get_address(const string &str_pubkey_nocompress, bool is_mainnet = true);
	string decompress_pub_key(const string& str_compressed_pub_key);

	string make_unsigned_tx_trc10(
		const string &str_token_name,
		const string &str_block_id,
		const string &str_from_address,
		const string &str_to_address,
		uint64_t u64_amount
	);

	string make_unsigned_tx_trc20(
		const string &str_block_id,
		const string &str_contract_address,
		const string &str_from_address,
		const string &str_to_address,
		uint64_t u64_amount
	);

	string make_unsigned_tx_from_json(const string &str_unsined_tx_json);
	string make_unsigned_tx_trc20_from_json(const string &str_unsign_tx_trc20_json);
	string make_unsigned_tx_trx_from_json(const string &str_unsigned_tx_trx_json);
	string make_unsigned_tx_freeze_from_json(const string &str_unsigned_tx_freeze_json);
	string make_unsigned_tx_unfreeze_from_json(const string &str_unsigned_tx_unfreeze_json);

	string tx_hash(const string& str_unsign_tx);

	string sign_tx(const string& str_unsign_tx, const string& str_prikey);
	string sign_message(const bool use_trx_header, const string &str_transaction , const string &str_prikey);
	string make_sign_tx(const string& str_unsign_tx, const string& str_sign_data);

	string trx10_to_json(const string &str_token_name, const string &str_sign_tx);
	string trx20_to_json(const string &str_sign_tx);
}

#define _in
#define _out
#define _inout

namespace TRX_API {
    typedef void *TRX_TX;

    int TxCreate(
            _in const char *json_param,
            _out TRX_TX **tx
    );

	string GetUnsignedTx(
            _in TRX_TX *tx
    );

    int TxSign(
            _in TRX_TX *tx,
            _in const char *json_param
    );
	int TxSetSignature(_in TRX_TX *tx, _in string signature);

    // use TxFree() to release resource of `result`.
    int TxGetTransacton(
            _in const TRX_TX *tx,
            _in const char *json_param,
            _out char **result
    );

    void TxDestory(_in TRX_TX** tx);

    int TxGetPriKey(
            _in const char *json_param,
            _out char** result
    );

    int TxGetPubKey(
            _in const char *json_param,
            _out char** result
    );

    // 根据公钥计算出地址
    // pubkey可以是压缩格式(33字节)或非压缩格式(65字节)
    // use TxFree() to release resource of `result`.
    int TxGetAddress(
            _in const char *json_param,
            _out char** result
    );

    string MakeSignedTx(
			_in const TRX_TX *tx,
			_in const string& str_sign_data
    );
}

#endif // TRON_API_H
