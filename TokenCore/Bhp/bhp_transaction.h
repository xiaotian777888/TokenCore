
#ifndef BHP_TRANSACTION_H
#define BHP_TRANSACTION_H

#include <string>
#include <stdint.h>
#include <vector>

enum BhpTxAttributeUsage
{
	ContractHash = 0x00,

	ECDH02 = 0x02,
	ECDH03 = 0x03,

	Script = 0x20,

	Vote = 0x30,

	DescriptionUrl = 0x81,
	Description = 0x90,

	Hash1 = 0xa1,
	Hash2 = 0xa2,
	Hash3 = 0xa3,
	Hash4 = 0xa4,
	Hash5 = 0xa5,
	Hash6 = 0xa6,
	Hash7 = 0xa7,
	Hash8 = 0xa8,
	Hash9 = 0xa9,
	Hash10 = 0xaa,
	Hash11 = 0xab,
	Hash12 = 0xac,
	Hash13 = 0xad,
	Hash14 = 0xae,
	Hash15 = 0xaf,

	Remark = 0xf0,
	Remark1 = 0xf1,
	Remark2 = 0xf2,
	Remark3 = 0xf3,
	Remark4 = 0xf4,
	Remark5 = 0xf5,
	Remark6 = 0xf6,
	Remark7 = 0xf7,
	Remark8 = 0xf8,
	Remark9 = 0xf9,
	Remark10 = 0xfa,
	Remark11 = 0xfb,
	Remark12 = 0xfc,
	Remark13 = 0xfd,
	Remark14 = 0xfe,
	Remark15 = 0xff
};

class BhpTxAttribute
{
public:
	BhpTxAttributeUsage Usage;
	std::string str_data;

	std::string encode();
	void decode(const std::string& str_tx_attr);
	BhpTxAttribute(const std::string& str_data = "", const BhpTxAttributeUsage& Usage = ContractHash);
};

class BhpTxIn
{
public:
	std::string str_prev_hash;
	uint16_t prev_index;

	std::string encode();
	void decode(const std::string& str_input);
	BhpTxIn() : str_prev_hash(""), prev_index(0) {}
	BhpTxIn(const std::string& str_prev_hash, uint16_t prev_index);
};

class BhpTxOut
{
public:
	std::string str_asset_id;
	uint64_t value;
	std::string str_pubkey_hash;

	std::string encode();
	void decode(const std::string& str_output);
	BhpTxOut();
	BhpTxOut(const std::string& str_asset_id, uint64_t value, const std::string& str_pubkey_hash);
};

class BhpWitness
{
public:
	std::string str_invocation_script;
	std::string str_verification_script;
	std::string str_sign;
	std::string str_pubkey;

	std::string encode();
	void decode(const std::string& str_witness);
	BhpWitness();
	BhpWitness(const std::string& str_sign, const std::string& str_pubkey);
};

enum BhpTxType
{	
	MinerTransaction = 0x00,	
	IssueTransaction = 0x01,	
	ClaimTransaction = 0x02,	
	EnrollmentTransaction = 0x20,	
	RegisterTransaction = 0x40,	
	ContractTransaction = 0x80,	
	StateTransaction = 0x90,	
	PublishTransaction = 0xd0,	
	InvocationTransaction = 0xd1
};

class BhpTransaction
{
public:
	BhpTxType type;
	uint8_t version;
	std::vector<BhpTxAttribute> TxAttributes;
	std::vector<BhpTxIn> Inputs;
	std::vector<BhpTxOut> Outputs;
	std::vector<BhpWitness> Witnesses;

	BhpTransaction() {}
	std::string encode();
	int decode(const std::string& str_tx);

	void set_tx_type(BhpTxType type = ContractTransaction) { this->type = type; }
	void set_tx_version(uint8_t version = 0) { this->version = version; }
	void add_tx_attr(BhpTxAttribute attr) { TxAttributes.push_back(attr); }
	int add_tx_in(const std::string& str_prev_hash, uint16_t prev_index);
	int add_tx_out(const std::string& str_asset_id, uint64_t value, const std::string& str_pubkey_hash);
	int signature(const std::string& str_prikey, std::string& str_sign);
	int add_tx_witness(const std::string& str_sign, const std::string& str_pubkey);
};

#endif // BHP_TRANSACTION_H
