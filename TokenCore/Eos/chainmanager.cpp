#include "chainmanager.h"
#include "action.h"
#include "signedtransaction.h"
#include "eos_key_encode.h"

namespace ChainManager {

bool createTransaction(SignedTransaction* stx, const string &contract, const string &actionName, const string &hexData, const vector<string> &permissions, const Json::Value& info) {
    if (stx == NULL)
        return false;
    //SignedTransaction txn;
    addAction(*stx, contract, actionName, hexData, permissions);
    setTransactionHeaderInfo(*stx, info);

    return true;
}

// bool createTransactionFromJson(
//     SignedTransaction* stx,
//     const string& expiration,
//     u32 ref_block_num,
//     u32 ref_block_prefix,
//     const Json::Value& jroot) {
//     if (stx == NULL)
//         return false;
// 
//     //addAction(*stx, contract, actionName, hexData, permissions);
//     stx->fromJson(jroot);
// 
//     setTransactionHeaderInfo(*stx, expiration, ref_block_num, ref_block_prefix);
//     return true;
// }

bool createTransactionFromJson(SignedTransaction* stx, const Json::Value& jroot) {
    if (stx == NULL)
        return false;

    //addAction(*stx, contract, actionName, hexData, permissions);
    stx->fromJson(jroot);

    //setTransactionHeaderInfo(*stx, expiration, ref_block_num, ref_block_prefix);
    return true;
}

vector<string> getActivePermission(const string &accountName) {
    vector<string> permissions;
    permissions.push_back(accountName + "@active");

    return permissions;
}

vector<string> getOwnerPermission(const string &accountName) {
    vector<string> permissions;
    permissions.push_back(accountName + "@owner");

    return permissions;
}

// void ValidateSignature(const string &raw, const string &signature, const string &pubKey, const string &chain_id, string &result)
// {
//     result = "unknown error";
// 
//     QJsonValue value = QJsonValue(QJsonDocument::fromJson(QByteArray::fromStdString(raw)).object());
//     if (value.isNull()) {
//         return;
//     }
// 
//     Binary pub = eos_key::get_public_key_by_eos_pub(pubKey);
//     Binary signatureHex(signature.begin(), signature.end());
// 
//     SignedTransaction sigedTxn;
//     sigedTxn.fromJson(value);
// 
//     if (sigedTxn.signTest(signatureHex, pub, Binary::decode(chain_id))) {
//         result = "Passed, signature and raw action match the public key!";
//     } else {
//         result = "Failed, signature and raw action do not match the public key!";
//     }
// }

void addAction(SignedTransaction &signedTrx, const string &contract, const string &actionName, const string &hexData, const vector<string> &permissions) {
    Action action(contract, actionName);
    action.setAuthorization(permissions);
    action.setData(hexData);

    signedTrx.addAction(action);
}

void setTransactionHeaderInfo(SignedTransaction &signedTrx, const Json::Value& info) {
    if (info.isNull() || !info.isObject()
        || info["last_irreversible_block_id"].isNull() || !info["last_irreversible_block_id"].isString()
        || info["head_block_time"].isNull() || !info["head_block_time"].isString()
        )
        return;

    //string headBlockId = info["head_block_id"].asString();
    string headBlockId = info["last_irreversible_block_id"].asString();
    string expiration = info["head_block_time"].asString();

    signedTrx.setReferenceBlock(headBlockId);
    signedTrx.setExpiration(expiration);
}

void setTransactionHeaderInfo(SignedTransaction& signedTrx, const string& expiration, u32 ref_block_num, u32 ref_block_prefix) {
    signedTrx.setExpiration(expiration);
    signedTrx.setReferenceBlock(ref_block_num, ref_block_prefix);
}

}
