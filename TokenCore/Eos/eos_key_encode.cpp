#include "eos_key_encode.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <crypto/base58.h>
#include <crypto/ripemd160.h>
#include <crypto/sha256.h>

extern "C"
{

#include <uEcc/macroECC.h>

}

/////////////////////////////////////////////////

eos_key::eos_key() {
    Binary tmp;
    tmp.resize(32);
    for (int i = 0; i < 32; i++) {
        unsigned char c = (unsigned char) (rand() % 255);
        tmp[i] = c;
    }

    _init_with_binary(tmp);
}

eos_key::eos_key(const Binary &priKey) {
    _init_with_binary(priKey);
}

void eos_key::_init_with_binary(const Binary &priKey) {
    private_key = priKey;
//
//    unsigned char str[32] = {0};
//    for (int i = 0; i < 32; i++) {
//        unsigned char c = (unsigned char)(rand() % 255);
//        str[i] = c;
//        private_key.push_back(c);
//    }

    // Encode private key to wif.
    unsigned char result[37];
    result[0] = 0x80;
    unsigned char digest[32];
    string base;

    memcpy(result + 1, &priKey[0], 32);
    SHA256_(result, 33, digest);
    SHA256_(digest, 32, digest);
    memcpy(result + 33, digest, 4);

    base = encode_base58(Binary(result, 37));
    wif_private_key = base;

    // Generate public key.
    uint8_t pub[64];
    uint8_t cpub[33];
    mECC_compute_public_key(&priKey[0], pub);
    for (auto c : pub) {
        public_key.push_back(c);
    }

    // Encode public key.
    mECC_compress(pub, cpub);
    Binary hash = ripemd160(Binary(cpub, 33));
    memcpy(result, cpub, 33);
    memcpy(result + 33, hash.data(), 4);
    base = encode_base58(Binary(result, 37));

    eos_public_key = "EOS";
    eos_public_key += base;
}

Binary eos_key::get_public_key_by_pri(const Binary &pri) {
    Binary result;
    uint8_t pub[64];
    unsigned char *priKey = new unsigned char[pri.size() + 1];
    int i;
    for (i = 0; i < (int) pri.size(); ++i) {
        priKey[i] = pri.at(i);
    }
    priKey[i] = '\0';

    mECC_compute_public_key(priKey, pub);
    for (auto c: pub) {
        result.push_back(c);
    }

    delete[] priKey;

    return result;
}

string eos_key::get_eos_public_key_by_wif(const string &wif) {
    string eos_pub;

	Binary result = decode_base58(wif);
    if (result.size()>0 && result[0] != 0x80)
	{
        printf("failed\n");
        return eos_pub;
    }

	Binary pri = result.mid(1, 32);
	Binary pub(64);
    mECC_compute_public_key(pri.data(), pub.data());

	Binary digest(32);
	SHA256_(result.data(), 33, digest.data());
	SHA256_(digest.data(), 32, digest.data());

	if (digest.left(4) != result.right(4))
	{
        printf("failed\n");
        return eos_pub;
    }

	Binary cpub(33);
    mECC_compress(pub.data(), cpub.data());
	Binary hash = ripemd160(cpub);

	result = cpub + hash.left(4);
    eos_pub = "EOS";
    eos_pub += encode_base58(result);

    return eos_pub;
}

Binary eos_key::get_private_key_by_wif(const string &wif) {
    Binary result;
    if (wif.empty()) {
        return result;
    }

    Binary bin(37);
    bin = decode_base58(wif);
    if (bin[0] != 0x80) {
        // wrong wif.
        return result;
    }

    unsigned char hexChar[33] = {0};
    unsigned char digest[32] = {0};
    unsigned char res[32] = {0};
    unsigned char last4Bytes[4] = {0};
    memcpy(hexChar, bin.data() + 1, 32);
    memcpy(last4Bytes, bin.data() + 33, 4);
    SHA256_(hexChar, 33, digest);
    SHA256_(digest, 32, digest);
    memcpy(res, digest, 4);

    if (!strncmp(reinterpret_cast<const char *>(res), reinterpret_cast<const char *>(last4Bytes),
                 4)) {
        // wif hash validate failed.
        return result;
    }

    for (auto c : hexChar) {
        result.push_back(c);
    }

    return result;
}

Binary eos_key::get_public_key_char(const string &eoskey) {
    string epk = eoskey.substr(3, -1);
    Binary lpk(37);
    lpk = decode_base58(epk);

    return lpk;
}

Binary eos_key::get_public_key_by_eos_pub(const string &pub) {
    Binary base = eos_key::get_public_key_char(pub);
    unsigned char pk[64];
    mECC_decompress(base.data(), pk);

    Binary result;
    for (int i = 0; i < 64; ++i) {
        result.push_back(pk[i]);
    }

    return result;
}
