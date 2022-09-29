#include <iostream>
#include <string>
#include <vector>
#include <iterator>

#include "BtcApi.h"
#include "uEcc/uECC.h"
#include <uEcc/macroECC.h>

#include "crypto/base58.h"
#include "crypto/utility_tools.h"
#include "crypto/hmac_sha512.h"
#include "crypto/hash.h"
#include "Btc/script.h"
#include "Usdt/createpayload.h"

// ===================================================================

struct TransactionInput
{
    string address;
    string previous_output_hash;
    uint32_t previous_output_index;
    string script;
    uint32_t sequence;
};

struct TransactionOutput
{
    string address;
    string script;
    uint64_t value;
};

struct TransactionBill
{
    string hash;
    uint32_t locktime;
    uint32_t version;
    vector<TransactionInput> inputs;
    vector<TransactionOutput> outputs;
};

void tx_decode(bool is_testnet, const string tx_str, TransactionBill& tb)
{
    Binary hash = bitcoin256(Binary::decode(tx_str));
    hash.reverse();
    tb.hash = Binary::encode(hash);

    size_t index = 0;
    tb.version = little_endian_to_uint32_t(tx_str.substr(index, 8));
    index += 8;

    string str_input_count = tx_str.substr(index, 2);
    size_t input_count = (size_t)(string_to_uint8_t(str_input_count));
    index += 2;
    for (size_t i = 0; i < input_count; i++)
    {
        TransactionInput in;
        in.previous_output_hash = reverse_big_little_endian(tx_str.substr(index, 64));
        index += 64;
        in.previous_output_index = little_endian_to_uint32_t(tx_str.substr(index, 8));
        index += 8;
        string str_script_len = tx_str.substr(index, 2);
        index += 2;
        int unlock_script_len = string_to_uint8_t(str_script_len);
        if (0 != unlock_script_len)
        {
            in.script = tx_str.substr(index, unlock_script_len * 2);
            index += unlock_script_len * 2;
        }
        in.sequence = little_endian_to_uint32_t(tx_str.substr(index, 8));
        index += 8;

        tb.inputs.push_back(in);
    }

    string str_output_count = tx_str.substr(index, 2);
    size_t output_count = (size_t)(string_to_uint8_t(str_output_count));
    index += 2;
    for (size_t i = 0; i < output_count; i++)
    {
        TransactionOutput out;
        out.value = little_endian_to_uint64_t(tx_str.substr(index, 16));
        index += 16;
        string str_pubkey_script_len = tx_str.substr(index, 2);
        index += 2;
        uint8_t u8_pubkey_script_len = string_to_uint8_t(str_pubkey_script_len);
        out.script = tx_str.substr(index, u8_pubkey_script_len * 2);
        index += u8_pubkey_script_len * 2;

        size_t pos = out.script.find("76a914");
        if (-1 != pos)
        {
            string str_pubkey_hash = out.script.substr(pos + 6, 0x14 * 2);
            if (is_testnet)
                out.address = encode_base58check(Binary::decode(str_pubkey_hash), 0x6f);
            else
                out.address = encode_base58check(Binary::decode(str_pubkey_hash), 0x00);
        }

        tb.outputs.push_back(out);
    }

    tb.locktime = little_endian_to_uint32_t(tx_str.substr(index, 8));
}

