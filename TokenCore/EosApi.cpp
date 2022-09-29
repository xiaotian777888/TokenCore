//#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <json/json.h>

#include "EosApi.h"

#include "crypto/bip.h"
#include "crypto/bx_str.h"
#include "crypto/tx_util.h"

#include "Eos/eosapi_utils.h"

#include "Eos/chainmanager.h"
#include "Eos/signedtransaction.h"
#include "Eos/packedtransaction.h"
#include "Eos/typename.h"
#include "Eos/eosbytewriter.h"

#include "Eos/eos_key_encode.h"


#include <uEcc/macroECC.h>
#include "Tron/Transaction.pb.h"
#include "crypto/base58.h"
#include "crypto/utility_tools.h"
#include "crypto/keccak256.h"
#include "crypto/ripemd160.h"
#include "crypto/hash.h"
#include "Tron/CJsonObject.hpp"

struct EOS_TX_INTERNAL {
    string chain_id;
    SignedTransaction stx;
};

namespace EOSAPI {

bool validate_account(string account)
{
    if (account.size() > 12)
        return false;

    auto is_account = [](const char c) {
        return ('1' <= c && c <= '5') || ('a' <= c && c <= 'z') || ('.' == c);
    };

    if (!all_of(account.begin(), account.end(), is_account))
        return false;

    return true;
}

string get_private_key(const string& seed)
{
	string private_key = bip44_get_private_key(seed, "m/44'/194'/0'/0/0");
	eos_key key = eos_key(Binary::decode(private_key));
	return key.get_wif_private_key();
}

string get_public_key(const string& private_key)
{
	return eos_key::get_eos_public_key_by_wif(private_key);
}

int make_unsign_tx(
        _out EOS_TX **tx,
        _in const bool owner,
        _in const string &code,
        _in const string &action,
        _in const string &account,
        _in const string &binargs,
        _in const string &chain_info
) {
    if (tx == NULL)
        return BXRET_BAD_PARAM;

    Json::Reader jreader;
    Json::Value jinfo;
    if (!jreader.parse(chain_info, jinfo))
        return BXRET_INTERNAL;
    if (!jinfo.isObject())
        return BXRET_INTERNAL;
    if (jinfo["chain_id"].isNull() || !jinfo["chain_id"].isString())
        return BXRET_INTERNAL;

    EOS_TX_INTERNAL *_tx = new EOS_TX_INTERNAL;

    _tx->chain_id = jinfo["chain_id"].asString();

    // create a package for sign.
    //SignedTransaction stx = ChainManager::createTransaction(code, action, binargs, ChainManager::getActivePermission(from), jinfo);
    ChainManager::createTransaction(&_tx->stx, code, action, binargs,
                                    owner ? ChainManager::getOwnerPermission(account)
                                          : ChainManager::getActivePermission(account), jinfo);

    *tx = (EOS_TX *) _tx;

#if 0
    //vector<unsigned char>pri;
    Binary pri;

    // for apexluna3333
    char* pkey = "5Hpp8tDpcqdtx9yWGT6HRGomZeoutTEpWnUkRg6KKqrWEASmJF5";
    // for mengchen1234
    //char* pkey = "5JKg6LU2z5oceAggunNEciHmedZxHfMM5EDNoxNNJVjRocMqeFs";


    if (!bx_eos_wif2bin(pkey, pri))
        return BXRET_INTERNAL;

    stx.sign(pri, Binary::decode(chain_id));

//     Json::Value jobj = stx.toJson();
//     printf("signed:\n");
//     printf(jsw.write(jobj).c_str());
//
//     Json::Value jsend;
//     jsend["compression"] = "none";
//     jsend["transaction"] = jobj;

    PackedTransaction ptx(stx, "none");
    Json::Value jsend = ptx.toJson();

    vector<string> signatures;
    signatures = stx.getSignatures();
    Json::Value signatureArr(Json::arrayValue);
    for (int i = 0; i < (int)signatures.size(); ++i) {
        signatureArr.append(Json::Value(signatures.at(i)));
    }

    jsend["signatures"] = signatureArr;


    printf("send:\n");
    printf(jsw.write(jsend).c_str());


    printf("press any key to send to EOS net...");
    getchar();

    Json::FastWriter jfw;

    btxon::bxStrA url("http://jungle.eosbcn.com:8080/v1/");
    url += "chain/push_transaction";

    btxon::bxStrA body;
    if (!bx_http_post(url, jfw.write(jsend), body))
        return BXRET_INTERNAL;

    Json::Value jret;
    if (!jreader.parse(body.c_str(), jret))
        return BXRET_INTERNAL;

    printf("push result:\n");
    printf(jsw.write(jret).c_str());


    if (!jret.isObject())
        return BXRET_INTERNAL;

#endif

    return BXRET_OK;
}

int make_unsign_tx_from_json(
    _out EOS_TX** tx,
    _in const char* chain_id,
    _in const char* json_raw
) {
    if (tx == NULL)
        return BXRET_BAD_PARAM;

    Json::Reader jreader;
    Json::Value jroot;
    if (!jreader.parse(json_raw, jroot))
        return BXRET_BAD_PARAM;
    if (!jroot.isObject())
        return BXRET_BAD_PARAM;

    EOS_TX_INTERNAL* _tx = new EOS_TX_INTERNAL;
    _tx->chain_id = chain_id;
    ChainManager::createTransactionFromJson(&_tx->stx, jroot);

    *tx = (EOS_TX*)_tx;
    return BXRET_OK;
}

int sign_tx(_in EOS_TX *tx, _in const string private_key) {
    Binary pri;
    if (!bx_eos_wif2bin(private_key, pri))
        return BXRET_INTERNAL;

    EOS_TX_INTERNAL *_tx = (EOS_TX_INTERNAL *) tx;
    _tx->stx.sign(pri, Binary::decode(_tx->chain_id));

    return BXRET_OK;
}

int add_action(_in EOS_TX *tx, _in const bool owner, _in const string &code, _in const string &action,
           _in const string &account, _in const string &binargs) {
    EOS_TX_INTERNAL *_tx = (EOS_TX_INTERNAL *) tx;

    ChainManager::addAction(_tx->stx, code, action, binargs,
                            owner ? ChainManager::getOwnerPermission(account)
                                  : ChainManager::getActivePermission(account));
    return 0;
}

void release(_in EOS_TX **tx) {
    if (NULL != tx) {
        EOS_TX_INTERNAL *_tx = (EOS_TX_INTERNAL *) (*tx);
        delete _tx;
        *tx = NULL;
    }
}

Binary get_data(_in const EOS_TX *tx) {
    Binary ret_data;

    if (NULL == tx)
        return ret_data;

    EOS_TX_INTERNAL *_tx = (EOS_TX_INTERNAL *) tx;
    ret_data = _tx->stx.getDigestForSignature(Binary::decode(_tx->chain_id));
    return ret_data;
}

string make_sign_tx(_in const EOS_TX *tx, _in Binary &sign_data) {
    string result;

    if (NULL == tx)
        return result;

    EOS_TX_INTERNAL *_tx = (EOS_TX_INTERNAL *) tx;

    if (sign_data.size() == 65)
        _tx->stx.setSignatures(sign_data);

    PackedTransaction ptx(_tx->stx, "none");
    Json::Value jsend = ptx.toJson();

    vector<string> signatures;
    signatures = _tx->stx.getSignatures();
    Json::Value signatureArr(Json::arrayValue);
    for (int i = 0; i < (int) signatures.size(); ++i) {
        signatureArr.append(Json::Value(signatures.at(i)));
    }

    jsend["signatures"] = signatureArr;

    Json::StyledWriter jsw;

    result = jsw.write(jsend);
    return result;
}

//=============================================================
// C Interface.
//=============================================================

/*
    1. normal
    {
      "owner": false,
      "code": "eosio.token",
      "action: "transfer",
      "account: "apexluna3333",
      "bin_args": "30c61866ead8553520841066ead855350c3000000000000004454f5300000000117465737420454f53207472616e73666572"
      "chain_info": "...."
    }

    2. From JSON
    {
      "chain_id": "....",
      "raw_json": {a transaction json object}
    }
*/
int TxCreate(
        _in const char *json_param,
        _out EOS_TX **tx
) {
    if (json_param == nullptr)
        return -1;

    Json::Reader jReader;
    Json::Value jRoot;

    if (!jReader.parse(json_param, jRoot))
        return -1;
    if (!jRoot.isObject())
        return -1;

    // 处理方案2的情况
    Json::Value j_chain_id = jRoot["chain_id"];
    Json::Value j_raw_json = jRoot["raw_json"];
    if (!j_chain_id.isNull() && !j_raw_json.isNull()) {
        if (!j_chain_id.isString())
            return -1;
        if (!j_raw_json.isObject())
            return -1;
        Json::FastWriter jWriter;
        std::string chain_id = j_chain_id.asString();
        std::string param = jWriter.write(j_raw_json);
        return make_unsign_tx_from_json(tx, chain_id.c_str(), param.c_str());
    }

    Json::Value j_owner = jRoot["owner"];
    Json::Value j_code = jRoot["code"];
    Json::Value j_action = jRoot["action"];
    Json::Value j_account = jRoot["account"];
    Json::Value j_bin_args = jRoot["bin_args"];
    Json::Value j_chain_info = jRoot["chain_info"];

    if (j_owner.isNull() || !j_owner.isBool()
        || j_code.isNull() || !j_code.isString()
        || j_action.isNull() || !j_action.isString()
        || j_account.isNull() || !j_account.isString()
        || j_bin_args.isNull() || !j_bin_args.isString()
        || j_chain_info.isNull() || !j_chain_info.isString()
            ) {
        return -1;
    }

    bool is_owner = j_owner.asBool();
    std::string str_code = j_code.asString();
    std::string str_action = j_action.asString();
    std::string str_account = j_account.asString();
    std::string str_bin_args = j_bin_args.asString();
    std::string str_chain_info = j_chain_info.asString();

    return make_unsign_tx(tx, is_owner, str_code, str_action, str_account, str_bin_args, str_chain_info);
}

int TxAddAction(
        _in EOS_TX *tx,
        _in const char *json_param
) {
    if(tx == nullptr)
        return -1;
    if (json_param == nullptr)
        return -1;

    Json::Reader jReader;
    Json::Value jRoot;

    if (!jReader.parse(json_param, jRoot))
        return -1;
    if (!jRoot.isObject())
        return -1;

    Json::Value j_owner = jRoot["owner"];
    Json::Value j_code = jRoot["code"];
    Json::Value j_action = jRoot["action"];
    Json::Value j_account = jRoot["account"];
    Json::Value j_bin_args = jRoot["bin_args"];

    if (j_owner.isNull() || !j_owner.isBool()
        || j_code.isNull() || !j_code.isString()
        || j_action.isNull() || !j_action.isString()
        || j_account.isNull() || !j_account.isString()
        || j_bin_args.isNull() || !j_bin_args.isString()
            ) {
        return -1;
    }

    bool is_owner = j_owner.asBool();
    std::string str_code = j_code.asString();
    std::string str_action = j_action.asString();
    std::string str_account = j_account.asString();
    std::string str_bin_args = j_bin_args.asString();

    return add_action(tx, is_owner, str_code, str_action, str_account, str_bin_args);
}

int TxSign(
        _in EOS_TX *tx,
        _in const char *json_param
) {
    if (tx == nullptr)
        return -1;

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
    std::string pri_key = j_key.asString();

    return sign_tx(tx, pri_key);
}

/*
{
  "auth_code": "123456"
}
*/
int TxGetTransacton(
        _in const EOS_TX *tx,
        _in const char *json_param,
        _out char **result
) {
    if (tx == nullptr)
        return -1;

    Binary binEmpty;
    std::string ret = make_sign_tx(tx, binEmpty);
    if (ret.empty())
        return 1;

    size_t len = ret.length() + 1;
    *result = (char *)tx::alloc(len);
    memset(*result, 0, len);
    memcpy(*result, ret.c_str(), len);

    return 0;
}

/*
{
  "seed": "3423423421231424"
}
*/
int TxGetPriKey(
        _in const char *json_param,
        _out char **result
) {
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
    std::string seed = j_seed.asString();

	string private_key = bip44_get_private_key(seed, "m/44'/194'/0'/0/0");
	eos_key key = eos_key(Binary::decode(private_key));
    std::string priKey = key.get_wif_private_key();

    size_t len = priKey.length() + 1;
    *result = (char *) tx::alloc(len);
    memset(*result, 0, len);
    memcpy(*result, priKey.c_str(), len);

    return 0;
}

int TxGetPubKey(
        _in const char *wifPriKey,
        _out char **result
) {
    std::string ret = eos_key::get_eos_public_key_by_wif(wifPriKey);
    if(ret.empty())
        return -1;

    size_t len = ret.length() + 1;
    *result = (char *) tx::alloc(len);
    memset(*result, 0, len);
    memcpy(*result, ret.c_str(), len);
    return 0;
}

int TxLongToName(
        _in long val,
        _out char **result
) {
    std::string ret = TypeName::long_name_to_string(val);

    size_t len = ret.length() + 1;
    *result = (char *) tx::alloc(len);
    memset(*result, 0, len);
    memcpy(*result, ret.c_str(), len);
    return 0;
}


/*
 * {
    "publicKey": "EOS8iAdjPL94BnNstBf9unGiVUfHnQbzHkXeXbApZoVMoUYgBpGnV",
    "data": "eosio.token | EOS | 1 | yumenglei123 | whaleexchang | 115098689128 680835 | eosio.token | EOS | 0",
    "whatfor": "Withdraw EOS",
    "isHash": false
    }
 *
 * */
    string sign_eos_message(
            _in string str_msg_json,
            _in string str_prikey

    ){
        Json::Reader jReader;
        Json::Value jRoot;

        if (!jReader.parse(str_msg_json.c_str(), jRoot))
            return "";
        if (!jRoot.isObject())
            return "";

        Json::Value jPubkey = jRoot["publicKey"];
        Json::Value jData = jRoot["data"];
        Json::Value jIsHash = jRoot["isHash"];

        if(jPubkey.isNull() || !jPubkey.isString() || jData.isNull() || !jData.isString() || jIsHash.isNull() || !jIsHash.isBool())
            return "";

        string pubkey = jPubkey.asString();
        string data = jData.asString();
        bool is_data_hashed = jIsHash.asBool();

        Binary binToBeSign;
        binToBeSign.resize(data.length());
        memcpy(&binToBeSign[0], data.c_str(), data.length());

        Binary binHash = sha256_hash(binToBeSign);

        uint8_t signature[mECC_BYTES * 2] = { 0 };

        //Binary sz_prikey = Binary::decode(str_prikey);
        Binary pri;
        if (!bx_eos_wif2bin(str_prikey, pri))
            return "";

        int recId = mECC_sign_forbc(pri.data(), &binHash[0], signature);

        if (recId == -1) {
            return "";
        } else {
            unsigned char bin[65+4] = { 0 };
            int binlen = 65+4;
            int headerBytes = recId + 27 + 4;
            bin[0] = (unsigned char)headerBytes;
            memcpy(bin + 1, signature, mECC_BYTES * 2);

            unsigned char temp[67] = { 0 };
            memcpy(temp, bin, 65);
            memcpy(temp + 65, "K1", 2);

            Binary rmdhash = ripemd160(Binary(temp, 67));
            memcpy(bin + 1 +  mECC_BYTES * 2, rmdhash.data(), 4);

            string sigbin = encode_base58(Binary(bin, binlen));
            string sig = "SIG_K1_";
            sig += sigbin;

            return sig;
        }
    }

}
