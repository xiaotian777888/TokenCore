#include <algorithm>
#include <iterator>

#include "BhpApi.h"
#include "uEcc/uECC.h"

#include "crypto/base58.h"
#include "crypto/hash.h"
#include "crypto/utility_tools.h"

#include "Bhp/bhp_transaction.h"

static string get_pubkey_hash_from_address(const string& address)
{
	string pubkey_hash;
	if (address.size() != 34)
		return pubkey_hash;

	Binary check_data = decode_base58(address);
	pubkey_hash = Binary::encode(check_data.mid(1, 20));
	return pubkey_hash;
}

namespace BHPAPI
{

string get_private_key(const string& seed)
{
	return bip44_get_private_key(seed, "m/34952'/0'/0'/0/0");
}

string get_public_key(const string& private_key)
{
	if (private_key.size() != 64)
		return std::string();

	Binary sz_pubkey(64), sz_pubkey_compress(33);
	const struct uECC_Curve_t * curve = uECC_secp256k1();
	Binary sz_prikey = Binary::decode(private_key);

	if (!uECC_compute_public_key(sz_prikey.data(), sz_pubkey.data(), curve))
		return std::string();

	uECC_compress(sz_pubkey.data(), sz_pubkey_compress.data(), curve);
	std::string public_key = Binary::encode(sz_pubkey_compress);
	return public_key;
}

string get_address(const string& public_key)
{
	string address;

	if (public_key.size() != 66)
		return address;

	Binary script;
	script.push_back(0x21);
	script += Binary::decode(public_key);
	script.push_back(0xac);

	Binary bin_address;
	bin_address.push_back(0x17);
	bin_address += bitcoin160(script);

	Binary checksum = bitcoin256(bin_address);
	bin_address += checksum.left(4);
	address = encode_base58(bin_address);

	return address;
}

bool validate_address(const string& str_address)
{
	if (str_address.size() != 34)
		return false;

	Binary bin_address = decode_base58(str_address);
	Binary bin_payload = bin_address.left(bin_address.size() - 4);
	Binary checksum = bitcoin256(bin_payload);

	if (checksum.left(4) != bin_address.right(4))
		return false;

	if (bin_payload[0] != 0x17)
		return false;

	return true;
}

int make_unsign_tx(UserTransaction* ut)
{
	// 这个类似合约地址，是固定的
	//string asset_id("13f76fabfe19f3ec7fd54d63179a156bafc44afc53a7f07a7a15f6724c0aa854");
	string asset_id("54a80a4c72f6157a7af0a753fc4ac4af6b159a17634dd57fecf319feab6ff713");

	BhpTransaction tx;
	tx.set_tx_type();
	tx.set_tx_version();

	BhpTxAttribute txAttr;
	tx.add_tx_attr(txAttr);

	u256 value_sum = 0;
	ut->input_count = 0;

	u256 fee = ut->fee_count * ut->fee_price;

	for (size_t i = 0; i < ut->utxo_list.size(); i++)
	{
		string rev_prev_tx = reverse_big_little_endian(ut->utxo_list[i].hash);
		tx.add_tx_in(rev_prev_tx, ut->utxo_list[i].index);

		// 保存每一个前交易脚本
		ut->input_count++;

		value_sum += ut->utxo_list[i].value;
		if (value_sum >= (ut->pay + fee))
			break;
	}

	if (value_sum < (ut->pay + fee))
		return -1;		// 余额不足

	string to_pubkey_hash = get_pubkey_hash_from_address(ut->to_address);
	tx.add_tx_out(asset_id, (uint64_t)ut->pay, to_pubkey_hash);

	u256 change = value_sum - ut->pay - fee;
	if (!ut->change_address.empty() && (change > 50))
	{
		string change_pubkey_hash = get_pubkey_hash_from_address(ut->change_address);
		tx.add_tx_out(asset_id, (uint64_t)change, change_pubkey_hash);
	}

	ut->tx_str = tx.encode();

	return 0;
}

void sign_tx(UserTransaction* ut, const string& private_key)
{
	ut->public_key = Binary::decode(get_public_key(private_key));

	BhpTransaction tx;
	tx.decode(ut->tx_str);

	string sign_data;
	tx.signature(private_key, sign_data);

	ut->sig_data.clear();
	ut->sig_data.push_back(Binary::decode(sign_data));
}

void make_sign_tx(UserTransaction* ut)
{
	BhpTransaction tx;
	tx.decode(ut->tx_str);

	if (ut->sig_data.size() > 0)
		tx.add_tx_witness(Binary::encode(ut->sig_data[0]), Binary::encode(ut->public_key));

	ut->tx_str = tx.encode();
}

string tx_hash(const string& tx_str)
{
	BhpTransaction tx;
	tx.decode(tx_str);
	tx.Witnesses.clear();

	Binary hash = sha256_hash(sha256_hash(Binary::decode(tx.encode())));
	return Binary::encode(hash.reverse());
}

// 生成固件需要的格式的数据
Binary firmware_prepare_data(UserTransaction* ut)
{
	Binary fdata(6);

	*((int*)fdata.data()) = ut->change_wallet_index;
	fdata.data()[4] = 0;	// 收款地址在output中的序号
	fdata.data()[5] = 1;	// 找零地址在output中的序号

	fdata += Binary::decode(ut->tx_str);

	return fdata;
}

void firmware_process_result(UserTransaction* ut, char* result, int result_size)
{
	unsigned char len1 = result[0];
	ut->public_key = Binary(result + 1, len1);

	unsigned char len2 = result[len1 + 1];
	Binary sig = Binary(result + len1 + 2, len2);

	ut->sig_data.clear();
	ut->sig_data.push_back(sig);
}

// 获取交易字符串长度
int get_tx_len(UserTransaction* ut)
{
	// 你那边按照btc 的构造就行了，手续费的话就是按照每512字节收取 0.0001，最多收取0.0005就行了
	UserTransaction tt;
	tt.from_address = ut->from_address;
	tt.to_address = ut->to_address;
	tt.change_address = ut->change_address;
	tt.pay = ut->pay;
	tt.fee_price = 0;
	tt.fee_count = 0;
	tt.utxo_list = ut->utxo_list;

	if (0 != make_unsign_tx(&tt))
		return 0;

	string private_key = "fca599567eb49585c2c091f257551b31beeed4bd0eea68cc48bfac25507a847e";
	sign_tx(&tt, private_key);
	make_sign_tx(&tt);

	return (int)(tt.tx_str.size() / 2);
}

void dump_tx(const string& tx_str)
{
	BhpTransaction tx;
	tx.decode(tx_str);

	printf("transaction\n{\n");
	printf("\ttype %u\n", (unsigned int)tx.type);
	printf("\tversion %u\n", (unsigned int)tx.version);

	printf("\tTxAttributes\n\t{\n");
	for (size_t i = 0; i < tx.TxAttributes.size(); i++)
	{
		printf("\t\tAttribute\n");
		printf("\t\t{\n");
		printf("\t\t\tusage %u\n", tx.TxAttributes[i].Usage);
		printf("\t\t\tdata %s\n", tx.TxAttributes[i].str_data.c_str());
		printf("\t\t}\n");
	}
	printf("\t}\n");
	
	printf("\tinputs\n\t{\n");
	for (size_t i = 0; i < tx.Inputs.size(); i++)
	{
		printf("\t\tinput\n");
		printf("\t\t{\n");

		string hash = reverse_big_little_endian(tx.Inputs[i].str_prev_hash);
		printf("\t\t\thash %s\n", hash.c_str());
		printf("\t\t\tindex %u\n", tx.Inputs[i].prev_index);

		printf("\t\t}\n");
	}
	printf("\t}\n");

	printf("\toutputs\n\t{\n");
	for (size_t i = 0; i < tx.Outputs.size(); i++)
	{
		printf("\t\toutput\n");
		printf("\t\t{\n");

		string asset_id = reverse_big_little_endian(tx.Outputs[i].str_asset_id);
		printf("\t\t\tasset_id %s\n", asset_id.c_str());
		printf("\t\t\tvalue %I64u\n", tx.Outputs[i].value);
		printf("\t\t\tpubkey_hash %s\n", tx.Outputs[i].str_pubkey_hash.c_str());

		printf("\t\t}\n");
	}
	printf("\t}\n");

	printf("\tWitnesses\n\t{\n");
	for (size_t i = 0; i < tx.Witnesses.size(); i++)
	{
		printf("\t\tWitnesse\n");
		printf("\t\t{\n");

		printf("\t\t\tinvocation_script %s\n", tx.Witnesses[i].str_invocation_script.c_str());
		printf("\t\t\tverification_script %s\n", tx.Witnesses[i].str_verification_script.c_str());
		printf("\t\t\tsign %s\n", tx.Witnesses[i].str_sign.c_str());
		printf("\t\t\tpubkey %s\n", tx.Witnesses[i].str_pubkey.c_str());

		printf("\t\t}\n");
	}
	printf("\t}\n");

	printf("}\n");
}

}
