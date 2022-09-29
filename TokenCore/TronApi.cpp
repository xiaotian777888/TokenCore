#include "TronApi.h"

#include <map>
#include <algorithm>
#include <json/json.h>

#include <uEcc/macroECC.h>
#include "Tron/Transaction.pb.h"
#include "crypto/bip.h"
#include "crypto/base58.h"
#include "crypto/utility_tools.h"
#include "crypto/keccak256.h"
#include "crypto/ripemd160.h"
#include "crypto/hash.h"

#include "Tron/CJsonObject.hpp"
#include "crypto/tx_util.h"

namespace TRONAPI
{

string _to_upper(const string &input)
{
    char *_tmp = (char *)calloc(input.length() + 1, 1);
    memcpy(_tmp, input.c_str(), input.length());
    for (size_t i = 0; i < input.length(); ++i) {
        if (_tmp[i] >= 'a' && _tmp[i] <= 'z')
            _tmp[i] = 'A' + (_tmp[i] - 'a');
    }

    string ret = _tmp;
    free(_tmp);
    return ret;
}

map<tronProtocol::Contract_ContractType, string> g_map_contract_type_2_string =
{
	{ tronProtocol::Contract_ContractType_AccountCreateContract, "AccountCreateContract" },
	{ tronProtocol::Contract_ContractType_TransferContract, "TransferContract" },
	{ tronProtocol::Contract_ContractType_TransferAssetContract, "TransferAssetContract" },
	{ tronProtocol::Contract_ContractType_VoteAssetContract, "VoteAssetContract" },
	{ tronProtocol::Contract_ContractType_VoteWitnessContract, "VoteWitnessContract" },
	{ tronProtocol::Contract_ContractType_WitnessCreateContract, "WitnessCreateContract" },
	{ tronProtocol::Contract_ContractType_AssetIssueContract, "AssetIssueContract" },
	{ tronProtocol::Contract_ContractType_WitnessUpdateContract, "WitnessUpdateContract" },
	{ tronProtocol::Contract_ContractType_ParticipateAssetIssueContract, "ParticipateAssetIssueContract" },
	{ tronProtocol::Contract_ContractType_AccountUpdateContract, "AccountUpdateContract" },
	{ tronProtocol::Contract_ContractType_FreezeBalanceContract, "FreezeBalanceContract" },
	{ tronProtocol::Contract_ContractType_UnfreezeBalanceContract, "UnfreezeBalanceContract" },
	{ tronProtocol::Contract_ContractType_WithdrawBalanceContract, "WithdrawBalanceContract" },
	{ tronProtocol::Contract_ContractType_UnfreezeAssetContract, "UnfreezeAssetContract" },
	{ tronProtocol::Contract_ContractType_UpdateAssetContract, "UpdateAssetContract" },
	{ tronProtocol::Contract_ContractType_ProposalCreateContract, "ProposalCreateContract" },
	{ tronProtocol::Contract_ContractType_ProposalApproveContract, "ProposalApproveContract" },
	{ tronProtocol::Contract_ContractType_ProposalDeleteContract, "ProposalDeleteContract" },
	{ tronProtocol::Contract_ContractType_SetAccountIdContract, "SetAccountIdContract" },
	{ tronProtocol::Contract_ContractType_CustomContract, "CustomContract" },
	{ tronProtocol::Contract_ContractType_CreateSmartContract, "CreateSmartContract" },
	{ tronProtocol::Contract_ContractType_TriggerSmartContract, "TriggerSmartContract" },
	{ tronProtocol::Contract_ContractType_GetContract, "GetContract" },
	{ tronProtocol::Contract_ContractType_UpdateSettingContract, "UpdateSettingContract" },
	{ tronProtocol::Contract_ContractType_ExchangeCreateContract, "ExchangeCreateContract" },
	{ tronProtocol::Contract_ContractType_ExchangeInjectContract, "ExchangeInjectContract" },
	{ tronProtocol::Contract_ContractType_ExchangeWithdrawContract, "ExchangeWithdrawContract" },
	{ tronProtocol::Contract_ContractType_ExchangeTransactionContract, "ExchangeTransactionContract" },
	{ tronProtocol::Contract_ContractType_UpdateEnergyLimitContract, "UpdateEnergyLimitContract" },
};

map<string, tronProtocol::Contract_ContractType> g_map_string_2_contract_type =
{
	{ "AccountCreateContract", tronProtocol::Contract_ContractType_AccountCreateContract },
	{ "TransferContract", tronProtocol::Contract_ContractType_TransferContract },
	{ "TransferAssetContract", tronProtocol::Contract_ContractType_TransferAssetContract },
	{ "VoteAssetContract", tronProtocol::Contract_ContractType_VoteAssetContract },
	{ "VoteWitnessContract", tronProtocol::Contract_ContractType_VoteWitnessContract },
	{ "WitnessCreateContract", tronProtocol::Contract_ContractType_WitnessCreateContract },
	{ "AssetIssueContract", tronProtocol::Contract_ContractType_AssetIssueContract },
	{ "WitnessUpdateContract", tronProtocol::Contract_ContractType_WitnessUpdateContract },
	{ "ParticipateAssetIssueContract", tronProtocol::Contract_ContractType_ParticipateAssetIssueContract },
	{ "AccountUpdateContract", tronProtocol::Contract_ContractType_AccountUpdateContract },
	{ "FreezeBalanceContract", tronProtocol::Contract_ContractType_FreezeBalanceContract },
	{ "UnfreezeBalanceContract", tronProtocol::Contract_ContractType_UnfreezeBalanceContract },
	{ "WithdrawBalanceContract", tronProtocol::Contract_ContractType_WithdrawBalanceContract },
	{ "UnfreezeAssetContract", tronProtocol::Contract_ContractType_UnfreezeAssetContract },
	{ "UpdateAssetContract", tronProtocol::Contract_ContractType_UpdateAssetContract },
	{ "ProposalCreateContract", tronProtocol::Contract_ContractType_ProposalCreateContract },
	{ "ProposalApproveContract", tronProtocol::Contract_ContractType_ProposalApproveContract },
	{ "ProposalDeleteContract", tronProtocol::Contract_ContractType_ProposalDeleteContract },
	{ "SetAccountIdContract", tronProtocol::Contract_ContractType_SetAccountIdContract },
	{ "CustomContract", tronProtocol::Contract_ContractType_CustomContract },
	{ "CreateSmartContract", tronProtocol::Contract_ContractType_CreateSmartContract },
	{ "TriggerSmartContract", tronProtocol::Contract_ContractType_TriggerSmartContract },
	{ "GetContract", tronProtocol::Contract_ContractType_GetContract },
	{ "UpdateSettingContract", tronProtocol::Contract_ContractType_UpdateSettingContract },
	{ "ExchangeCreateContract", tronProtocol::Contract_ContractType_ExchangeCreateContract },
	{ "ExchangeInjectContract", tronProtocol::Contract_ContractType_ExchangeInjectContract },
	{ "ExchangeWithdrawContract", tronProtocol::Contract_ContractType_ExchangeWithdrawContract },
	{ "ExchangeTransactionContract", tronProtocol::Contract_ContractType_ExchangeTransactionContract },
	{ "UpdateEnergyLimitContract", tronProtocol::Contract_ContractType_UpdateEnergyLimitContract },
};

bool validate_address(const string& str_address)
{
	if (str_address.size() != 34 || str_address[0] != 'T')
		return false;

	Binary sz_address = decode_base58(str_address);
	Binary sz_pubkey_hash = sz_address.left(21);
	Binary sz_checksum = sz_address.right(4);
	Binary sz_checksum_tmp = bitcoin256(sz_pubkey_hash).left(4);
	if (sz_checksum == sz_checksum_tmp)
		return true;

	return false;
}

string get_private_key(const string& seed)
{
	return bip44_get_private_key(seed, "m/44'/195'/0'/0/0");
}

string get_public_key(const string str_prikey, bool is_compress /* = true */)
{
	if (str_prikey.size() != 64)
		return string("");

	Binary sz_pubkey(mECC_BYTES * 2), sz_pubkey_compress(mECC_BYTES + 1);
	Binary sz_prikey = Binary::decode(str_prikey);

	int ret = mECC_compute_public_key(sz_prikey.data(), sz_pubkey.data());
	if (1 != ret)
		return string("");

	if (is_compress)
	{
		mECC_compress(sz_pubkey.data(), sz_pubkey_compress.data());
		return Binary::encode(sz_pubkey_compress);
	}

	return "04" + Binary::encode(sz_pubkey);
}

string get_address(const string &str_pubkey_nocompress, bool is_mainnet /* = true */)
{
    if (str_pubkey_nocompress.size() != 128 && str_pubkey_nocompress.size() != 130)
        return string("");

	string str_pubkey;
	if (str_pubkey_nocompress.size() == 130)
		str_pubkey = str_pubkey_nocompress.substr(2);
	else
		str_pubkey = str_pubkey_nocompress;

    string str_address;
    Binary sz_keccak256(32);
    Binary sz_pubkey_nocompress = Binary::decode(str_pubkey);
    keccak_256(sz_pubkey_nocompress.data(), sz_pubkey_nocompress.size(), sz_keccak256.data());
    string str_keccak256 = Binary::encode(sz_keccak256);
    string str_pubkey_hash = str_keccak256.substr(str_keccak256.size() - 40);
    if (is_mainnet)
        str_pubkey_hash = "41" + str_pubkey_hash;
    else
        str_pubkey_hash = "a0" + str_pubkey_hash;

    Binary sz_pubkey_hash = Binary::decode(str_pubkey_hash);
    Binary sz_checksum = bitcoin256(sz_pubkey_hash).left(4);
    sz_pubkey_hash += sz_checksum;
    str_address = encode_base58(sz_pubkey_hash);
    return str_address;
}

string decompress_pub_key(const string &str_compressed_pub_key)
{
	if (str_compressed_pub_key.size() != 66)
		return string("");

	Binary compressed_pub_key = Binary::decode(str_compressed_pub_key);
	if (compressed_pub_key.size() != (mECC_BYTES + 1))
		return "";
	Binary result(mECC_BYTES * 2);
	mECC_decompress(compressed_pub_key.data(), result.data());
	return "04" + Binary::encode(result);
}

static Binary get_pubkey_hash_from_address(const string &str_address)
{
    if (str_address.size() != 34)
        return string("");

    Binary check_data = decode_base58(str_address);
    Binary sz_pubkey_hash = check_data.mid(0, check_data.size() - 4);
    return sz_pubkey_hash;
}

string make_unsigned_tx_trc10(
    const string &str_token_name,
    const string &str_block_id,
    const string &str_from_address,
    const string &str_to_address,
    uint64_t u64_amount
) {
    if (str_from_address.size() != 34 || str_to_address.size() != 34)
        return string("");

    tronProtocol::raw raw_data;
    //string str_blockid = getNewestBlockID();
    // cout << str_blockid << endl;

    // 设置引用块
    string str_ref_block_bytes = str_block_id.substr(12, 4);
    Binary sz_ref_block_bytes = Binary::decode(str_ref_block_bytes);
    raw_data.set_ref_block_bytes(sz_ref_block_bytes.data(), sz_ref_block_bytes.size());
    string str_ref_block_hash = str_block_id.substr(16, 16);
    Binary sz_ref_block_hash = Binary::decode(str_ref_block_hash);
    raw_data.set_ref_block_hash(sz_ref_block_hash.data(), sz_ref_block_hash.size());

    // 设置时间截和交易到期时间
    chrono::system_clock::duration d = chrono::system_clock::now().time_since_epoch();
    chrono::milliseconds mil = chrono::duration_cast<chrono::milliseconds>(d);
    uint64_t timestamp = mil.count();
    // cout << timestamp << endl;
    raw_data.set_timestamp(timestamp);
    chrono::milliseconds time_out(1000 * 60 * 60);
    // cout << time_out.count() << endl;
    uint64_t expiration = timestamp + time_out.count();
    raw_data.set_expiration(expiration);

    // 设置合约类型
    tronProtocol::Contract *lp_contract = raw_data.mutable_contract();
    string str_token = _to_upper(str_token_name);
    //		string str_token(str_token_name);
    //        transform(str_token_name.begin(), str_token_name.end(), str_token.begin(), toupper);
    tronProtocol::Contract_ContractType type = tronProtocol::Contract_ContractType_TransferContract;

    google::protobuf::Any *lp_any = lp_contract->mutable_parameter();
    Binary sz_owner_pubkey_hash = get_pubkey_hash_from_address(str_from_address);
    Binary sz_to_pubkey_hash = get_pubkey_hash_from_address(str_to_address);

    if ("TRX" == str_token) {
        tronProtocol::TransferContract *lp_transfer = new tronProtocol::TransferContract();
        lp_transfer->set_owner_address(sz_owner_pubkey_hash.data(),
            sz_owner_pubkey_hash.size());
        lp_transfer->set_to_address(sz_to_pubkey_hash.data(), sz_to_pubkey_hash.size());
        lp_transfer->set_amount(u64_amount);

        int iSize = lp_transfer->ByteSize();
        unsigned char *lpszTransferContract = new unsigned char[iSize];
        memset(lpszTransferContract, 0, iSize);
        lp_transfer->SerializeToArray(lpszTransferContract, iSize);

        lp_any->set_value(lpszTransferContract, iSize);

        lp_contract->set_type(type);

        delete[]lpszTransferContract;
        lpszTransferContract = NULL;
    }
    else {
        tronProtocol::TransferAssetContract *lp_transfer = new tronProtocol::TransferAssetContract();
        lp_transfer->set_asset_name(str_token_name.c_str(), str_token_name.size());
        lp_transfer->set_owner_address(sz_owner_pubkey_hash.data(),
            sz_owner_pubkey_hash.size());
        lp_transfer->set_to_address(sz_to_pubkey_hash.data(), sz_to_pubkey_hash.size());
        lp_transfer->set_amount(u64_amount);

        int iSize = lp_transfer->ByteSize();
        unsigned char *lpszTransferAssetContract = new unsigned char[iSize];
        memset(lpszTransferAssetContract, 0, iSize);
        lp_transfer->SerializeToArray(lpszTransferAssetContract, iSize);

        lp_any->set_value(lpszTransferAssetContract, iSize);

        type = tronProtocol::Contract_ContractType_TransferAssetContract;
        lp_contract->set_type(type);

        delete[]lpszTransferAssetContract;
        lpszTransferAssetContract = NULL;
    }

    string str_type_url = string("type.googleapis.com/protocol.") + g_map_contract_type_2_string[type];
    lp_any->set_type_url(str_type_url.c_str());

    int raw_data_size = raw_data.ByteSize();
    Binary sz_raw_data(raw_data_size);
    raw_data.SerializeToArray(sz_raw_data.data(), raw_data_size);
    string str_raw_data = Binary::encode(sz_raw_data);
    // cout << str_raw_data << endl;
    // string str_tx_hash = sha256_hash(str_raw_data);
    // cout << str_tx_hash << endl;
    return str_raw_data;
}

static Binary padding_to_256bit(const Binary &src)
{
    if (src.size() >= 32)
        return Binary();

    Binary _src = src;
	_src.reverse();
    for (size_t i = 0; i < 32 - src.size(); i++)
        _src.push_back(0);
	_src.reverse();
    return _src;
}

string make_unsigned_tx_trc20(
    const string &str_block_id,
    const string &str_contract_address,
    const string &str_from_address,
    const string &str_to_address,
    uint64_t u64_amount)
{
    if (str_from_address.size() != 34 || str_to_address.size() != 34)
        return string("");

    tronProtocol::raw raw_data;
    //string str_blockid = getNewestBlockID();
    // cout << str_blockid << endl;

    // 设置引用块
    string str_ref_block_bytes = str_block_id.substr(12, 4);
    Binary sz_ref_block_bytes = Binary::decode(str_ref_block_bytes);
    raw_data.set_ref_block_bytes(sz_ref_block_bytes.data(), sz_ref_block_bytes.size());
    string str_ref_block_hash = str_block_id.substr(16, 16);
    Binary sz_ref_block_hash = Binary::decode(str_ref_block_hash);
    raw_data.set_ref_block_hash(sz_ref_block_hash.data(), sz_ref_block_hash.size());

    // 设置时间截和交易到期时间
    chrono::system_clock::duration d = chrono::system_clock::now().time_since_epoch();
    chrono::milliseconds mil = chrono::duration_cast<chrono::milliseconds>(d);
    uint64_t timestamp = mil.count();
    // cout << timestamp << endl;
    raw_data.set_timestamp(timestamp);
    chrono::milliseconds time_out(1000 * 60 * 60);
    // cout << time_out.count() << endl;
    uint64_t expiration = timestamp + time_out.count();
    raw_data.set_expiration(expiration);

    // 设置fee_limit,默认为1000000sun=1trx
    raw_data.set_fee_limit(1000000);

    // 设置合约
    tronProtocol::Contract *lp_contract = raw_data.mutable_contract();
    tronProtocol::Contract_ContractType type = tronProtocol::Contract_ContractType_TriggerSmartContract;
    lp_contract->set_type(type);
    google::protobuf::Any *lp_any = lp_contract->mutable_parameter();
    Binary sz_owner_pubkey_hash = get_pubkey_hash_from_address(str_from_address);
    Binary sz_to_pubkey_hash = get_pubkey_hash_from_address(str_to_address);
    Binary sz_contract_address = get_pubkey_hash_from_address(str_contract_address);

    tronProtocol::TriggerSmartContract trigger_smart_contract = tronProtocol::TriggerSmartContract();
    trigger_smart_contract.set_owner_address(sz_owner_pubkey_hash.data(),
        sz_owner_pubkey_hash.size());
    trigger_smart_contract.set_contract_address(sz_contract_address.data(),
        sz_contract_address.size());

    // method
    Binary sz_method = "transfer(address,uint256)", sz_method_keccak256(32);
    keccak_256(sz_method.data(), sz_method.size(), sz_method_keccak256.data());
    string str_method_keccak256 = Binary::encode(sz_method_keccak256);

    // parameters
    Binary sz_to_pubkey_hash_256 = padding_to_256bit(sz_to_pubkey_hash);
    Binary sz_amount = Binary::decode(uint64_t_to_big_endian(u64_amount));
    sz_amount = padding_to_256bit(sz_amount);

    // bytes data
    Binary sz_data(sz_method_keccak256.begin(), sz_method_keccak256.begin() + 4);
    sz_data.insert(sz_data.end(), sz_to_pubkey_hash_256.begin(), sz_to_pubkey_hash_256.end());
    sz_data.insert(sz_data.end(), sz_amount.begin(), sz_amount.end());
    trigger_smart_contract.set_data(sz_data.data(), sz_data.size());

    int smart_contract_size = trigger_smart_contract.ByteSize();
    Binary sz_smart_contract(smart_contract_size);
    trigger_smart_contract.SerializeToArray(sz_smart_contract.data(), smart_contract_size);
    lp_any->set_value(sz_smart_contract.data(), smart_contract_size);

    string str_type_url = string("type.googleapis.com/protocol.") + g_map_contract_type_2_string[type];
    lp_any->set_type_url(str_type_url.c_str());

    int raw_data_size = raw_data.ByteSize();
    Binary sz_raw_data(raw_data_size);
    raw_data.SerializeToArray(sz_raw_data.data(), raw_data_size);
    string str_raw_data = Binary::encode(sz_raw_data);
    // cout << str_raw_data << endl;
    // string str_tx_hash = sha256_hash(str_raw_data);
    // cout << str_tx_hash << endl;
    return str_raw_data;
}

string make_unsigned_tx_from_json(const string &str_unsined_tx_json) {
	if ("" == str_unsined_tx_json) {
		return string("");
	}

	neb::CJsonObject oJson(str_unsined_tx_json);
	string str_raw_data_tag = "raw_data", str_contract_tag = "contract";
	string str_type = oJson[str_raw_data_tag][str_contract_tag][0]["type"].ToString();
	str_type = str_type.substr(1, str_type.size() - 2);

	if ("TransferContract" == str_type) {
		return make_unsigned_tx_trx_from_json(str_unsined_tx_json);
	}
	else if ("TriggerSmartContract" == str_type) {
		return make_unsigned_tx_trc20_from_json(str_unsined_tx_json);
	}
	else if ("FreezeBalanceContract" == str_type) {
		return make_unsigned_tx_freeze_from_json(str_unsined_tx_json);
	}
	else if ("UnfreezeBalanceContract" == str_type) {
		return make_unsigned_tx_unfreeze_from_json(str_unsined_tx_json);
	}
	else {
		return string("");
	}
}

string make_unsigned_tx_trc20_from_json(const string &str_unsign_tx_trc20_json)
{
    if ("" == str_unsign_tx_trc20_json)
        return string("");

    neb::CJsonObject oJson(str_unsign_tx_trc20_json);
    tronProtocol::raw raw_data;

    // ref_block_bytes ref_block_hash
    string str_raw_data_tag = "raw_data";
    string str_ref_block_bytes = oJson[str_raw_data_tag]["ref_block_bytes"].ToString();
    str_ref_block_bytes = str_ref_block_bytes.substr(1, str_ref_block_bytes.size() - 2);
    Binary sz_ref_block_bytes = Binary::decode(str_ref_block_bytes);
    raw_data.set_ref_block_bytes(sz_ref_block_bytes.data(), sz_ref_block_bytes.size());
    string str_ref_block_hash = oJson[str_raw_data_tag]["ref_block_hash"].ToString();
    str_ref_block_hash = str_ref_block_hash.substr(1, str_ref_block_hash.size() - 2);
    Binary sz_ref_block_hash = Binary::decode(str_ref_block_hash);
    raw_data.set_ref_block_hash(sz_ref_block_hash.data(), sz_ref_block_hash.size());

    // expiration fee_limit timestamp
    uint64 expiration = 0, fee_limit = 0, timestamp = 0;
    oJson[str_raw_data_tag].Get("expiration", expiration);
    raw_data.set_expiration(expiration);
    oJson[str_raw_data_tag].Get("fee_limit", fee_limit);
    raw_data.set_fee_limit(fee_limit);
    oJson[str_raw_data_tag].Get("timestamp", timestamp);
    raw_data.set_timestamp(timestamp);

    // contract
    string str_contract_tag = "contract", str_parameter_tag = "parameter";
    string str_value_tag = "value";
    string str_type = oJson[str_raw_data_tag][str_contract_tag][0]["type"].ToString();
    str_type = str_type.substr(1, str_type.size() - 2);
    tronProtocol::Contract_ContractType type = g_map_string_2_contract_type[str_type];
    tronProtocol::Contract *lp_contract = raw_data.mutable_contract();
    lp_contract->set_type(type);

    google::protobuf::Any *lp_any = lp_contract->mutable_parameter();
    string str_type_url = oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag]["type_url"].ToString();
    str_type_url = str_type_url.substr(1, str_type_url.size() - 2);
    lp_any->set_type_url(str_type_url.c_str());

