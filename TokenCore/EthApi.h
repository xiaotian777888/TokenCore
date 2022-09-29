#ifndef ETH_API_H
#define ETH_API_H

#include "TokenCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PB_BYTES_ARRAY_T(n) struct { uint32_t size; uint8_t bytes[n]; }

typedef PB_BYTES_ARRAY_T(32) EthereumSignTx_nonce_t;
typedef PB_BYTES_ARRAY_T(32) EthereumSignTx_gas_price_t;
typedef PB_BYTES_ARRAY_T(32) EthereumSignTx_gas_limit_t;
typedef PB_BYTES_ARRAY_T(20) EthereumSignTx_to_t;
typedef PB_BYTES_ARRAY_T(32) EthereumSignTx_value_t;
typedef PB_BYTES_ARRAY_T(1024) EthereumSignTx_data_t;
typedef PB_BYTES_ARRAY_T(32) EthereumSignTx_chain_id_str_t;
typedef PB_BYTES_ARRAY_T(2) EthereumSignTx_zero1_t;
typedef PB_BYTES_ARRAY_T(2) EthereumSignTx_zero2_t;

typedef struct _EthereumSignTx
{
	EthereumSignTx_nonce_t nonce;
	EthereumSignTx_gas_price_t gas_price;
	EthereumSignTx_gas_limit_t gas_limit;
	EthereumSignTx_to_t to;
	EthereumSignTx_value_t value;
	EthereumSignTx_data_t data;
	int32_t chain_id;
	EthereumSignTx_chain_id_str_t chain_id_str;
	EthereumSignTx_zero1_t zero1;
	EthereumSignTx_zero2_t zero2;
} EthereumSignTx;

typedef PB_BYTES_ARRAY_T(64) EthereumTxRequest_signature_v_t;
typedef PB_BYTES_ARRAY_T(64) EthereumTxRequest_signature_r_t;
typedef PB_BYTES_ARRAY_T(64) EthereumTxRequest_signature_s_t;
typedef PB_BYTES_ARRAY_T(32) EthereumTxRequest_hash_t;


typedef struct _EthereumTxRequest
{
	EthereumTxRequest_signature_v_t signature_v;
	EthereumTxRequest_signature_r_t signature_r;
	EthereumTxRequest_signature_s_t signature_s;
	EthereumTxRequest_hash_t hash;
} EthereumTxRequest;

typedef PB_BYTES_ARRAY_T(33) EncodeEthersumSignTx_nonce_t;
typedef PB_BYTES_ARRAY_T(33) EncodeEthersumSignTx_gas_price_t;
typedef PB_BYTES_ARRAY_T(33) EncodeEthersumSignTx_gas_limit_t;
typedef PB_BYTES_ARRAY_T(21) EncodeEthersumSignTx_to_t;
typedef PB_BYTES_ARRAY_T(33) EncodeEthersumSignTx_value_t;
typedef PB_BYTES_ARRAY_T(1025) EncodeEthersumSignTx_data_t;
typedef PB_BYTES_ARRAY_T(33) EncodeEthersumSignTx_signature_v_t;
typedef PB_BYTES_ARRAY_T(33) EncodeEthersumSignTx_signature_r_t;
typedef PB_BYTES_ARRAY_T(33) EncodeEthersumSignTx_signature_s_t;

typedef struct _EncodeEthereumSignTx
{
	EncodeEthersumSignTx_nonce_t nonce;
	EncodeEthersumSignTx_gas_price_t gas_price;
	EncodeEthersumSignTx_gas_limit_t gas_limit;
	EncodeEthersumSignTx_to_t to;
	EncodeEthersumSignTx_value_t value;
	EncodeEthersumSignTx_data_t data;
	EncodeEthersumSignTx_signature_v_t signature_v;
	EncodeEthersumSignTx_signature_r_t signature_r;
	EncodeEthersumSignTx_signature_s_t signature_s;
} EncodeEthereumSignTx;

#ifdef __cplusplus
	}
#endif

namespace ETHAPI
{
	bool validate_address(string address);
	string get_private_key(const string& seed);
	string get_public_key(const string& private_key);
	string get_address(const string& public_key);

	int make_unsign_tx(EthereumSignTx& tx, unsigned char coinType, const char* /*from*/, const char* to, const uint64_t nonce, const int chain_id, const char* contract_address, const u256& value, uint64_t gasLimit, uint64_t gasPrice);
	void firmware_prepare_data(EthereumSignTx& tx, unsigned  char* firmware_data, int& firmware_size, const Binary& custom_data);
	void firmware_process_result(EthereumSignTx& tx, char* result_data, int result_size, EthereumTxRequest& txReq);
	string make_sign_tx(EthereumSignTx& tx, const EthereumTxRequest& txReq);
	void sign_tx(EthereumSignTx& msg, const string& private_key, EthereumTxRequest& txReq);

    // add interface for make eth-unsigned-tx for non-transaction.
    int make_unsign_tx(EthereumSignTx& tx, const uint64_t nonce, const int chain_id, const char* to_address, const u256& value, uint64_t gasLimit, uint64_t gasPrice, const char* custom_data);
}

#endif //ETH_API_H