string tx_encode(const TransactionBill& tb)
{
    string str_version = uint32_t_to_little_endian(tb.version);
    string str_encode = str_version;

    str_encode += uint8_t_to_string((uint8_t)(tb.inputs.size()));
    for (size_t i = 0; i < tb.inputs.size(); i++)
    {
        str_encode += reverse_big_little_endian(tb.inputs[i].previous_output_hash);
        str_encode += uint32_t_to_little_endian(tb.inputs[i].previous_output_index);

        if ("" == tb.inputs[i].script)
            str_encode += "00";
        else
        {
            uint8_t u8_sign_script_len = (uint8_t)tb.inputs[i].script.size();
            string str_sign_script_len = uint8_t_to_string(u8_sign_script_len / 2);
            str_encode += str_sign_script_len;
            str_encode += tb.inputs[i].script;
        }

        str_encode += uint32_t_to_little_endian(tb.inputs[i].sequence);
    }

    str_encode += uint8_t_to_string((uint8_t)(tb.outputs.size()));

    for (size_t i = 0; i < tb.outputs.size(); i++)
    {
        str_encode = str_encode + uint64_t_to_little_endian(tb.outputs[i].value);
        uint8_t u8_pubkey_script_len = (uint8_t)tb.outputs[i].script.size();
        string str_pubkey_script_len = uint8_t_to_string(u8_pubkey_script_len / 2);
        str_encode += str_pubkey_script_len;
        str_encode += tb.outputs[i].script;
    }

    str_encode += uint32_t_to_little_endian(tb.locktime);

    return str_encode;
}

namespace BTCAPI
{

    bool validate_address(string address)
    {
        if (address.size() != 34)
            return false;
        if (address[0] != '1' && address[0] != '3' && address[0] != 'm' && address[0] != 'n')
            return false;

        Binary bin_address = decode_base58(address);
        Binary checkdata = bin_address.left(bin_address.size() - 4);
        Binary checksum_verify = bitcoin256(checkdata);

        if (checksum_verify.left(4) != bin_address.right(4))
            return false;

        return true;
    }

/*
这个废弃了
// 原 ec_new，返回 private_key
string get_private_key(const string& seed)
{
	if (0 != seed.size() % 2)
		return string("");
	Binary bin_seed = Binary::decode(seed);
	Binary key("Bitcoin seed");
	Binary hash(64);
	HMACSHA512(bin_seed.data(), bin_seed.size(), key.data(), key.size(), hash.data());
	string str_hash = Binary::encode(hash);
	str_hash = str_hash.substr(0, 64);
	return str_hash;
}
*/

    string get_private_key(bool is_testnet, const string& seed)
    {
        if (!is_testnet)
            return bip44_get_private_key(seed, "m/44'/0'/0'/0/0");
        else
            return bip44_get_private_key(seed, "m/44'/1'/0'/0/0");
    }

// 原 ec_to_public，返回 public_key
    string get_public_key(const string& private_key, bool compress)
    {
        string public_key;
        int compress_flag = 0;	// 0:compress, 1:uncompress

        if (!compress)
        {
            if (66 == private_key.size() && "01" == private_key.substr(private_key.size() - 2))
                compress_flag = 0;
            else if (64 == private_key.size())
                compress_flag = 1;
            else
                return public_key;	// private key error, return
        }

        Binary bin_private_key(32);
        bin_private_key = Binary::decode(private_key);

        Binary bin_public_key(64);
        const struct uECC_Curve_t * curve = uECC_secp256k1();

        if (!uECC_compute_public_key(bin_private_key.data(), bin_public_key.data(), curve))
            return public_key;

        if (0 == compress_flag)
        {
            Binary bin_public_key_compress(33);
            uECC_compress(bin_public_key.data(), bin_public_key_compress.data(), curve);
            public_key = Binary::encode(bin_public_key_compress);
        }
        else if (1 == compress_flag)
        {
            public_key = Binary::encode(bin_public_key);
        }

        return public_key;
    }

// 原 ec_to_address，返回 address
    string get_address(bool is_testnet, const string& public_key)
    {
        // const uint8_t mainnet_p2kh = 0x00;
        // const uint8_t testnet_p2kh = 0x6f;
        uint8_t version = 0x00;
        if (is_testnet)
            version = 0x6f;

        Binary hash = bitcoin160(Binary::decode(public_key));
        return encode_base58check(hash, version);
    }

    string decode_script(const string& script_str)
    {
        return decode_script_(script_str);
    }