    string str_data = oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag][str_value_tag]["data"].ToString();
    str_data = str_data.substr(1, str_data.size() - 2);
    Binary sz_data = Binary::decode(str_data);
    string str_owner_address = oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag][str_value_tag]["owner_address"].ToString();
    str_owner_address = str_owner_address.substr(1, str_owner_address.size() - 2);
    Binary sz_owner_address = Binary::decode(str_owner_address);
    string str_contract_address = oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag][str_value_tag]["contract_address"].ToString();
    str_contract_address = str_contract_address.substr(1, str_contract_address.size() - 2);
    Binary sz_contract_address = Binary::decode(str_contract_address);
    uint64 call_value = 0;
    oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag][str_value_tag].Get("call_value", call_value);

    tronProtocol::TriggerSmartContract trigger_smart_contract = tronProtocol::TriggerSmartContract();
    trigger_smart_contract.set_owner_address(sz_owner_address.data(), sz_owner_address.size());
    trigger_smart_contract.set_contract_address(sz_contract_address.data(), sz_contract_address.size());
    trigger_smart_contract.set_call_value(call_value);
    trigger_smart_contract.set_data(sz_data.data(), sz_data.size());

    int smart_contract_size = trigger_smart_contract.ByteSize();
    Binary sz_smart_contract(smart_contract_size);
    trigger_smart_contract.SerializeToArray(sz_smart_contract.data(), smart_contract_size);
    lp_any->set_value(sz_smart_contract.data(), smart_contract_size);

    int raw_data_size = raw_data.ByteSize();
    Binary sz_raw_data(raw_data_size);
    raw_data.SerializeToArray(sz_raw_data.data(), raw_data_size);
    string str_raw_data = Binary::encode(sz_raw_data);
    // cout << str_raw_data << endl;
    // string str_tx_hash = sha256_hash(str_raw_data);
    // cout << str_tx_hash << endl;
    return str_raw_data;
}

