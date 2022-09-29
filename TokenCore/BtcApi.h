#ifndef BTCBASE_H_
#define BTCBASE_H_

#include "TokenCommon.h"

namespace BTCAPI
{
    bool validate_address(string address);										// 验证 BTC 地址的有效性
    string get_private_key(bool is_testnet, const string& seed);				// 从种子生成私钥
    string get_public_key(const string& private_key, bool compress = true);		// 从私钥生成公钥
    string get_address(bool is_testnet, const string& public_key);				// 转换公钥为收付款地址
    string tx_hash(const string& tx_str);										// 获取交易串 hash
    void dump_tx(bool is_testnet, const string tx_str);							// 输出tx的解析结果

    int make_unsign_tx(UserTransaction* ut);
    Binary firmware_prepare_data(bool is_testnet, UserTransaction* ut, int script_index);
    void firmware_process_result(UserTransaction* ut, int index, char* result, int result_size);
    void sign_tx(bool is_testnet, UserTransaction* ut, const string& private_key);
    void make_sign_tx(bool is_testnet, UserTransaction* ut);

    string decode_script(const string& script_str);								// 解码脚本
    string encode_script(const string& script_str);								// 编码脚本
    int get_tx_len(UserTransaction* ut);										// 采用模拟签名的方式获得交易串长度(必须已获取UTXO)
}

namespace USDTAPI
{
    int make_unsign_tx(UserTransaction* ut);
    Binary firmware_prepare_data(bool is_testnet, UserTransaction* ut, int script_index);
    int get_tx_len(UserTransaction* ut);										// 采用模拟签名的方式获得交易串长度(必须已获取UTXO)
    u256 get_usdt_from_tx(bool is_testnet, const string str_tx);				// 从一个交易串里面获得usdt转账金额
}

#endif // BTCBASE_H_