    string encode_script(const string& script_str)
    {
        return encode_script_(script_str);
    }

    string tx_hash(const string& tx_str)
    {
        return Binary::encode(bitcoin256(Binary::decode(tx_str)));
    }

    string signature(const string& str_prikey, string& str_hash)
    {
        if (str_prikey.size() != 64 || str_hash.size() != 64)
            return string("");

        Binary hash = Binary::decode(str_hash);
        Binary prikey = Binary::decode(str_prikey);
        const struct uECC_Curve_t * curve = uECC_secp256k1();

        Binary sign(64);
        int v = mECC_sign_forbc(prikey.data(), hash.data(), sign.data());

        return Binary::encode(sign);
    }

// 序列化
    string sig_serialize(const string& sig)
    {
        Binary bin_sig = Binary::decode(sig);
        Binary ar = bin_sig.left(32);
        Binary as = bin_sig.right(32);

        Binary r, s;
        r.push_back(0);
        r += ar;
        s.push_back(0);
        s += as;

        unsigned char *rp = r.data(), *sp = s.data();
        size_t lenR = r.size(), lenS = s.size();

        while (lenR > 1 && rp[0] == 0 && rp[1] < 0x80)
        {
            lenR--; rp++;
        }
        while (lenS > 1 && sp[0] == 0 && sp[1] < 0x80)
        {
            lenS--; sp++;
        }

        Binary output;
        output.push_back(0x30);
        output.push_back((unsigned char)(4 + lenS + lenR));
        output.push_back(0x02);
        output.push_back((unsigned char)lenR);
        output += Binary(rp, lenR);
        output.push_back(0x02);
        output.push_back((unsigned char)lenS);
        output += Binary(sp, lenS);

        return Binary::encode(output);
    }

    string sign_input(bool is_testnet, const string& str_tx, const int input_index, const string& input_script, const string& private_key)
    {
        TransactionBill tx;
        tx_decode(is_testnet, str_tx, tx);
        tx.inputs[input_index].script = input_script;

        uint32_t u32_sign_type = 1;
        string str_script_tx = tx_encode(tx) + uint32_t_to_little_endian(u32_sign_type);
        string str_hash = Binary::encode(bitcoin256(Binary::decode(str_script_tx)));
        string str_sign = signature(private_key, str_hash);
        str_sign = sig_serialize(str_sign) + "01";

        return str_sign;
    }