string make_unsigned_tx_trx_from_json(const string &str_unsigned_tx_trx_json) {
	if ("" == str_unsigned_tx_trx_json)
		return std::string("");

	neb::CJsonObject oJson(str_unsigned_tx_trx_json);
	tronProtocol::raw raw_data;

	// ref_block_bytes ref_block_hash
	string str_raw_data_tag = "raw_data";
	string str_ref_block_bytes = oJson[str_raw_data_tag]["ref_block_bytes"].ToString();
	// remove double quotation mark
	str_ref_block_bytes = str_ref_block_bytes.substr(1, str_ref_block_bytes.size() - 2);
	Binary sz_ref_block_bytes = Binary::decode(str_ref_block_bytes);
	raw_data.set_ref_block_bytes(sz_ref_block_bytes.data(), sz_ref_block_bytes.size());
	string str_ref_block_hash = oJson[str_raw_data_tag]["ref_block_hash"].ToString();
	str_ref_block_hash = str_ref_block_hash.substr(1, str_ref_block_hash.size() - 2);
	Binary sz_ref_block_hash = Binary::decode(str_ref_block_hash);
	raw_data.set_ref_block_hash(sz_ref_block_hash.data(), sz_ref_block_hash.size());

	// expiration fee_limit timestamp
	uint64 expiration = 0, fee_limit = 0, timestamp = 0;
	oJson[str_raw_data_tag].Get("expiration", expiration);
	raw_data.set_expiration(expiration);
	oJson[str_raw_data_tag].Get("fee_limit", fee_limit);
	raw_data.set_fee_limit(fee_limit);
	oJson[str_raw_data_tag].Get("timestamp", timestamp);
	raw_data.set_timestamp(timestamp);

	// contract
	string str_contract_tag = "contract", str_parameter_tag = "parameter";
	string str_value_tag = "value";
	string str_type = oJson[str_raw_data_tag][str_contract_tag][0]["type"].ToString();
	str_type = str_type.substr(1, str_type.size() - 2);
	tronProtocol::Contract_ContractType type = g_map_string_2_contract_type[str_type];
	tronProtocol::Contract *lp_contract = raw_data.mutable_contract();
	lp_contract->set_type(type);

	google::protobuf::Any *lp_any = lp_contract->mutable_parameter();
	string str_type_url = oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag]["type_url"].ToString();
	str_type_url = str_type_url.substr(1, str_type_url.size() - 2);
	lp_any->set_type_url(str_type_url.c_str());

	//string str_data = oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag][str_value_tag]["data"].ToString();
	//str_data = str_data.substr(1, str_data.size() - 2);
	//Binary sz_data = Binary::decode(str_data);
	string str_owner_address = oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag][str_value_tag]["owner_address"].ToString();
	str_owner_address = str_owner_address.substr(1, str_owner_address.size() - 2);
	Binary sz_owner_address = Binary::decode(str_owner_address);
	string str_to_address = oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag][str_value_tag]["to_address"].ToString();
	str_to_address = str_to_address.substr(1, str_to_address.size() - 2);
	Binary sz_to_address = Binary::decode(str_to_address);
	uint64 amount = 0;
	oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag][str_value_tag].Get("amount", amount);

	tronProtocol::TransferContract transfer_contract = tronProtocol::TransferContract();
	transfer_contract.set_owner_address(sz_owner_address.data(), sz_owner_address.size());
	transfer_contract.set_to_address(sz_to_address.data(), sz_to_address.size());
	transfer_contract.set_amount(amount);

	int transfer_contract_size = transfer_contract.ByteSize();
	Binary sz_transfer_contract(transfer_contract_size);
	transfer_contract.SerializeToArray(sz_transfer_contract.data(), transfer_contract_size);
	lp_any->set_value(sz_transfer_contract.data(), transfer_contract_size);

	int raw_data_size = raw_data.ByteSize();
	Binary sz_raw_data(raw_data_size);
	raw_data.SerializeToArray(sz_raw_data.data(), raw_data_size);
	string str_raw_data = Binary::encode(sz_raw_data);
	// cout << str_raw_data << endl;
	// string str_tx_hash = sha256_hash(str_raw_data);
	// cout << str_tx_hash << endl;
	return str_raw_data;
}

