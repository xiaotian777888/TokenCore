#ifndef CHAINMANAGER_H
#define CHAINMANAGER_H

#include "signedtransaction.h"
#include "eosnewaccount.h"
// #include <QByteArray>

namespace ChainManager {
    bool createTransaction(SignedTransaction* stx, const std::string& contract, const std::string& actionName,
        const std::string& hexData, const std::vector<std::string>& permissions, const Json::Value& info);

//     bool createTransactionFromJson(
//         SignedTransaction* stx,
//         const std::string& expiration,
//         u32 ref_block_num,
//         u32 ref_block_prefix,
//         const Json::Value& jroot);
    bool createTransactionFromJson(SignedTransaction* stx, const Json::Value& jroot);

    void setTransactionHeaderInfo(SignedTransaction& signedTrx, const Json::Value& info);
    void setTransactionHeaderInfo(SignedTransaction& signedTrx, const std::string& expiration, u32 ref_block_num, u32 ref_block_prefix);

    void addAction(SignedTransaction& signedTrx, const std::string& contract, const std::string& actionName, const std::string& hexData, const std::vector<std::string>& permissions);

    std::vector<std::string> getActivePermission(const std::string& accountName);
    std::vector<std::string> getOwnerPermission(const std::string& accountName);

    void ValidateSignature(const std::string& raw, const std::string& signature, const std::string& pubKey, const std::string& chain_id, std::string& result);
}

#endif // CHAINMANAGER_H