    void dump_tx(bool is_testnet, const string tx_str)
    {
        TransactionBill tb;
        tx_decode(is_testnet, tx_str, tb);

        printf("transaction\n{\n");
        printf("\thash %s\n", tb.hash.c_str());
        printf("\tinputs\n\t{\n");

        for (size_t i = 0; i < tb.inputs.size(); i++)
        {
            printf("\t\tinput\n");
            printf("\t\t{\n");

            printf("\t\t\tprevious_output\n");
            printf("\t\t\t{\n");

            printf("\t\t\t\thash %s\n", tb.inputs[i].previous_output_hash.c_str());
            printf("\t\t\t\tindex %u\n", tb.inputs[i].previous_output_index);

            printf("\t\t\t}\n");
            // printf("\t\t\tscript %s\n", tb.inputs[i].script.c_str());
            string str_script = tb.inputs[i].script;
            string str_sign, str_pubkey;
            if ("" != str_script)
            {
                size_t script_index = 0;
                uint8_t u8_sign_len = string_to_uint8_t(str_script.substr(0, 2));
                script_index += 2;
                str_sign = str_script.substr(script_index, u8_sign_len * 2);
                script_index += u8_sign_len * 2;
                uint8_t u8_pubkey_len = string_to_uint8_t(str_script.substr(script_index, 2));
                script_index += 2;
                str_pubkey = str_script.substr(script_index, u8_pubkey_len * 2);
                script_index += u8_pubkey_len * 2;
            }
            printf("\t\t\tscript \"[%s] [%s]\"\n", str_sign.c_str(), str_pubkey.c_str());

            printf("\t\t\tsequence %08X\n", tb.inputs[i].sequence);

            printf("\t\t}\n");
        }
        printf("\t}\n");

        printf("\tlock_time %u\n", tb.locktime);
        printf("\toutputs\n\t{\n");
        for (size_t i = 0; i < tb.outputs.size(); i++)
        {
            printf("\t\toutput\n");
            printf("\t\t{\n");
            if ("" != tb.outputs[i].address)
                printf("\t\t\taddress %s\n", tb.outputs[i].address.c_str());
            printf("\t\t\tscript \"%s\"\n", decode_script_(tb.outputs[i].script).c_str());
            printf("\t\t\tvalue %I64u\n", tb.outputs[i].value);
            printf("\t\t}\n");
        }
        printf("\t}\n");
        printf("\tversion %u\n", tb.version);

        printf("}\n");
    }

/*
int tx_len(const int inputs, const int outputs)
{
	int len = 9 + inputs * 147 + outputs * 35;
	return len;
}
int firmware_tx_len(UserTransaction* ut)
{
	size_t i;
	u256 value_sum = 0;
	vector<Utxo> utxo_list;
	u256 fee = ut->fee_count * ut->fee_price;
	for (i = 0; i < ut->utxo_list.size(); i++)
	{
		value_sum += ut->utxo_list[i].value;
		if (value_sum >= (ut->pay + fee))
			break;
	}
	int o = 1;
	if (!ut->change_address.empty())
		o = 2;
	size_t len = 9 + ((i + 1) * 147) + (o * 35);
	return (int)len;
}
*/

// 采用模拟签名的方式获得交易串长度
    int get_tx_len(UserTransaction* ut)
    {
        UserTransaction fx;
        fx.from_address = "16TEzNLEMX5dpm18gGbzZ6X9cFoqgUPuMk";
        fx.to_address = "15KJzn2AzrL8hkyfvrGWf2qKpEyR5U8u3Z";
        fx.change_address = "16TEzNLEMX5dpm18gGbzZ6X9cFoqgUPuMk";
        fx.pay = ut->pay;
        fx.fee_count = 0;
        fx.fee_price = 0;
        fx.utxo_list = ut->utxo_list;

        int retcode = make_unsign_tx(&fx);
        if (retcode != 0)
            return(retcode);

        sign_tx(false, &fx, "2260d0236ce4bf2836aeea1fda679ac811e3c09e9e47e038d61034b1f491e75d");
        make_sign_tx(false, &fx);

        return (int)fx.tx_str.size() / 2;
    }

    string get_output_script(string address)
    {
        string script;

        const char fc_addr = address[0];
        if ((fc_addr == '1') || (fc_addr == 'm') || (fc_addr == 'n'))
        {
            script = "76a914" + get_pubkey_hash_from_base58check(address) + "88ac";
        }
        else if ((fc_addr == '3') || (fc_addr == '2'))
        {
            script = "a914" + get_pubkey_hash_from_base58check(address) + "87";
        }
        else
            script = "";	// 不支持这种交易

        return script;
    }