string make_unsigned_tx_freeze_from_json(const string &str_unsigned_tx_freeze_json) {
	if ("" == str_unsigned_tx_freeze_json)
		return std::string("");

	neb::CJsonObject oJson(str_unsigned_tx_freeze_json);
	tronProtocol::raw raw_data;

	// ref_block_bytes ref_block_hash
	string str_raw_data_tag = "raw_data";
	string str_ref_block_bytes = oJson[str_raw_data_tag]["ref_block_bytes"].ToString();
	// remove double quotation mark
	str_ref_block_bytes = str_ref_block_bytes.substr(1, str_ref_block_bytes.size() - 2);
	Binary sz_ref_block_bytes = Binary::decode(str_ref_block_bytes);
	raw_data.set_ref_block_bytes(sz_ref_block_bytes.data(), sz_ref_block_bytes.size());
	string str_ref_block_hash = oJson[str_raw_data_tag]["ref_block_hash"].ToString();
	str_ref_block_hash = str_ref_block_hash.substr(1, str_ref_block_hash.size() - 2);
	Binary sz_ref_block_hash = Binary::decode(str_ref_block_hash);
	raw_data.set_ref_block_hash(sz_ref_block_hash.data(), sz_ref_block_hash.size());

	// expiration fee_limit timestamp
	uint64 expiration = 0, fee_limit = 0, timestamp = 0;
	oJson[str_raw_data_tag].Get("expiration", expiration);
	raw_data.set_expiration(expiration);
	oJson[str_raw_data_tag].Get("fee_limit", fee_limit);
	raw_data.set_fee_limit(fee_limit);
	oJson[str_raw_data_tag].Get("timestamp", timestamp);
	raw_data.set_timestamp(timestamp);

	// contract
	string str_contract_tag = "contract", str_parameter_tag = "parameter";
	string str_value_tag = "value";
	string str_type = oJson[str_raw_data_tag][str_contract_tag][0]["type"].ToString();
	str_type = str_type.substr(1, str_type.size() - 2);
	tronProtocol::Contract_ContractType type = g_map_string_2_contract_type[str_type];
	tronProtocol::Contract *lp_contract = raw_data.mutable_contract();
	lp_contract->set_type(type);

	google::protobuf::Any *lp_any = lp_contract->mutable_parameter();
	string str_type_url = oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag]["type_url"].ToString();
	str_type_url = str_type_url.substr(1, str_type_url.size() - 2);
	lp_any->set_type_url(str_type_url.c_str());

	// owner address
	string str_owner_address = oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag][str_value_tag]["owner_address"].ToString();
	str_owner_address = str_owner_address.substr(1, str_owner_address.size() - 2);
	Binary sz_owner_address = Binary::decode(str_owner_address);
	// freeze resource
	string str_freeze_resource = oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag][str_value_tag]["resource"].ToString();	
	tronProtocol::ResourceCode resource_type = tronProtocol::BANDWIDTH;
	if ("" != str_freeze_resource) {
		str_freeze_resource = _to_upper(str_freeze_resource);
		if (str_freeze_resource.size() > 2) {
			str_freeze_resource = str_freeze_resource.substr(1, str_freeze_resource.size() - 2);
			if ("ENERGY" == str_freeze_resource) {
				resource_type = tronProtocol::ENERGY;
			}
		}
	}
	// freeze balance and freeze duration
	uint64 frozen_duration = 0, frozen_balance = 0;
	oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag][str_value_tag].Get("frozen_duration", frozen_duration);
	oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag][str_value_tag].Get("frozen_balance", frozen_balance);

	tronProtocol::FreezeBalanceContract freeze_contract = tronProtocol::FreezeBalanceContract();
	freeze_contract.set_owner_address(sz_owner_address.data(), sz_owner_address.size());
	freeze_contract.set_frozen_duration(frozen_duration);
	freeze_contract.set_frozen_balance(frozen_balance);
	freeze_contract.set_resource(resource_type);

	int freeze_contract_size = freeze_contract.ByteSize();
	Binary sz_freeze_contract(freeze_contract_size);
	freeze_contract.SerializeToArray(sz_freeze_contract.data(), freeze_contract_size);
	lp_any->set_value(sz_freeze_contract.data(), freeze_contract_size);

	int raw_data_size = raw_data.ByteSize();
	Binary sz_raw_data(raw_data_size);
	raw_data.SerializeToArray(sz_raw_data.data(), raw_data_size);
	string str_raw_data = Binary::encode(sz_raw_data);
	// cout << str_raw_data << endl;
	// string str_tx_hash = sha256_hash(str_raw_data);
	// cout << str_tx_hash << endl;
	return str_raw_data;
}

