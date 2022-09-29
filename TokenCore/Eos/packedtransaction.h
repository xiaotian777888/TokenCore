#ifndef PACKEDTRANSACTION_H
#define PACKEDTRANSACTION_H

#include <TokenCommon.h>
#include "chainbase.h"
#include "signedtransaction.h"

class PackedTransaction : public IFormater
{
private:
    enum class compress_type {
        COMPRESS_NONE,
        COMPRESS_ZLIB
    };

public:
    PackedTransaction(const SignedTransaction& signedTxn, const string& compress);

    virtual Json::Value toJson() const;
//    virtual void fromJson(const QJsonValue& value);

private:
    Binary packTxn(const SignedTransaction& txn, compress_type cmp);
	Binary packCtxFreeData(const vector<string>& ctxFreeData, compress_type ct);
	Binary compress(const Binary& src, compress_type cmp);

private:
    vector<string> signatures;
    string compression;
    string packed_context_free_data;
    string packed_trx;
};

#endif // PACKEDTRANSACTION_H
