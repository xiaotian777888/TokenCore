#ifndef SIGN_H
#define SIGN_H

#include <stdbool.h>
#include <stdint.h>
#include "crypto/util.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PB_BYTES_ARRAY_T(n) struct { pb_size_t size; pb_byte_t bytes[n]; }

typedef PB_BYTES_ARRAY_T(32) EthereumSignTx_nonce_t;
typedef PB_BYTES_ARRAY_T(32) EthereumSignTx_gas_price_t;
typedef PB_BYTES_ARRAY_T(32) EthereumSignTx_gas_limit_t;
typedef PB_BYTES_ARRAY_T(20) EthereumSignTx_to_t;
typedef PB_BYTES_ARRAY_T(32) EthereumSignTx_value_t;
typedef PB_BYTES_ARRAY_T(1024) EthereumSignTx_data_t;
typedef PB_BYTES_ARRAY_T(32) EthereumSignTx_chain_id_str_t;
typedef PB_BYTES_ARRAY_T(2) EthereumSignTx_zero1_t;
typedef PB_BYTES_ARRAY_T(2) EthereumSignTx_zero2_t;

typedef struct _EthereumSignTx {
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


typedef struct _EthereumTxRequest {
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

typedef struct _EncodeEthereumSignTx {
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
#endif // SIGN_H