string make_unsigned_tx_unfreeze_from_json(const string &str_unsigned_tx_unfreeze_json) {
	if ("" == str_unsigned_tx_unfreeze_json)
		return std::string("");

	neb::CJsonObject oJson(str_unsigned_tx_unfreeze_json);
	tronProtocol::raw raw_data;

	// ref_block_bytes ref_block_hash
	string str_raw_data_tag = "raw_data";
	string str_ref_block_bytes = oJson[str_raw_data_tag]["ref_block_bytes"].ToString();
	// remove double quotation mark
	str_ref_block_bytes = str_ref_block_bytes.substr(1, str_ref_block_bytes.size() - 2);
	Binary sz_ref_block_bytes = Binary::decode(str_ref_block_bytes);
	raw_data.set_ref_block_bytes(sz_ref_block_bytes.data(), sz_ref_block_bytes.size());
	string str_ref_block_hash = oJson[str_raw_data_tag]["ref_block_hash"].ToString();
	str_ref_block_hash = str_ref_block_hash.substr(1, str_ref_block_hash.size() - 2);
	Binary sz_ref_block_hash = Binary::decode(str_ref_block_hash);
	raw_data.set_ref_block_hash(sz_ref_block_hash.data(), sz_ref_block_hash.size());

	// expiration fee_limit timestamp
	uint64 expiration = 0, fee_limit = 0, timestamp = 0;
	oJson[str_raw_data_tag].Get("expiration", expiration);
	raw_data.set_expiration(expiration);
	oJson[str_raw_data_tag].Get("fee_limit", fee_limit);
	raw_data.set_fee_limit(fee_limit);
	oJson[str_raw_data_tag].Get("timestamp", timestamp);
	raw_data.set_timestamp(timestamp);

	// contract
	string str_contract_tag = "contract", str_parameter_tag = "parameter";
	string str_value_tag = "value";
	string str_type = oJson[str_raw_data_tag][str_contract_tag][0]["type"].ToString();
	str_type = str_type.substr(1, str_type.size() - 2);
	tronProtocol::Contract_ContractType type = g_map_string_2_contract_type[str_type];
	tronProtocol::Contract *lp_contract = raw_data.mutable_contract();
	lp_contract->set_type(type);

	google::protobuf::Any *lp_any = lp_contract->mutable_parameter();
	string str_type_url = oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag]["type_url"].ToString();
	str_type_url = str_type_url.substr(1, str_type_url.size() - 2);
	lp_any->set_type_url(str_type_url.c_str());

	string str_owner_address = oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag][str_value_tag]["owner_address"].ToString();
	str_owner_address = str_owner_address.substr(1, str_owner_address.size() - 2);
	Binary sz_owner_address = Binary::decode(str_owner_address);
	string str_unfreeze_type = oJson[str_raw_data_tag][str_contract_tag][0][str_parameter_tag][str_value_tag]["resource"].ToString();
	tronProtocol::ResourceCode resource_type = tronProtocol::BANDWIDTH;
	if ("" != str_unfreeze_type) {
		str_unfreeze_type = _to_upper(str_unfreeze_type);
		if (str_unfreeze_type.size() > 2) {
			str_unfreeze_type = str_unfreeze_type.substr(1, str_unfreeze_type.size() - 2);
			if ("ENERGY" == str_unfreeze_type) {
				resource_type = tronProtocol::ENERGY;
			}
		}
	}

	tronProtocol::UnfreezeBalanceContract unfreeze_contract = tronProtocol::UnfreezeBalanceContract();
	unfreeze_contract.set_owner_address(sz_owner_address.data(), sz_owner_address.size());
	unfreeze_contract.set_resource(resource_type);

	int unfreeze_contract_size = unfreeze_contract.ByteSize();
	Binary sz_unfreeze_contract(unfreeze_contract_size);
	unfreeze_contract.SerializeToArray(sz_unfreeze_contract.data(), unfreeze_contract_size);
	lp_any->set_value(sz_unfreeze_contract.data(), unfreeze_contract_size);

	// serialize
	int raw_data_size = raw_data.ByteSize();
	Binary sz_raw_data(raw_data_size);
	raw_data.SerializeToArray(sz_raw_data.data(), raw_data_size);
	string str_raw_data = Binary::encode(sz_raw_data);
	return str_raw_data;
}

string tx_hash(const string& str_unsign_tx)
{
	if ("" == str_unsign_tx)
		return string("");

	Binary sz_unsign_tx = Binary::decode(str_unsign_tx);
	Binary sz_sha256_hash = sha256_hash(sz_unsign_tx);
	return Binary::encode(sz_sha256_hash);
}

