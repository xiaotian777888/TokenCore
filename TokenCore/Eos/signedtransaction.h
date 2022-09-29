#ifndef SIGNEDTRANSACTION_H
#define SIGNEDTRANSACTION_H

#include "transaction.h"

class SignedTransaction : public Transaction
{
public:
    SignedTransaction();

//     virtual QJsonValue toJson();
//     virtual void fromJson(const QJsonValue& value);
    virtual Json::Value toJson() const;
    virtual void fromJson(const Json::Value& value);

    vector<string> getSignatures() const;
    vector<string> getCtxFreeData() const;
    void setSignatures(const vector<string>& signatures);

    void sign(const Binary& pri_key, const Binary& chain_id);
    //bool signTest(const Binary& signature, const Binary& pubKey, const Binary& chain_id);

    Binary getDigestForSignature(const Binary& chain_id);
    void setSignatures(Binary& signatures);

private:
    vector<string> signatures;
    vector<string> context_free_data;
};

#endif // SIGNEDTRANSACTION_H