    int make_unsign_tx(UserTransaction* ut)
    {
        TransactionBill tx;
        tx.version = 1;
        tx.locktime = 0;

        u256 value_sum = 0;
        ut->input_count = 0;
        u256 fee = ut->fee_count * ut->fee_price;

        for (size_t i = 0; i < ut->utxo_list.size(); i++)
        {
            TransactionInput input;
            input.previous_output_hash = ut->utxo_list[i].hash;
            input.previous_output_index = ut->utxo_list[i].index;
            input.sequence = 0xffffffff;
            tx.inputs.push_back(input);

            // 保存每一个前交易脚本
            ut->input_count++;

            value_sum += ut->utxo_list[i].value;
            if (value_sum >= (ut->pay + fee))
                break;
        }

        if (value_sum < (ut->pay + fee))
            return -1;		// 余额不足

        TransactionOutput output0;
        output0.address = ut->to_address;
        output0.script = get_output_script(output0.address);
        if (output0.script.empty())
            return -2;		// 不支持这种交易

        output0.value = (uint64_t)(ut->pay);
        tx.outputs.push_back(output0);

        u256 change = value_sum - ut->pay - fee;
        if (!ut->change_address.empty() && (change > 500))
        {
            TransactionOutput output1;
            output1.address = ut->change_address;
            output1.script = get_output_script(output1.address);
            if (output1.script.empty())
                return -2;		// 不支持这种交易

            output1.value = (uint64_t)change;
            tx.outputs.push_back(output1);
        }

        ut->tx_str = tx_encode(tx);

        return 0;
    }

// 生成固件需要的格式的数据
    Binary firmware_prepare_data(bool is_testnet, UserTransaction* ut, int script_index)
    {
        Binary fdata(6);

        *((int*)fdata.data()) = ut->change_wallet_index;
        fdata.data()[4] = 0;	// 收款地址在output中的序号
        fdata.data()[5] = 1;	// 找零地址在output中的序号

        TransactionBill tx;
        tx_decode(is_testnet, ut->tx_str, tx);
        tx.inputs[script_index].script = encode_script_(ut->utxo_list[script_index].script);

        string str_tx = tx_encode(tx);
        fdata += Binary::decode(str_tx);

        Binary tail(4);
        *((unsigned long*)tail.data()) = 1;	// 注意最后要补一个4字节小端序的0x01
        fdata += tail;

        return fdata;
    }

    void firmware_process_result(UserTransaction* ut, int index, char* result, int result_size)
    {
        unsigned char len1 = result[0];
        ut->public_key = Binary(result + 1, len1);

        unsigned char len2 = result[len1 + 1];
        Binary sig = Binary(result + len1 + 2, len2);

        //ut->sig_data[index] = sig;
        ut->sig_data.push_back(sig);
    }

    void sign_tx(bool is_testnet, UserTransaction* ut, const string& private_key)
    {
        ut->public_key = Binary::decode(get_public_key(private_key));
        ut->sig_data.clear();

        for (int i = 0; i < ut->input_count; i++)
        {
            string prev_utxo_script = encode_script_(ut->utxo_list[i].script);
            string signed_tx = sign_input(is_testnet, ut->tx_str, i, prev_utxo_script, private_key);
            ut->sig_data.push_back(Binary::decode(signed_tx));
        }
    }

    void make_sign_tx(bool is_testnet, UserTransaction* ut)
    {
        TransactionBill tx;
        tx_decode(is_testnet, ut->tx_str, tx);
        string str_pubkey = Binary::encode(ut->public_key);

        for (int i = 0; i < ut->input_count; i++)
        {
            string str_sign = Binary::encode(ut->sig_data[i]);

            string str_script;
            str_script += uint8_t_to_string((uint8_t)ut->sig_data[i].size());
            str_script += str_sign;
            str_script += uint8_t_to_string((uint8_t)ut->public_key.size());
            str_script += str_pubkey;

            tx.inputs[i].script = str_script;
        }

        ut->tx_str = tx_encode(tx);
    }

}

// ===================================================================

namespace USDTAPI
{