string sign_tx(const string &str_unsign_tx, const string &str_prikey)
{
    if (str_unsign_tx == "" || str_prikey.size() != 64)
        return string("");

    Binary sz_unsign_tx = Binary::decode(str_unsign_tx);
    Binary sz_hash = sha256_hash(sz_unsign_tx);
    Binary sz_prikey = Binary::decode(str_prikey);

    Binary sz_sign(mECC_BYTES * 2);
    int v = mECC_sign_forbc(sz_prikey.data(), sz_hash.data(), sz_sign.data());

    string str_sign = Binary::encode(sz_sign);
    str_sign += uint8_t_to_string(uint8_t(v));
    return str_sign;
}

static const string TRX_MESSAGE_HEADER = "\u0019TRON Signed Message:\n";
static const string ETH_MESSAGE_HEADER = "\u0019Ethereum Signed Message:\n";

string sign_message(const bool use_trx_header, const string &str_transaction , const string &str_prikey)
{
    if (str_transaction == "" || str_prikey.size() != 64)
        return string("");

    string fmt = use_trx_header?TRX_MESSAGE_HEADER:ETH_MESSAGE_HEADER;
    if (use_trx_header){
        fmt += "32";
    } else {
        char str_len[5] = {0};
        sprintf(str_len, "%d", (int)str_transaction.length() / 2);
        fmt += str_len;
    }
    Binary sz_unsign_tx = Binary(fmt) + Binary::decode(str_transaction);


    Binary sz_hash(32);
    keccak_256(sz_unsign_tx.data(), sz_unsign_tx.size(), sz_hash.data());
    Binary sz_prikey = Binary::decode(str_prikey);
    Binary sz_sign(mECC_BYTES * 2);
    int v = mECC_sign_forbc(sz_prikey.data(), sz_hash.data(), sz_sign.data());

    string str_sign = Binary::encode(sz_sign);
    str_sign += uint8_t_to_string(uint8_t(v + 27));
    return str_sign;
}


string make_sign_tx(const string &str_unsign_tx, const string &str_sign_data)
{
    if (str_unsign_tx == "" || str_sign_data.size() != 130)
        return string("");

    tronProtocol::Transaction tx;
    tronProtocol::raw *lp_raw_data = tx.mutable_raw_data();
    Binary sz_raw_data = Binary::decode(str_unsign_tx);
    lp_raw_data->ParseFromArray(sz_raw_data.data(), (int)sz_raw_data.size());

    Binary sz_sign_data = Binary::decode(str_sign_data);
    tx.set_signature(sz_sign_data.data(), sz_sign_data.size());

    int tx_size = tx.ByteSize();
    Binary sz_sign_tx(tx_size);
    tx.SerializeToArray(sz_sign_tx.data(), tx_size);
    string str_tx = Binary::encode(sz_sign_tx);
    return str_tx;
}

string trx10_to_json(const string &str_token_name, const string &str_sign_tx)
{
    if ("" == str_sign_tx || "" == str_token_name)
        return string("");

    tronProtocol::Transaction tx;
    Binary sz_sign_tx = Binary::decode(str_sign_tx);
    tx.ParseFromArray(sz_sign_tx.data(), (int)sz_sign_tx.size());

    // signature
    neb::CJsonObject oJson;
    string str_sign_tag = "signature";
    oJson.AddEmptySubArray(str_sign_tag);
    string *lp_sign = tx.mutable_signature();
    Binary sz_sign(lp_sign->c_str(), 65);
    string str_sign = Binary::encode(sz_sign);
    oJson[str_sign_tag].Add(str_sign);

    tronProtocol::raw raw_data = tx.raw_data();

    // tx hash
    int raw_data_size = raw_data.ByteSize();
    Binary sz_raw_data(raw_data_size);
    raw_data.SerializeToArray(sz_raw_data.data(), raw_data_size);
    Binary sz_tx_hash = sha256_hash(sz_raw_data);
    string str_tx_hash = Binary::encode(sz_tx_hash);
    oJson.Add("txID", str_tx_hash);

    string str_raw_data_tag = "raw_data";
    oJson.AddEmptySubObject(str_raw_data_tag);

    tronProtocol::Contract contract = raw_data.contract();
    google::protobuf::Any any = contract.parameter();

    string str_token = _to_upper(str_token_name);
    //string str_token(str_token_name);
    //transform(str_token_name.begin(), str_token_name.end(), str_token.begin(), toupper);

    tronProtocol::TransferContract transferContract;
    tronProtocol::TransferAssetContract transferAssetContract;
    Binary szTransferContract(any.value().c_str(), any.value().size());
    size_t iSize = szTransferContract.size();

    if ("TRX" == str_token)
        transferContract.ParseFromArray(szTransferContract.data(), (int)iSize);
    else
        transferAssetContract.ParseFromArray(szTransferContract.data(), (int)iSize);

    string str_contract_tag = "contract";
    oJson[str_raw_data_tag].AddEmptySubArray(str_contract_tag);

    string str_parameter = "parameter";
    string str_value = "value";
    neb::CJsonObject contract_json;
    contract_json.AddEmptySubObject(str_parameter);
    contract_json[str_parameter].AddEmptySubObject(str_value);

    // from, to, amount
    uint64 amount = 0;
    string str_owner_address_base16, str_to_address_base16;
    string str_asset_name_base16;
    if ("TRX" == str_token) {
        amount = (uint64)transferContract.amount();
        string str_owner_address = transferContract.owner_address();
        Binary sz_owner_address(str_owner_address.c_str(), str_owner_address.size());
        str_owner_address_base16 = Binary::encode(sz_owner_address);
        string str_to_address = transferContract.to_address();
        Binary sz_to_address(str_to_address.c_str(), str_to_address.size());
        str_to_address_base16 = Binary::encode(sz_to_address);
    }
    else {
        amount = (uint64)transferAssetContract.amount();
        string str_owner_address = transferAssetContract.owner_address();
        Binary sz_owner_address(str_owner_address.c_str(), str_owner_address.size());
        str_owner_address_base16 = Binary::encode(sz_owner_address);
        string str_to_address = transferAssetContract.to_address();
        Binary sz_to_address(str_to_address.c_str(), str_to_address.size());
        str_to_address_base16 = Binary::encode(sz_to_address);

        string str_asset_name = transferAssetContract.asset_name();
        Binary sz_asset_name(str_asset_name.c_str(), str_asset_name.size());
        str_asset_name_base16 = Binary::encode(sz_asset_name);
        contract_json[str_parameter][str_value].Add("asset_name", str_asset_name_base16);
    }

    contract_json[str_parameter][str_value].Add("amount", amount);
    contract_json[str_parameter][str_value].Add("owner_address", str_owner_address_base16);
    contract_json[str_parameter][str_value].Add("to_address", str_to_address_base16);

    // contract type and type_url
    string str_type_url = any.type_url();
    contract_json[str_parameter].Add("type_url", str_type_url);
    string str_type = str_type_url.substr(str_type_url.find_last_of('.') + 1);
    contract_json.Add("type", str_type);
    oJson[str_raw_data_tag][str_contract_tag].Add(contract_json);

    // ref_block_bytes and ref_block_hash
    string str_ref_block_bytes = raw_data.ref_block_bytes();
    Binary sz_ref_block_bytes(str_ref_block_bytes.c_str(), str_ref_block_bytes.size());
    string str_ref_block_bytes_base16 = Binary::encode(sz_ref_block_bytes);
    string str_ref_block_hash = raw_data.ref_block_hash();
    Binary sz_ref_block_hash(str_ref_block_hash.c_str(), str_ref_block_hash.size());
    string str_ref_block_hash_base16 = Binary::encode(sz_ref_block_hash);
    oJson[str_raw_data_tag].Add("ref_block_bytes", str_ref_block_bytes_base16);
    oJson[str_raw_data_tag].Add("ref_block_hash", str_ref_block_hash_base16);

    // expiration and timestamp
    uint64 expiration = (uint64)raw_data.expiration();
    oJson[str_raw_data_tag].Add("expiration", expiration);
    uint64 timestamp = (uint64)raw_data.timestamp();
    oJson[str_raw_data_tag].Add("timestamp", timestamp);

    string str_sign_tx_json = oJson.ToFormattedString();
    return str_sign_tx_json;
}

