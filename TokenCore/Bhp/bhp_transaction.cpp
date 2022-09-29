
#include <iostream>
#include "bhp_transaction.h"
#include "crypto/utility_tools.h"
#include "crypto/hash.h"
#include "uEcc/uECC.h"

string BhpTxAttribute::encode()
{
	string str_encode = uint8_t_to_string(Usage);

	if (Usage == DescriptionUrl)
		str_encode += uint8_t_to_string((uint8_t)str_data.size());
	else if (Usage == Description || Usage >= Remark)
		str_encode += uint32_t_to_little_endian((uint32_t)str_data.size());
	if (Usage == ECDH02 || Usage == ECDH03)
		str_encode += str_data.substr(2, 64);
	else
		str_encode += str_data;

	return str_encode;
}

void BhpTxAttribute::decode(const string& str_tx_attr)
{
	size_t index = 0;
	Usage = (BhpTxAttributeUsage)(string_to_uint8_t(str_tx_attr.substr(index, 2)));
	index += 2;
	str_data = str_tx_attr.substr(index);
}

BhpTxAttribute::BhpTxAttribute(const string& str_data /* = "" */, const BhpTxAttributeUsage& Usage /* = ContractHash */)
{
	this->str_data = str_data;
	this->Usage = Usage;
}

BhpTxIn::BhpTxIn(const string& str_prev_hash, uint16_t prev_index)
{
	this->str_prev_hash = str_prev_hash;
	this->prev_index = prev_index;
}

string BhpTxIn::encode()
{
	string str_encode = str_prev_hash;
	str_encode += uint16_t_to_little_endian(prev_index);
	return str_encode;
}

void BhpTxIn::decode(const string& str_input)
{
	size_t index = 0;
	str_prev_hash = str_input.substr(index, 64);
	index += 64;
	prev_index = little_endian_to_uint16_t(str_input.substr(index, 4));
}

string BhpTxOut::encode()
{
	string str_encode = str_asset_id;
	str_encode += uint64_t_to_little_endian(value);
	str_encode += str_pubkey_hash;
	return str_encode;
}

void BhpTxOut::decode(const string& str_output)
{
	size_t index = 0;
	str_asset_id = str_output.substr(index, 64);
	index += 64;
	value = little_endian_to_uint64_t(str_output.substr(index, 16));
	index += 16;
	str_pubkey_hash = str_output.substr(index);
}

BhpTxOut::BhpTxOut()
{
	str_asset_id = "";
	value = 0;
	str_pubkey_hash = "";
}

BhpTxOut::BhpTxOut(const string& str_asset_id, uint64_t value, const string& str_pubkey_hash)
{
	this->str_asset_id = str_asset_id;
	this->value = value;
	this->str_pubkey_hash = str_pubkey_hash;
}

string BhpWitness::encode()
{
	string str_encode = uint8_t_to_string((uint8_t)str_sign.size() / 2 + 1);
	str_encode += uint8_t_to_string((uint8_t)str_sign.size() / 2);
	str_encode += str_sign;

	str_encode += uint8_t_to_string((uint8_t)str_pubkey.size() / 2 + 2);
	str_encode += uint8_t_to_string((uint8_t)str_pubkey.size() / 2);
	str_encode += str_pubkey;
	str_encode += "ac";

	return str_encode;
}

void BhpWitness::decode(const string& str_witness)
{
	size_t index = 0;
	index += 4;		// Ìø¹ý4140
	str_sign = str_witness.substr(index, 128);
	index += 128;
	index += 4;		// Ìø¹ý2321
	str_pubkey = str_witness.substr(index, 66);
}

BhpWitness::BhpWitness()
{
	str_sign = "";
	str_pubkey = "";
}

BhpWitness::BhpWitness(const string& str_sign, const string& str_pubkey)
{
	this->str_sign = str_sign;
	this->str_pubkey = str_pubkey;
}

