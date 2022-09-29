#include "signedtransaction.h"

#include "eosbytewriter.h"
#include <crypto/base58.h>
#include <crypto/hash.h>
#include <crypto/ripemd160.h>
#include <crypto/sha256.h>

extern "C"
{
#include <uEcc/macroECC.h>
}

SignedTransaction::SignedTransaction()
{

}

// QJsonValue SignedTransaction::toJson()
// {
//     QJsonObject obj = Transaction::toJson().toObject();
//     QJsonArray signaturesArr, ctxFreeDataArr;
//     for (const auto& s : signatures) {
//         signaturesArr.append(QJsonValue(s.c_str()));
//     }
// 
//     for (const auto& d : context_free_data) {
//         ctxFreeDataArr.append(QJsonValue(d.c_str()));
//     }
// 
//     obj.insert("signatures", signaturesArr);
//     obj.insert("context_free_data", ctxFreeDataArr);
// 
//     return QJsonValue(obj);
// }
// 
Json::Value SignedTransaction::toJson() const
{
//     QJsonObject obj = Transaction::toJson().toObject();
//     QJsonArray signaturesArr, ctxFreeDataArr;
//     for (const auto& s : signatures) {
//         signaturesArr.append(QJsonValue(s.c_str()));
//     }
// 
//     for (const auto& d : context_free_data) {
//         ctxFreeDataArr.append(QJsonValue(d.c_str()));
//     }
// 
//     obj.insert("signatures", signaturesArr);
//     obj.insert("context_free_data", ctxFreeDataArr);

    //return QJsonValue(obj);

    Json::Value obj = Transaction::toJson();

    Json::Value signaturesArr(Json::arrayValue);
    Json::Value ctxFreeDataArr(Json::arrayValue);
    for (const auto& s : signatures) {
        signaturesArr.append(Json::Value(s.c_str()));
    }

    for (const auto& d : context_free_data) {
        ctxFreeDataArr.append(Json::Value(d.c_str()));
    }

    obj["signatures"] = signaturesArr;
    obj["context_free_data"] = ctxFreeDataArr;

    return obj;
}
// void SignedTransaction::fromJson(const QJsonValue &value)
// {
//     QJsonObject obj = value.toObject();
//     if (obj.isEmpty()) {
//         return;
//     }
// 
//     QJsonArray signaturesArr = obj.value("signatures").toArray();
//     if (!signaturesArr.isEmpty()) {
//         for (int i = 0; i < signaturesArr.size(); ++i) {
//             signatures.push_back(signaturesArr.at(i).toString().toStdString());
//         }
//     }
// 
//     QJsonArray ctxFreeDataArr = obj.value("context_free_data").toArray();
//     if (!ctxFreeDataArr.isEmpty()) {
//         for (int i = 0; i < ctxFreeDataArr.size(); ++i) {
//             context_free_data.push_back(ctxFreeDataArr.at(i).toString().toStdString());
//         }
//     }
// 
//     Transaction::fromJson(value);
// }

void SignedTransaction::fromJson(const Json::Value& jval)
{
    Json::Value signaturesArr = jval["signatures"];
    if (signaturesArr.isArray() && signaturesArr.size() > 0) {
        for (size_t i = 0; i < signaturesArr.size(); ++i) {
            signatures.push_back(signaturesArr[(int)i].asString());
        }
    }

    Json::Value ctxFreeDataArr = jval["context_free_data"];
    if (ctxFreeDataArr.isArray() && ctxFreeDataArr.size() > 0) {
        for (size_t i = 0; i < ctxFreeDataArr.size(); ++i) {
            context_free_data.push_back(ctxFreeDataArr[(int)i].asString());
        }
    }

    Transaction::fromJson(jval);
}

vector<string> SignedTransaction::getSignatures() const
{
    return signatures;
}

vector<string> SignedTransaction::getCtxFreeData() const
{
    return context_free_data;
}

void SignedTransaction::setSignatures(const vector<string> &signatures)
{
    this->signatures = signatures;
}

void SignedTransaction::sign(const Binary& pri_key, const Binary& chain_id)
{
	Binary packedBytes = getDigestForSignature(chain_id);

    uint8_t packedSha256[SHA256_DIGEST_LENGTH];
	SHA256_(packedBytes.data(), packedBytes.size(), packedSha256);

    uint8_t signature[mECC_BYTES * 2] = { 0 };

    int recId = mECC_sign_forbc(pri_key.data(), packedSha256, signature);


    if (recId == -1) {
        return;
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

        signatures.push_back(sig);
    }
}


#if 0
bool SignedTransaction::signTest(const Binary &signatureHex, const Binary &pubKey, const Binary& chain_id)
{
	Binary packedBytes = getDigestForSignature(chain_id);
    printf("data to be check:\n");
    bx_log_bin(&packedBytes[0], packedBytes.size());

    uint8_t packedSha256[SHA256_DIGEST_LENGTH];
	SHA256_(packedBytes.data(), packedBytes.size(), packedSha256);
    printf("hash to be check:\n");
    bx_log_bin(packedSha256, SHA256_DIGEST_LENGTH);

//     Binary signature = Binary::decode(string(signatureHex.begin(), signatureHex.end()));
//     signature.erase(signature.begin());   //remove headerBytes
//     return checkSignature(pubKey.data(), packedSha256, signature.data()) != -1;
    return checkSignature(pubKey.data(), packedSha256, signatureHex.data()) != -1;
}
#endif

Binary SignedTransaction::getDigestForSignature(const Binary& chain_id)
{
    EOSByteWriter writer;
    writer.putBytes(chain_id.data(), (int)chain_id.size());

    serialize(&writer);

	Binary free_data;
    for (auto str = context_free_data.cbegin(); str != context_free_data.cend(); ++str) {
        for (auto it = (*str).cbegin(); it != (*str).cend(); ++it) {
            free_data.push_back(*it);
        }
    }

    if (free_data.size())
	{
        string str(free_data.begin(), free_data.end());
		Binary bin = sha256_hash(Binary(str));
        writer.putBytes(bin.data(), (int)bin.size());
    }
	else
	{
		Binary bin(32);
		memset(&bin[0], 0, 32);
        writer.putBytes(bin.data(), (int)bin.size());
    }

    return writer.toBytes();
}

void SignedTransaction::setSignatures(Binary& bin_signatures) {
    if (bin_signatures.size() != 65)
        return;

    int recId = -1;
    uint8_t signature[mECC_BYTES * 2] = { 0 };
    memcpy(signature, &bin_signatures[0], 64);
    recId = bin_signatures[64];

    unsigned char bin[65 + 4] = { 0 };
    int binlen = 65 + 4;
    int headerBytes = recId + 27 + 4;
    bin[0] = (unsigned char)headerBytes;
    memcpy(bin + 1, signature, mECC_BYTES * 2);

    unsigned char temp[67] = { 0 };
    memcpy(temp, bin, 65);
    memcpy(temp + 65, "K1", 2);

	Binary rmdhash = ripemd160(Binary(temp, 67));
    memcpy(bin + 1 + mECC_BYTES * 2, rmdhash.data(), 4);

	string sigbin = encode_base58(Binary(bin, binlen));
    string sig = "SIG_K1_";
    sig += sigbin;

    signatures.push_back(sig);
}
