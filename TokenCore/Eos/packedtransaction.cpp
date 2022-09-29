#include "packedtransaction.h"
#include "eosbytewriter.h"

PackedTransaction::PackedTransaction(const SignedTransaction &signedTxn, const string &compress)
{
    compression = compress;
    signatures = signedTxn.getSignatures();

    compress_type ct;
    if (compress.compare("none") == 0) {
        ct = compress_type::COMPRESS_NONE;
    } else {
        ct = compress_type::COMPRESS_ZLIB;
    }

	packed_trx = Binary::encode(packTxn(signedTxn, ct));
    vector<string> ctxFreeData = signedTxn.getCtxFreeData();
	packed_context_free_data = Binary::encode(packCtxFreeData(ctxFreeData, ct));
}

Json::Value PackedTransaction::toJson() const
{
    Json::Value obj;
    obj["compression"] = Json::Value(compression);
    obj["packed_context_free_data"] = Json::Value(packed_context_free_data);
    obj["packed_trx"] = Json::Value(packed_trx);

    Json::Value signatureArr(Json::arrayValue);
    for (int i = 0; i < (int)signatures.size(); ++i) {
        signatureArr.append(Json::Value(signatures.at(i)));
    }

    obj["signatures"] = signatureArr;

    return obj;
}

// void PackedTransaction::fromJson(const QJsonValue &value)
// {
//     QJsonObject obj = value.toObject();
//     if (obj.isEmpty()) {
//         return;
//     }
// 
//     compression = obj.value("compression").toString().toStdString();
//     packed_context_free_data = obj.value("packed_context_free_data").toString().toStdString();
//     packed_trx = obj.value("packed_trx").toString().toStdString();
// 
//     QJsonArray signatureArr = obj.value("signatures").toArray();
//     if (!signatureArr.isEmpty()) {
//         for (int i = 0; i < signatureArr.size(); ++i)
//         signatures.push_back(signatureArr.at(i).toString().toStdString());
//     }
// }

Binary PackedTransaction::packCtxFreeData(const vector<string> &ctxFreeData, PackedTransaction::compress_type ct)
{
    EOSByteWriter writer;

    int ctxCnt = (int)ctxFreeData.size();
    writer.putVariableUInt(ctxCnt);
    if (!ctxCnt) {
        return writer.toBytes();
    }

    for (int i = 0; i < ctxCnt; ++i)
	{
        string hexStr = ctxFreeData.at(i);
		Binary bytes = Binary::decode(hexStr);

        writer.putBytes(bytes.data(), (int)bytes.size());
    }

    return compress(writer.toBytes(), ct);
}

Binary PackedTransaction::packTxn(const SignedTransaction &txn, PackedTransaction::compress_type ct)
{
    EOSByteWriter writer;
    txn.serialize(&writer);

    return compress(writer.toBytes(), ct);
}

Binary PackedTransaction::compress(const Binary &src, compress_type cmp)
{
    if (cmp == compress_type::COMPRESS_NONE || cmp != compress_type::COMPRESS_ZLIB) {
        return src;
    }

    // use zip to compress.
    // FIX ME!

	Binary dst;
    return dst;
}