string BhpTransaction::encode()
{
	string str_encode = uint8_t_to_string((uint8_t)(type));
	str_encode += uint8_t_to_string(version);

	string str_tx_attribute;
	for (size_t i = 0; i < TxAttributes.size(); ++i)
		str_tx_attribute += TxAttributes[i].encode();
	str_encode += str_tx_attribute;

	str_encode += uint8_t_to_string((uint8_t)Inputs.size());
	string str_tx_inputs;
	for (size_t i = 0; i < Inputs.size(); ++i)
		str_tx_inputs += Inputs[i].encode();
	str_encode += str_tx_inputs;

	str_encode += uint8_t_to_string((uint8_t)Outputs.size());
	string str_tx_outputs;
	for (size_t i = 0; i < Outputs.size(); ++i)
		str_tx_outputs += Outputs[i].encode();
	str_encode += str_tx_outputs;

	if (0 == Witnesses.size())
		return str_encode;

	str_encode += uint8_t_to_string((uint8_t)Witnesses.size());
	string str_witness;
	for (size_t i = 0; i < Witnesses.size(); ++i)
		str_witness += Witnesses[i].encode();
	str_encode += str_witness;

	return str_encode;
}

int BhpTransaction::decode(const string& str_tx)
{
	if (str_tx.empty())
		return -1;

	size_t index = 0;
	uint8_t u8_type = string_to_uint8_t(str_tx.substr(index, 2));
	index += 2;
	type = (BhpTxType)u8_type;
	version = string_to_uint8_t(str_tx.substr(index, 2));
	index += 2;

	string str_tx_attr = str_tx.substr(index, 2);
	index += 2;
	BhpTxAttribute txAttr;
	txAttr.decode(str_tx_attr);
	TxAttributes.push_back(txAttr);

	size_t input_count = string_to_uint8_t(str_tx.substr(index, 2));
	index += 2;
	for (size_t i = 0; i < input_count; ++i)
	{
		string str_input = str_tx.substr(index, 68);
		index += 68;
		BhpTxIn in;
		in.decode(str_input);
		Inputs.push_back(in);
	}

	size_t output_count = string_to_uint8_t(str_tx.substr(index, 2));
	index += 2;
	for (size_t i = 0; i < output_count; ++i)
	{
		string str_output = str_tx.substr(index, 120);
		index += 120;
		BhpTxOut out;
		out.decode(str_output);
		Outputs.push_back(out);
	}

	if (index < str_tx.size())
	{
		size_t witness_count = string_to_uint8_t(str_tx.substr(index, 2));
		index += 2;
		for (size_t i = 0; i < witness_count; ++i)
		{
			string str_witness = str_tx.substr(index, 204);
			index += 204;
			BhpWitness wit;
			wit.decode(str_witness);
			Witnesses.push_back(wit);
		}
	}

	return 0;
}

int BhpTransaction::add_tx_in(const string& str_prev_hash, uint16_t prev_index)
{
	if (str_prev_hash.size() != 64)
		return -1;
	
	Inputs.push_back(BhpTxIn(str_prev_hash, prev_index));
	return 0;
}

int BhpTransaction::add_tx_out(const string& str_asset_id, uint64_t value, const string& str_pubkey_hash)
{
	if (str_asset_id.size() != 64)
		return -1;
	if (str_pubkey_hash.size() != 40)
		return -2;

	Outputs.push_back(BhpTxOut(str_asset_id, value, str_pubkey_hash));
	return 0;
}

int BhpTransaction::signature(const string& str_prikey, string& str_sign)
{
	if (str_prikey.size() != 64)
		return -1;

	string str_tx_unsign = encode();
	Binary sz_hash = sha256_hash(Binary::decode(str_tx_unsign));
	Binary sz_prikey = Binary::decode(str_prikey);
	Binary sz_sign(64);

	const struct uECC_Curve_t * curve = uECC_secp256k1();
	if (!uECC_sign(sz_prikey.data(), sz_hash.data(), (uint32_t)sz_hash.size(), sz_sign.data(), curve))
		return -2;

	str_sign = Binary::encode(sz_sign);
	return 0;
}

int BhpTransaction::add_tx_witness(const string& str_sign, const string& str_pubkey)
{
	if ("" == str_sign || "" == str_pubkey)
		return -1;

	Witnesses.push_back(BhpWitness(str_sign, str_pubkey));
	return 0;
}
