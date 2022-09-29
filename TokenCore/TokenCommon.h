#ifndef TOKENCOMMON_H_
#define TOKENCOMMON_H_

#include <string>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <string.h>
#include "u256.h"

using std::string;
using std::vector;


class Binary : public vector<unsigned char>
{
public:
	Binary() : vector<unsigned char>() {}
	Binary(size_t count) : vector<unsigned char>(count) {}
	Binary(vector<unsigned char>::iterator begin, vector<unsigned char>::iterator end) : vector<unsigned char>(begin, end) {}
	Binary(string::const_iterator begin, string::const_iterator end)  { this->assign(begin, end); }
	Binary(const char* str) { this->assign(str, str + strlen(str)); }
	Binary(const char* str, int len) { this->assign(str, str + len); }
	Binary(const char* str, size_t len) { this->assign(str, str + len); }
	Binary(const unsigned char* data, int len) { this->assign(data, data + len); }
	Binary(const unsigned char* data, size_t len) { this->assign(data, data + len); }
	Binary(const string str) { this->assign(str.begin(), str.end()); }

	Binary& operator+=(const Binary& bin)
	{
		copy(bin.begin(), bin.end(), back_inserter(*this));
		return *this;
	}

	Binary operator+(const Binary& bin) const
	{
		Binary out(*this);

		out += bin;
		return(out);
	}

	bool operator== (const Binary& bin) const
	{
		if (this->size() != bin.size())
			return false;

		return memcmp(this->data(), bin.data(), this->size()) == 0;
	}

	bool operator!= (const Binary& bin) const
	{
		return(!operator==(bin));
	}

	size_t to(char* buffer, int size)
	{
		size_t len = (size_t)size > this->size() ? this->size() : size;
		copy(this->begin(), this->begin() + len, buffer);
		if ((size_t)size > this->size())
			buffer[len] = 0;
		return len;
	}

	size_t to(unsigned char* buffer, int size)
	{
		size_t len = (size_t)size > this->size() ? this->size() : size;
		copy(this->begin(), this->begin() + len, buffer);
		return len;
	}

	size_t to(string str)
	{
		str.assign(this->begin(), this->end());
		return this->size();
	}

	Binary& reverse()
	{
		std::reverse(begin(), end());
		return *this;
	}

	// 等同于 encode_base16
	static string encode(const Binary& bin)
	{
		string str;
		if (0 == bin.size())
			return str;

		str.reserve((bin.size()) * 2);

		auto to_hex = [](unsigned char c)
		{
			if (c < 10)
				return(c + 0x30);
			return(c - 10 + 'a');
		};

		for (size_t i = 0; i < bin.size(); i++)
		{
			str.push_back(to_hex(bin[i] >> 4));
			str.push_back(to_hex(bin[i] & 0x0f));
		}

		return str;
	}

	// 等同于 decode_base16
	static Binary decode(const string& str)
	{
		Binary bin;

		if (str.size() % 2 != 0)
			return bin;

		auto is_base16 = [](const char c)
		{
			return ('0' <= c && c <= '9') || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f');
		};

		if (!all_of(str.begin(), str.end(), is_base16))
			return bin;

		auto from_hex = [](const char c)
		{
			if ('A' <= c && c <= 'F')
				return 10 + c - 'A';
			if ('a' <= c && c <= 'f')
				return 10 + c - 'a';
			return c - '0';
		};

		bin.resize(str.size() / 2);
		for (size_t i = 0; i < bin.size(); i++)
		{
			bin[i] = (from_hex(str[i * 2]) << 4) + from_hex(str[i * 2 + 1]);
		}

		return bin;
	}

	Binary left(size_t count)
	{
		size_t num = this->size() > count ? count : this->size();
		if (num == 0)
			return Binary();
		return Binary(this->begin(), this->begin() + num);
	}

	Binary right(size_t count)
	{
		size_t num = this->size() > count ? count : this->size();
		if (num == 0)
			return Binary();
		return Binary(this->end() - num, this->end());
	}

	Binary mid(size_t position, size_t len)
	{
		if (position >= this->size())
			return Binary();

		size_t epos = position + len;
		if (epos >= this->size())
			epos = this->size();

		if (len == 0)
			return Binary();
		return Binary(this->begin() + position, this->begin() + epos);
	}

	// all.insert(all.end(), checksum.begin(), checksum.begin() + 4);
};

struct Utxo
{
	string hash;
	uint32_t index;
	uint64_t value;
	string script;
};

struct UserTransaction
{
	// 输入内容
	string from_address;		// 付款地址
	string to_address;			// 收款地址
	string change_address;		// 找零地址
	u256 pay;					// 付款金额
	u256 fee_count;				// 交易费的 gas 个数
	u256 fee_price;				// 交易费的 gas 单价，fee = fee_count * fee_price
	u256 nonce;					// 交易序号(ETH)
	string contract_address;	// 合约地址

	int from_wallet_index;		// 付款的子私钥的索引
	int change_wallet_index;	// 找零地址的子公钥的索引
	vector<Utxo> utxo_list;

	// 内部使用
	int  input_count;			// 输入的个数
	Binary public_key;			// 付款地址的公钥
	vector<Binary> sig_data;	// 签名数据列表

	// 输出内容
	string tx_str;				// 生成的 tx 串

	void clear(int mode = 0)
	{
		nonce = 0;
		input_count = from_wallet_index = change_wallet_index = 0;
		contract_address.clear();
		public_key.clear();
		utxo_list.clear();
		sig_data.clear();
		tx_str.clear();

		if (mode == 1)
		{
			from_address.clear();
			to_address.clear();
			change_address.clear();
			pay = fee_count = fee_price = 0;
		}
	}

	UserTransaction()
	{
		clear(1);
	}
};

// 测试专用
void VF(const char* hint, bool check);

// 生成指定长度的伪随机数串
string gen_seed(size_t bit_length);

// 根据路径跟种子获取BIP44标准的分层私钥
string bip44_get_private_key(const string bip39_seed, const string bip32_path);

// 根据熵值获取助记词
string entropy_to_mnemonic(const string entropy);

// 通过助记词生成种子(passphrase为BIP38加密短语)
string mnemonic_to_seed(const string mnemonic, const string passphrase);

// 检验助记词是否符合规则
bool check_mnemonic(const string mnemonic);

#endif // TOKENCOMMON_H_
