#include "EthApi.h"
#include "Eth/RLP.h"

#include <crypto/keccak256.h>
#include <cinttypes>
#include "crypto/bip.h"

extern "C"
{
#include <uEcc/macroECC.h>
}

#ifndef WIN32
    #ifndef strcpy_s
       #define strcpy_s(dest, size, src) strncpy((dest),(src),(size))
    #endif //strcpy_s
    #ifndef sprintf_s
       #define sprintf_s(str, size, fmt, ...) snprintf((str),(size),(fmt), __VA_ARGS__)
    #endif //sprintf_s
    #ifndef _strdup
       #define _strdup strdup
    #endif //_strdup
#endif //WIN32

namespace ETHAPI
{

static int wallet_ethereum_assemble_tx(EthereumSignTx *msg, const EthereumTxRequest *tx, uint64_t *rawTx)
{
	EncodeEthereumSignTx new_msg;
	memset(&new_msg, 0, sizeof(new_msg));

	wallet_encode_element(msg->nonce.bytes, msg->nonce.size, new_msg.nonce.bytes, &(new_msg.nonce.size), false);
	wallet_encode_element(msg->gas_price.bytes, msg->gas_price.size, new_msg.gas_price.bytes, &(new_msg.gas_price.size), false);
	wallet_encode_element(msg->gas_limit.bytes, msg->gas_limit.size, new_msg.gas_limit.bytes, &(new_msg.gas_limit.size), false);
	wallet_encode_element(msg->to.bytes, msg->to.size, new_msg.to.bytes, &(new_msg.to.size), false);
	wallet_encode_element(msg->value.bytes, msg->value.size, new_msg.value.bytes, &(new_msg.value.size), false);
	wallet_encode_element(msg->data.bytes, msg->data.size, new_msg.data.bytes, &(new_msg.data.size), false);
	wallet_encode_element(tx->signature_v.bytes, tx->signature_v.size, new_msg.signature_v.bytes, &(new_msg.signature_v.size), true);

	wallet_encode_element(tx->signature_r.bytes, tx->signature_r.size, new_msg.signature_r.bytes, &(new_msg.signature_r.size), true);
	wallet_encode_element(tx->signature_s.bytes, tx->signature_s.size, new_msg.signature_s.bytes, &(new_msg.signature_s.size), true);

	int length = wallet_encode_list(&new_msg, rawTx);
	return length;
}

bool validate_address(string address)
{
	if (address.length() != 42 || address[0] != '0' || address[1] != 'x')
		return false;

	auto is_base16 = [](const char c)
	{
		return ('0' <= c && c <= '9') || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f');
	};

	if (!all_of(address.begin() + 2, address.end(), is_base16))
		return false;

	return true;
}

string get_private_key(const string& seed)
{
	return bip44_get_private_key(seed, "m/44'/60'/0'/0/0");
}

// keyFromPrivate
string get_public_key(const string& private_key)
{
	Binary bin_private_key = Binary::decode(private_key);
	Binary bin_public_key(64);
	Binary bin_compressed(33);

	mECC_compute_public_key(bin_private_key.data(), bin_public_key.data());
	mECC_compress(bin_public_key.data(), bin_compressed.data());

	string public_key = Binary::encode(bin_compressed);
	return public_key;
}

string get_address(const string& public_key)
{
	Binary bin_compressed = Binary::decode(public_key);
	Binary bin_public_key(64);
	mECC_decompress(bin_compressed.data(), bin_public_key.data());

	Binary bin_hash(32);
	keccak_256(bin_public_key.data(), bin_public_key.size(), bin_hash.data());

	string str_address = Binary::encode(bin_hash.right(20));

	Binary bin_address_hash(32);
	keccak_256((const uint8_t *)str_address.c_str(), 40, bin_address_hash.data());

	for (int i = 0; i < 20; i++)
	{
		if ((bin_address_hash[i] & 0x80) && (str_address[i * 2] >= 'a') && (str_address[i * 2] <= 'f'))
			str_address[i * 2] -= 0x20;

		if ((bin_address_hash[i] & 0x08) && (str_address[i * 2 + 1] >= 'a') && (str_address[i * 2 + 1] <= 'f'))
			str_address[i * 2 + 1] -= 0x20;
	}

	return string("0x") + str_address;
}

int make_unsign_tx(EthereumSignTx& tx, unsigned char coinType, const char* /*from*/, const char* to, const uint64_t nonce, const int chain_id, const char* contract_address, const u256& value, uint64_t gasLimit, uint64_t gasPrice)
{
	char buffer[1024];

	// nonce
    if (nonce == 0) {
        tx.nonce.size = 0;
    }else{
        sprintf_s(buffer, 1024, "%" PRIxLEAST64, nonce);
        string nonceStr = buffer;
        tx.nonce.size = size_of_bytes((int)nonceStr.size());
        hex2byte_arr(nonceStr.c_str(), (int)nonceStr.size(), tx.nonce.bytes, tx.nonce.size);
    }
    
	// gas-price
    sprintf_s(buffer, 1024, "%" PRIxLEAST64, gasPrice);
	string gasPriceStr = buffer;
	tx.gas_price.size = size_of_bytes((int)gasPriceStr.size());
	hex2byte_arr(gasPriceStr.c_str(), (int)gasPriceStr.size(), tx.gas_price.bytes, tx.gas_price.size);

	// gas-limit
    sprintf_s(buffer, 1024, "%" PRIxLEAST64, gasLimit);
	string gasLimitStr = buffer;
	tx.gas_limit.size = size_of_bytes((int)gasLimitStr.size());
	hex2byte_arr(gasLimitStr.c_str(), (int)gasLimitStr.size(), tx.gas_limit.bytes, tx.gas_limit.size);

	if (coinType == 0)
	{
		// to
		string toStr(to+2);
		tx.to.size = size_of_bytes((int)toStr.size());
		hex2byte_arr(toStr.c_str(), (int)toStr.size(), tx.to.bytes, tx.to.size);

		// value
		string valueStr = value.hex_str();
		tx.value.size = size_of_bytes((int)valueStr.size());
		hex2byte_arr(valueStr.c_str(), (int)valueStr.size(), tx.value.bytes, tx.value.size);

		// data
		string dataStr("");
		tx.data.size = size_of_bytes((int)dataStr.size());
		hex2byte_arr(dataStr.c_str(), (int)dataStr.size(), tx.data.bytes, tx.data.size);
	}
	else
	{
		// "to" should be address of ERC20 smart contract.
		Binary bin_contract_address = Binary::decode(contract_address+2);
		tx.to.size = 20;
		memcpy(tx.to.bytes, bin_contract_address.data(), 20);

		// value: no value for ERC20.
		string valStr("");
		tx.value.size = size_of_bytes((int)valStr.size());
		hex2byte_arr(valStr.c_str(), (int)valStr.size(), tx.value.bytes, tx.value.size);

		// data: make ABI
		string valueHex = value.hex_str();
		string abi = string("a9059cbb""000000000000000000000000") + string(to+2) + string(64 - valueHex.length(), '0').append(valueHex);
		//QString valueDec= value.str(0, std::ios_base::dec).c_str();
		//encodeABI("", to, valueDec, token_type->symbol); // TODO: fix me
		if (abi.empty())
			return -1;

		string dataStr = abi;
		tx.data.size = size_of_bytes((int)dataStr.size());
		hex2byte_arr(dataStr.c_str(), (int)dataStr.size(), tx.data.bytes, tx.data.size);
	}

	// chain-id
	tx.chain_id = chain_id;
	sprintf_s(buffer, 1024, "%x", tx.chain_id);
	string chainIdStr = buffer;
	tx.chain_id_str.size = size_of_bytes((int)chainIdStr.size());
	hex2byte_arr(chainIdStr.c_str(), (int)chainIdStr.size(), tx.chain_id_str.bytes, tx.chain_id_str.size);

	const char* zero1 = "";
	const char* zero2 = "";

	tx.zero1.size = size_of_bytes((int)strlen(zero1));
	hex2byte_arr(zero1, (int)strlen(zero1), tx.zero1.bytes, tx.zero1.size);

	tx.zero2.size = size_of_bytes((int)strlen(zero2));
	hex2byte_arr(zero2, (int)strlen(zero2), tx.zero2.bytes, tx.zero1.size);

	return 0;
}

int make_unsign_tx(EthereumSignTx& tx, const uint64_t nonce, const int chain_id, const char* to_address, const u256& value, uint64_t gasLimit, uint64_t gasPrice, const char* custom_data)
{
    char buffer[1024];

    // nonce
    sprintf_s(buffer, 1024, "%" PRIxLEAST64, nonce);
    string nonceStr = buffer;
    tx.nonce.size = size_of_bytes((int)nonceStr.size());
    hex2byte_arr(nonceStr.c_str(), (int)nonceStr.size(), tx.nonce.bytes, tx.nonce.size);

    // gas-price
    sprintf_s(buffer, 1024, "%" PRIxLEAST64, gasPrice);
    string gasPriceStr = buffer;
    tx.gas_price.size = size_of_bytes((int)gasPriceStr.size());
    hex2byte_arr(gasPriceStr.c_str(), (int)gasPriceStr.size(), tx.gas_price.bytes, tx.gas_price.size);

    // gas-limit
    sprintf_s(buffer, 1024, "%" PRIxLEAST64, gasLimit);
    string gasLimitStr = buffer;
    tx.gas_limit.size = size_of_bytes((int)gasLimitStr.size());
    hex2byte_arr(gasLimitStr.c_str(), (int)gasLimitStr.size(), tx.gas_limit.bytes, tx.gas_limit.size);

//     if (coinType == 0) {
//         // to
//         string toStr(to + 2);
//         tx.to.size = size_of_bytes((int)toStr.size());
//         hex2byte_arr(toStr.c_str(), (int)toStr.size(), tx.to.bytes, tx.to.size);
// 
//         // value
//         string valueStr = value.hex_str();
//         tx.value.size = size_of_bytes((int)valueStr.size());
//         hex2byte_arr(valueStr.c_str(), (int)valueStr.size(), tx.value.bytes, tx.value.size);
// 
//         // data
//         string dataStr("");
//         tx.data.size = size_of_bytes((int)dataStr.size());
//         hex2byte_arr(dataStr.c_str(), (int)dataStr.size(), tx.data.bytes, tx.data.size);
//     }
//     else {
        // "to" should be address of ERC20 smart contract.
        Binary bin_contract_address = Binary::decode(to_address + 2);
        tx.to.size = 20;
        memcpy(tx.to.bytes, bin_contract_address.data(), 20);

//         // value: no value for ERC20.
//         string valStr("");
//         tx.value.size = size_of_bytes((int)valStr.size());
//         hex2byte_arr(valStr.c_str(), (int)valStr.size(), tx.value.bytes, tx.value.size);
        // value
        string valueStr = value.hex_str();
        tx.value.size = size_of_bytes((int)valueStr.size());
        hex2byte_arr(valueStr.c_str(), (int)valueStr.size(), tx.value.bytes, tx.value.size);

        // data: make ABI
//         string valueHex = value.hex_str();
//         string abi = string("a9059cbb""000000000000000000000000") + string(to + 2) + string(64 - valueHex.length(), '0').append(valueHex);
//         //QString valueDec= value.str(0, std::ios_base::dec).c_str();
//         //encodeABI("", to, valueDec, token_type->symbol); // TODO: fix me
//         if (abi.empty())
//             return -1;

        string dataStr(custom_data);// abi;
        tx.data.size = size_of_bytes((int)dataStr.size());
        hex2byte_arr(dataStr.c_str(), (int)dataStr.size(), tx.data.bytes, tx.data.size);
//     }

    // chain-id
    tx.chain_id = chain_id;
    sprintf_s(buffer, 1024, "%x", tx.chain_id);
    string chainIdStr = buffer;
    tx.chain_id_str.size = size_of_bytes((int)chainIdStr.size());
    hex2byte_arr(chainIdStr.c_str(), (int)chainIdStr.size(), tx.chain_id_str.bytes, tx.chain_id_str.size);

    const char* zero1 = "";
    const char* zero2 = "";

    tx.zero1.size = size_of_bytes((int)strlen(zero1));
    hex2byte_arr(zero1, (int)strlen(zero1), tx.zero1.bytes, tx.zero1.size);

    tx.zero2.size = size_of_bytes((int)strlen(zero2));
    hex2byte_arr(zero2, (int)strlen(zero2), tx.zero2.bytes, tx.zero1.size);

    return 0;
}

// firmware2
void firmware_prepare_data(EthereumSignTx& tx, unsigned char* firmware_data, int& firmware_size, const Binary& custom_data)
{
	firmware_size = 0;

	// nonce
	firmware_data[firmware_size] = tx.nonce.size;
	firmware_size++;
	if (tx.nonce.size > 0)
	{
		memcpy(firmware_data + firmware_size, tx.nonce.bytes, tx.nonce.size);
		firmware_size += tx.nonce.size;
	}

	// gas-price
	firmware_data[firmware_size] = tx.gas_price.size;
	firmware_size++;
	if (tx.gas_price.size > 0)
	{
		memcpy(firmware_data + firmware_size, tx.gas_price.bytes, tx.gas_price.size);
		firmware_size += tx.gas_price.size;
	}

	// gas-limit
	firmware_data[firmware_size] = tx.gas_limit.size;
	firmware_size++;
	if (tx.gas_limit.size > 0)
	{
		memcpy(firmware_data + firmware_size, tx.gas_limit.bytes, tx.gas_limit.size);
		firmware_size += tx.gas_limit.size;
	}

	// to
	firmware_data[firmware_size] = tx.to.size;
	firmware_size++;
	if (tx.to.size > 0)
	{
		memcpy(firmware_data + firmware_size, tx.to.bytes, tx.to.size);
		firmware_size += tx.to.size;
	}

	// value
	firmware_data[firmware_size] = tx.value.size;
	firmware_size++;
	if (tx.value.size > 0)
	{
		memcpy(firmware_data + firmware_size, tx.value.bytes, tx.value.size);
		firmware_size += tx.value.size;
	}

	// data
	if (custom_data.size() != 0)
	{
		uint32_t data_size = (uint32_t)custom_data.size();
		tx.data.size = data_size;
		memcpy(tx.data.bytes, custom_data.data(), data_size);
	}

	const int data_size = tx.data.size;
	firmware_data[firmware_size] = ((data_size >> 8) & 0xff);
	firmware_size++;
	firmware_data[firmware_size] = (data_size & 0xff);
	firmware_size++;

	if (data_size > 0)
	{
		memcpy(firmware_data + firmware_size, tx.data.bytes, tx.data.size);
		firmware_size += data_size;
	}

	// chain-id
	firmware_data[firmware_size] = tx.chain_id_str.size;
	firmware_size++;
	if (tx.chain_id_str.size > 0)
	{
		memcpy(firmware_data + firmware_size, tx.chain_id_str.bytes, tx.chain_id_str.size);
		firmware_size += tx.chain_id_str.size;
	}

	firmware_data[firmware_size] = tx.zero1.size;
	firmware_size++;
	if (tx.zero1.size > 0)
	{
		memcpy(firmware_data + firmware_size, tx.zero1.bytes, tx.zero1.size);
		firmware_size += tx.zero1.size;
	}

	firmware_data[firmware_size] = tx.zero2.size;
	firmware_size++;
	if (tx.zero2.size > 0)
	{
		memcpy(firmware_data + firmware_size, tx.zero2.bytes, tx.zero2.size);
		firmware_size += tx.zero2.size;
	}
}

// firmware3
void firmware_process_result(EthereumSignTx& tx, char* result_data, int result_size, EthereumTxRequest& txReq)
{
	memset(&txReq, 0, sizeof(EthereumTxRequest));
	txReq.signature_r.size = 32;
	memcpy(txReq.signature_r.bytes, result_data, 32);
	txReq.signature_s.size = 32;
	memcpy(txReq.signature_s.bytes, result_data + 32, 32);

	unsigned long hw_v = (unsigned long)result_data[64];
	unsigned long v = 0;
	if (tx.chain_id)
		v = hw_v + 2 * tx.chain_id + 35;
	else
		v = hw_v + 27;

	txReq.signature_v.size = 1; //FIXME
	*((unsigned long*)txReq.signature_v.bytes) = v;
}

// post_process_sign
string make_sign_tx(EthereumSignTx& tx, const EthereumTxRequest& txReq)
{
	Binary bin_tx(96 * sizeof(uint64_t));

	int length = wallet_ethereum_assemble_tx(&tx, &txReq, (uint64_t*)bin_tx.data());
	bin_tx.resize(length);

	string str_tx = "0x";
	str_tx = str_tx + Binary::encode(bin_tx);
	return str_tx;
}

// Calculate the number of bytes needed for an RLP length header.
// NOTE: supports up to 16MB of data (how unlikely...)
// FIXME: improve
 static int rlp_calculate_length(int length, uint8_t firstbyte)
{
	if (length == 1 && firstbyte <= 0x7f)
		return 1;
	else if (length <= 55)
		return 1 + length;
	else if (length <= 0xff)
		return 2 + length;
	else if (length <= 0xffff)
		return 3 + length;
	else
		return 4 + length;
}

// Push an RLP encoded length to the hash buffer.
static void hash_rlp_length(struct SHA3_CTX& keccak_ctx, uint32_t length, uint8_t firstbyte)
{
	uint8_t buf[4];

	if (length == 1 && firstbyte <= 0x7f)
	{
		// empty length header
	}
	else if (length <= 55)
	{
		buf[0] = 0x80 + length;
		sha3_Update(&keccak_ctx, buf, 1);
	}
	else if (length <= 0xff)
	{
		buf[0] = 0xb7 + 1;
		buf[1] = length;
		sha3_Update(&keccak_ctx, buf, 2);
	}
	else if (length <= 0xffff)
	{
		buf[0] = 0xb7 + 2;
		buf[1] = length >> 8;
		buf[2] = length & 0xff;
		sha3_Update(&keccak_ctx, buf, 3);
	}
	else
	{
		buf[0] = 0xb7 + 3;
		buf[1] = length >> 16;
		buf[2] = length >> 8;
		buf[3] = length & 0xff;
		sha3_Update(&keccak_ctx, buf, 4);
	}
}

// Push an RLP encoded list length to the hash buffer.
static void hash_rlp_list_length(struct SHA3_CTX& keccak_ctx, uint32_t length)
{
	uint8_t buf[4];

	if (length <= 55)
	{
		buf[0] = 0xc0 + length;
		sha3_Update(&keccak_ctx, buf, 1);
	}
	else if (length <= 0xff)
	{
		buf[0] = 0xf7 + 1;
		buf[1] = length;
		sha3_Update(&keccak_ctx, buf, 2);
	}
	else if (length <= 0xffff)
	{
		buf[0] = 0xf7 + 2;
		buf[1] = length >> 8;
		buf[2] = length & 0xff;
		sha3_Update(&keccak_ctx, buf, 3);
	}
	else
	{
		buf[0] = 0xf7 + 3;
		buf[1] = length >> 16;
		buf[2] = length >> 8;
		buf[3] = length & 0xff;
		sha3_Update(&keccak_ctx, buf, 4);
	}
}

// Push an RLP encoded length field and data to the hash buffer.
static void hash_rlp_field(struct SHA3_CTX& keccak_ctx, const uint8_t *buf, uint32_t size)
{
	hash_rlp_length(keccak_ctx, size, buf[0]);
	sha3_Update(&keccak_ctx, buf, size);
}

void sign_tx(EthereumSignTx& msg, const string& private_key, EthereumTxRequest& txReq)
{
	struct SHA3_CTX keccak_ctx;
	
	sha3_256_Init(&keccak_ctx);
	memset(&txReq, 0, sizeof(EthereumTxRequest));

	/* Stage 1: Calculate total RLP length */
	uint32_t rlp_length = 0;

	rlp_length += rlp_calculate_length(msg.nonce.size, msg.nonce.bytes[0]);
	rlp_length += rlp_calculate_length(msg.gas_price.size, msg.gas_price.bytes[0]);
	rlp_length += rlp_calculate_length(msg.gas_limit.size, msg.gas_limit.bytes[0]);
	rlp_length += rlp_calculate_length(msg.to.size, msg.to.bytes[0]);
	rlp_length += rlp_calculate_length(msg.value.size, msg.value.bytes[0]);
	rlp_length += rlp_calculate_length(msg.data.size, msg.data.bytes[0]);
	rlp_length += rlp_calculate_length(msg.chain_id_str.size, msg.chain_id_str.bytes[0]);
	rlp_length += rlp_calculate_length(msg.zero1.size, msg.zero1.bytes[0]);
	rlp_length += rlp_calculate_length(msg.zero2.size, msg.zero2.bytes[0]);

	/* Stage 2: Store header fields */
	hash_rlp_list_length(keccak_ctx, rlp_length);

	hash_rlp_field(keccak_ctx, msg.nonce.bytes, msg.nonce.size);
	hash_rlp_field(keccak_ctx, msg.gas_price.bytes, msg.gas_price.size);
	hash_rlp_field(keccak_ctx, msg.gas_limit.bytes, msg.gas_limit.size);
	hash_rlp_field(keccak_ctx, msg.to.bytes, msg.to.size);
	hash_rlp_field(keccak_ctx, msg.value.bytes, msg.value.size);
	hash_rlp_field(keccak_ctx, msg.data.bytes, msg.data.size);
	hash_rlp_field(keccak_ctx, msg.chain_id_str.bytes, msg.chain_id_str.size);
	hash_rlp_field(keccak_ctx, msg.zero1.bytes, msg.zero1.size);
	hash_rlp_field(keccak_ctx, msg.zero2.bytes, msg.zero2.size);

	Binary bin_private_key = Binary::decode(private_key);

	uint8_t myHash[32], sig[64];
	keccak_Final(&keccak_ctx, myHash);

	int recId = mECC_sign_forbc(bin_private_key.data(), myHash, sig);
	if (recId == -1)
		return;

	uint8_t deltaV = (uint8_t)recId;

	int v = deltaV + 2 * msg.chain_id + 35;
	Binary bin_v;
	bin_v.push_back((unsigned char)v);
	string str_v = Binary::encode(bin_v);

	txReq.signature_v.size = size_of_bytes((int)str_v.size());
	hex2byte_arr(str_v.c_str(), (int)str_v.size(), txReq.signature_v.bytes, txReq.signature_v.size);

	txReq.signature_r.size = 32;
	memcpy(txReq.signature_r.bytes, sig, 32);

	txReq.signature_s.size = 32;
	memcpy(txReq.signature_s.bytes, sig + 32, 32);

	txReq.hash.size = 32;
	memcpy(txReq.hash.bytes, myHash, txReq.hash.size);
}

}