    int make_unsign_tx(UserTransaction* ut)
    {
        const uint32_t omni_property_id = 31;
        TransactionBill tx;
        tx.version = 1;
        tx.locktime = 0;

        u256 value_sum = 0;
        ut->input_count = 0;
        u256 fee = ut->fee_count * ut->fee_price;

        for (size_t i = 0; i < ut->utxo_list.size(); i++)
        {
            TransactionInput input;
            input.previous_output_hash = ut->utxo_list[i].hash;
            input.previous_output_index = ut->utxo_list[i].index;
            input.sequence = 0xffffffff;
            tx.inputs.push_back(input);

            // 保存每一个前交易脚本
            ut->input_count++;

            value_sum += ut->utxo_list[i].value;
            if (value_sum >= ((u256)546 + fee))
                break;
        }

        if (value_sum < ((u256)546 + fee))
            return -1;		// 余额不足

        // 第一个压入 payload
        TransactionOutput payload;
        payload.value = 0;
        payload.script = "6a";    // 6a is op_return
        string str_payload = CreatePayload_SimpleSend(omni_property_id, (uint64_t)(ut->pay));
        string str_USDT_data = "6f6d6e69" + str_payload;
        payload.script += uint8_t_to_string((uint8_t)str_USDT_data.size() / 2);
        payload.script += str_USDT_data;
        tx.outputs.push_back(payload);

        TransactionOutput output0;
        output0.address = ut->to_address;
        output0.script = "76a914" + get_pubkey_hash_from_base58check(output0.address) + "88ac";
        output0.value = 546;
        tx.outputs.push_back(output0);

        u256 change = value_sum - 546 - fee;
        if (!ut->change_address.empty() && (change > 500))
        {
            TransactionOutput output1;
            output1.address = ut->change_address;
            output1.script = "76a914" + get_pubkey_hash_from_base58check(output1.address) + "88ac";
            output1.value = (uint64_t)change;
            tx.outputs.push_back(output1);
        }

        ut->tx_str = tx_encode(tx);

        return 0;
    }

// 生成固件需要的格式的数据
    Binary firmware_prepare_data(bool is_testnet, UserTransaction* ut, int script_index)
    {
        Binary fdata(6);

        *((int*)fdata.data()) = ut->change_wallet_index;
        fdata.data()[4] = 1;	// 收款地址在output中的序号
        fdata.data()[5] = 2;	// 找零地址在output中的序号

        TransactionBill tx;
        tx_decode(is_testnet, ut->tx_str, tx);
        tx.inputs[script_index].script = encode_script_(ut->utxo_list[script_index].script);

        string str_tx = tx_encode(tx);
        fdata += Binary::decode(str_tx);

        Binary tail(4);
        *((unsigned long*)tail.data()) = 1;	// 注意最后要补一个4字节小端序的0x01
        fdata += tail;

        return fdata;
    }

// 采用模拟签名的方式获得交易串长度
    int get_tx_len(UserTransaction* ut)
    {
        UserTransaction fx;
        fx.from_address = "16TEzNLEMX5dpm18gGbzZ6X9cFoqgUPuMk";
        fx.to_address = "15KJzn2AzrL8hkyfvrGWf2qKpEyR5U8u3Z";
        fx.change_address = "16TEzNLEMX5dpm18gGbzZ6X9cFoqgUPuMk";
        fx.pay = ut->pay;
        fx.fee_count = 0;
        fx.fee_price = 0;
        fx.utxo_list = ut->utxo_list;

        int retcode = make_unsign_tx(&fx);
        if (retcode != 0)
            return(retcode);

        BTCAPI::sign_tx(false, &fx, "2260d0236ce4bf2836aeea1fda679ac811e3c09e9e47e038d61034b1f491e75d");
        BTCAPI::make_sign_tx(false, &fx);

        return (int)fx.tx_str.size() / 2;
    }

    u256 get_usdt_from_tx(bool is_testnet, const string str_tx)
    {
        TransactionBill tx;
        tx_decode(is_testnet, str_tx, tx);

        string str_usdt_amount;
        for (size_t i = 0; i < tx.outputs.size(); i++)
        {
            size_t pos = tx.outputs[i].script.find("6f6d6e69");
            if (-1 != pos)
            {
                str_usdt_amount = tx.outputs[i].script.substr(tx.outputs[i].script.size() - 16);
                uint64_t u64_usdt_amount = big_endian_to_uint64_t(str_usdt_amount);
                return (u256)u64_usdt_amount;
            }
        }
        return((u256)-1);
    }

}