string trx20_to_json(const string &str_sign_tx)
{
    if ("" == str_sign_tx)
        return string("");

    tronProtocol::Transaction tx;
    Binary sz_sign_tx = Binary::decode(str_sign_tx);
    tx.ParseFromArray(sz_sign_tx.data(), (int)sz_sign_tx.size());

    // signature
    neb::CJsonObject oJson;
    string str_sign_tag = "signature";
    oJson.AddEmptySubArray(str_sign_tag);
    string *lp_sign = tx.mutable_signature();
    Binary sz_sign(lp_sign->c_str(), 65);
    string str_sign = Binary::encode(sz_sign);
    oJson[str_sign_tag].Add(str_sign);

    tronProtocol::raw raw_data = tx.raw_data();

    // tx hash
    int raw_data_size = raw_data.ByteSize();
    Binary sz_raw_data(raw_data_size);
    raw_data.SerializeToArray(sz_raw_data.data(), raw_data_size);
    Binary sz_tx_hash = sha256_hash(sz_raw_data);
    string str_tx_hash = Binary::encode(sz_tx_hash);
    oJson.Add("txID", str_tx_hash);

    string str_raw_data_tag = "raw_data";
    oJson.AddEmptySubObject(str_raw_data_tag);
    tronProtocol::Contract contract = raw_data.contract();
    google::protobuf::Any any = contract.parameter();

    tronProtocol::TriggerSmartContract trigger_smart_contract;
    Binary sz_trigger_smart_contract(any.value().c_str(), any.value().size());
    size_t i_size = sz_trigger_smart_contract.size();
    trigger_smart_contract.ParseFromArray(sz_trigger_smart_contract.data(), (int)i_size);

    string str_contract_tag = "contract";
    oJson[str_raw_data_tag].AddEmptySubArray(str_contract_tag);

    string str_parameter = "parameter";
    string str_value = "value";
    neb::CJsonObject contract_json;
    contract_json.AddEmptySubObject(str_parameter);
    contract_json[str_parameter].AddEmptySubObject(str_value);

    // data, owner_address, contract_address
    string str_data = trigger_smart_contract.data();
    Binary sz_data(str_data.c_str(), str_data.size());
    string str_data_base16 = Binary::encode(sz_data);
    string str_owner_address = trigger_smart_contract.owner_address();
    Binary sz_owner_address(str_owner_address.c_str(), str_owner_address.size());
    string str_owner_address_base16 = Binary::encode(sz_owner_address);
    string str_contract_address = trigger_smart_contract.contract_address();
    Binary sz_contract_address(str_contract_address.c_str(), str_contract_address.size());
    string str_contract_address_base16 = Binary::encode(sz_contract_address);
    uint64 call_value = (uint64)trigger_smart_contract.call_value();

    contract_json[str_parameter][str_value].Add("data", str_data_base16);
    contract_json[str_parameter][str_value].Add("owner_address", str_owner_address_base16);
    contract_json[str_parameter][str_value].Add("contract_address", str_contract_address_base16);
    contract_json[str_parameter][str_value].Add("call_value", call_value);

    // contract type and type_url
    string str_type_url = any.type_url();
    contract_json[str_parameter].Add("type_url", str_type_url);
    tronProtocol::Contract_ContractType type = contract.type();
    string str_type = g_map_contract_type_2_string[type];
    //string str_type = str_type_url.substr(str_type_url.find_last_of('.') + 1);
    contract_json.Add("type", str_type);
    oJson[str_raw_data_tag][str_contract_tag].Add(contract_json);

    // ref_block_bytes and ref_block_hash
    string str_ref_block_bytes = raw_data.ref_block_bytes();
    Binary sz_ref_block_bytes(str_ref_block_bytes.c_str(), str_ref_block_bytes.size());
    string str_ref_block_bytes_base16 = Binary::encode(sz_ref_block_bytes);
    string str_ref_block_hash = raw_data.ref_block_hash();
    Binary sz_ref_block_hash(str_ref_block_hash.c_str(), str_ref_block_hash.size());
    string str_ref_block_hash_base16 = Binary::encode(sz_ref_block_hash);
    oJson[str_raw_data_tag].Add("ref_block_bytes", str_ref_block_bytes_base16);
    oJson[str_raw_data_tag].Add("ref_block_hash", str_ref_block_hash_base16);

    // expiration fee_limit and timestamp
    uint64 expiration = (uint64)raw_data.expiration();
    oJson[str_raw_data_tag].Add("expiration", expiration);
    uint64 fee_limit = (uint64)raw_data.fee_limit();
    oJson[str_raw_data_tag].Add("fee_limit", fee_limit);
    uint64 timestamp = (uint64)raw_data.timestamp();
    oJson[str_raw_data_tag].Add("timestamp", timestamp);

    string str_sign_tx_json = oJson.ToFormattedString();
    return str_sign_tx_json;
}
} // namespace TRONAPI

namespace TRX_API
{

typedef struct TRX_TX_INTERNAL
{
    bool is_trc10;
    string token_name;
    string tx_unsigned;
    string signature;
} TRX_TX_INTERNAL;

/*
    1. TRC10，包括TRX本身
    {
      "block_id": "141A34523C4......1BF2D35",
      "token": "TRX",
      "from": "TPiQeYwqPosuun4DB3UJ5sE7ajnray58nc",
      "to": "TMxbZ97qmYc9sqhKznrbsAeN2B1FMN3B6R",
      "amount": 500
    }

    2. TRC20
    {
      "block_id": "141A34523C4......1BF2D35",
      "contract": "TP52amYqPosutn41B3tJ7sE7ajnray50nd",
      "from": "TPiQeYwqPosuun4DB3UJ5sE7ajnray58nc",
      "to": "TMxbZ97qmYc9sqhKznrbsAeN2B1FMN3B6R",
      "amount": 500
    }

    3. From JSON
    {
      "transaction": {a transaction json object}
    }
*/
int TxCreate(_in const char *json_param, _out TRX_TX **tx)
{
    if (json_param == nullptr)
        return -1;

    Json::Reader jReader;
    Json::Value jRoot;

    if (!jReader.parse(json_param, jRoot))
        return -1;
    if (!jRoot.isObject())
        return -1;

    bool is_trc10 = false;
    string str_token;
    string ret;

    // 处理方案3的情况
    Json::Value j_transaction = jRoot["transaction"];
    if (!j_transaction.isNull()) {
        if (!j_transaction.isObject())
            return -1;
        Json::FastWriter jWriter;
        string param = jWriter.write(j_transaction);
        ret = TRONAPI::make_unsigned_tx_from_json(param);
    }
    else {
        Json::Value j_block_id = jRoot["block_id"];
        Json::Value j_from_addr = jRoot["from"];
        Json::Value j_to_addr = jRoot["to"];
        Json::Value j_amount = jRoot["amount"];
        if (j_block_id.isNull() || !j_block_id.isString()
            || j_from_addr.isNull() || !j_from_addr.isString()
            || j_to_addr.isNull() || !j_to_addr.isString()
            || j_amount.isNull() || !j_amount.isUInt64()
            ) {
            return -1;
        }

        Json::Value j_token = jRoot["token"];
        Json::Value j_contract = jRoot["contract"];

        string str_block_id = j_block_id.asString();
        string str_from_address = j_from_addr.asString();
        string str_to_address = j_to_addr.asString();
        uint64 amount = j_amount.asUInt64();

        if (!j_token.isNull()) {
            is_trc10 = true;
            str_token = j_token.asString();
            if (str_token.empty())
                return -1;
            ret = TRONAPI::make_unsigned_tx_trc10(str_token, str_block_id, str_from_address,
                str_to_address, amount);
        }
        else if (!j_contract.isNull()) {
            string str_contract = j_contract.asString();
            if (str_contract.empty())
                return -1;
            ret = TRONAPI::make_unsigned_tx_trc20(str_block_id, str_contract, str_from_address,
                str_to_address, amount);
        }
    }

    TRX_TX_INTERNAL *_tx = new TRX_TX_INTERNAL;
    _tx->is_trc10 = is_trc10;
    if (is_trc10)
        _tx->token_name = str_token;
    _tx->tx_unsigned = ret;
    *tx = (TRX_TX *)_tx;
    return 0;
}

string GetUnsignedTx(_in TRX_TX *tx)
{
    if (tx == nullptr)
        return "";
    TRX_TX_INTERNAL *_tx = (TRX_TX_INTERNAL *)tx;
    return _tx->tx_unsigned;
}

/*
{
  "key": "11eadf3ae12028ddabc765b16cab6e622080dacf69e0f1eae81ba8ceba5fff15"
}
*/
int TxSign(_in TRX_TX *tx, _in const char *json_param)
{
    if (tx == nullptr)
        return -1;
    TRX_TX_INTERNAL *_tx = (TRX_TX_INTERNAL *)tx;

    if (json_param == nullptr)
        return -1;

    Json::Reader jReader;
    Json::Value jRoot;

    if (!jReader.parse(json_param, jRoot))
        return -1;
    if (!jRoot.isObject())
        return -1;

    Json::Value j_key = jRoot["key"];
    if (j_key.isNull() || !j_key.isString()) {
        return -1;
    }
    string pri_key = j_key.asString();

    string ret = TRONAPI::sign_tx(_tx->tx_unsigned, pri_key);
    if (ret.empty())
        return -1;

    _tx->signature = ret;
    return 0;
}

int TxSetSignature(_in TRX_TX *tx, _in string signature)
{
    if (tx == nullptr)
        return -1;
    TRX_TX_INTERNAL *_tx = (TRX_TX_INTERNAL *)tx;

    if (signature.empty())
        return -2;

    _tx->signature = signature;
    return 0;
}

/*
{
  "auth_code": "123456"
}
*/
int TxGetTransacton(_in const TRX_TX *tx, _in const char *json_param, _out char **result)
{
    if (tx == nullptr)
        return -1;
    TRX_TX_INTERNAL *_tx = (TRX_TX_INTERNAL *)tx;

    string tx_signed = TRONAPI::make_sign_tx(_tx->tx_unsigned, _tx->signature);
    if (tx_signed.empty())
        return 1;

    string ret;

    if (_tx->is_trc10) {
        ret = TRONAPI::trx10_to_json(_tx->token_name, tx_signed);
    }
    else {
        ret = TRONAPI::trx20_to_json(tx_signed);
    }

    if (tx_signed.empty())
        return 1;

    size_t len = ret.length() + 1;
    *result = (char *)tx::alloc(len);
    memset(*result, 0, len);
    memcpy(*result, ret.c_str(), len);

    return 0;
}

void TxDestory(_in TRX_TX **tx)
{
    if (NULL != tx) {
        TRX_TX_INTERNAL *_tx = (TRX_TX_INTERNAL *)(*tx);
        delete _tx;
        *tx = NULL;
    }
}

/*
{
  "seed": "3423423421231424"
}
*/
int TxGetPriKey(_in const char *json_param, _out char **result)
{
    if (json_param == nullptr)
        return -1;

    Json::Reader jReader;
    Json::Value jRoot;

    if (!jReader.parse(json_param, jRoot))
        return -1;
    if (!jRoot.isObject())
        return -1;

    Json::Value j_seed = jRoot["seed"];
    if (j_seed.isNull() || !j_seed.isString()) {
        return -1;
    }
    string seed = j_seed.asString();
	string priKey = bip44_get_private_key(seed, "m/44'/195'/0'/0/0");

    size_t len = priKey.length() + 1;
    *result = (char *)tx::alloc(len);
    memset(*result, 0, len);
    memcpy(*result, priKey.c_str(), len);

    return 0;
}

/*
1. 由种子计算公钥
{
  "compress": false,
  "seed": "3423423421231424"
}

2. 由私钥计算公钥
{
  "compress": false,
  "pri_key": "11eadf3ae12028ddabc765b16cab6e622080dacf69e0f1eae81ba8ceba5fff15"
}
*/
int TxGetPubKey(_in const char *json_param, _out char **result)
{
    if (json_param == nullptr)
        return -1;

    Json::Reader jReader;
    Json::Value jRoot;

    if (!jReader.parse(json_param, jRoot))
        return -1;
    if (!jRoot.isObject())
        return -1;

    Json::Value j_compress = jRoot["compress"];
    Json::Value j_key = jRoot["pri_key"];
    if (j_compress.isNull() || !j_compress.isBool()
        || j_key.isNull() || !j_key.isString()) {
        return -1;
    }
    bool is_compress = j_compress.asBool();
    string pri_key = j_key.asString();

	string pub_key = TRONAPI::get_public_key(pri_key, is_compress);
    /*if (is_compress) {
        pub_key = TRONAPI::get_pubkey_compress(pri_key);
    }
    else {
        pub_key = TRONAPI::get_pubkey_nocompress(pri_key);
    }*/

    size_t len = pub_key.length() + 1;
    *result = (char *)tx::alloc(len);
    memset(*result, 0, len);
    memcpy(*result, pub_key.c_str(), len);

    return 0;
}

// 根据公钥计算出地址
// pubkey可以是压缩格式(33字节)或非压缩格式(65字节)
/*
{
  "main_net": true,
  "pub_key": "3423423421231424"
}
*/
int TxGetAddress(_in const char *json_param, _out char **result)
{
    if (json_param == nullptr)
        return -1;

    Json::Reader jReader;
    Json::Value jRoot;

    if (!jReader.parse(json_param, jRoot))
        return -1;
    if (!jRoot.isObject())
        return -1;

    Json::Value j_main_net = jRoot["main_net"];
    Json::Value j_key = jRoot["pub_key"];
    if (j_main_net.isNull() || !j_main_net.isBool()
        || j_key.isNull() || !j_key.isString()
        ) {
        return -1;
    }
    bool is_main_net = j_main_net.asBool();
    string pub_key = j_key.asString();

    if (pub_key.length() == 33 * 2) {
        string _tmp = TRONAPI::decompress_pub_key(pub_key);
        if (_tmp.empty())
            return -1;
        else
            pub_key = _tmp;
    }
    else if ((pub_key.length() == 128) || (pub_key.length() == 130))
	{
        // ok.
    }
    else {
        return -1;
    }

    string addr = TRONAPI::get_address(pub_key, is_main_net);
    size_t len = addr.length() + 1;
    *result = new char[len];
    memset(*result, 0, len);
    memcpy(*result, addr.c_str(), len);

    return 0;
}

string MakeSignedTx(_in const TRX_TX *tx, _in const string& str_sign_data)
{
    if (tx == nullptr)
        return "";
    TRX_TX_INTERNAL *_tx = (TRX_TX_INTERNAL *)tx;
    return TRONAPI::make_sign_tx(_tx->tx_unsigned,str_sign_data);
}

} // namespace TRX_API