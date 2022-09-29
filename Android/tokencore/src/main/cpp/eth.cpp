#include <jni.h>
#include <json/json.h>
#include "EthApi.h"
#include "TokenCommon.h"

typedef struct {
    UserTransaction *ut;
    int major;
    int minor;
    int chain_id;
    const char *contract_address;
    EthereumSignTx eth_tx;
    string tx_str;
} TxEth;

using namespace ETHAPI;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_btxon_tokencore_TxEth__1createTx(JNIEnv *env, jclass type, jstring params) {
    jlong result = 0;

    if (params == nullptr)
        return -1;

    const char *json_param = env->GetStringUTFChars(params, 0);
    Json::Reader jReader;
    Json::Value jRoot;

    if (!jReader.parse(json_param, jRoot))
        return -2;
    if (!jRoot.isObject())
        return -3;

    UserTransaction *ut = new UserTransaction();
    Json::Value j_coin_id = jRoot["coin_id"];
    Json::Value j_transaction_hash = jRoot["transaction_hash"];
    Json::Value j_from_address = jRoot["from_address"];
    Json::Value j_to_address = jRoot["to_address"];
    Json::Value j_value = jRoot["value"];
    Json::Value j_fee = jRoot["fee"];
    Json::Value j_fee_count = jRoot["fee_count"];
    Json::Value j_nonce = jRoot["nonce"];
    Json::Value j_major = jRoot["major"];
    Json::Value j_minor = jRoot["minor"];
    Json::Value j_chain_id = jRoot["chain_id"];
    Json::Value j_contract_address = jRoot["contract_address"];

    if (j_coin_id.isNull()
        || j_transaction_hash.isNull()
        || j_from_address.isNull()
        || j_to_address.isNull()
        || j_value.isNull()
        || j_fee_count.isNull()
        || j_fee.isNull()) {
        return -4;
    }

    ut->from_address = j_from_address.asString();
    ut->to_address = j_to_address.asString();
    ut->change_address = ut->from_address;
    ut->pay = atoll(j_value.asString().c_str());
    ut->nonce = atoll(j_nonce.asString().c_str());
    ut->fee_count = atoll(j_fee_count.asString().c_str());
    ut->fee_price = atoll(j_fee.asString().c_str());
    ut->contract_address = j_contract_address.asString();
    ut->from_wallet_index = 0;
    ut->change_wallet_index = 0;

    TxEth *txEth = new TxEth();
    txEth->major = j_major.asInt();
    txEth->minor = j_minor.asInt();
    txEth->chain_id = j_chain_id.asInt();
    txEth->contract_address = j_chain_id.asString().c_str();
    txEth->ut = ut;
    result = (jlong) txEth;
    env->ReleaseStringUTFChars(params, json_param);
    return result;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_btxon_tokencore_TxEth__1destroyTx(JNIEnv *env, jclass type, jlong handle) {
    TxEth *tx = (TxEth *) handle;
    if (tx != nullptr) {
        delete tx;
    }
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxEth__1firmware_1prepare_1data(JNIEnv *env, jclass type, jlong handler) {
    TxEth *tx = (TxEth *) handler;
    if (tx == nullptr) {
        return 0;
    }
    u256 fee = tx->ut->fee_count * tx->ut->fee_price;
    EthereumSignTx eth_tx;
    int ret = make_unsign_tx(eth_tx, (unsigned char) tx->minor, tx->ut->from_address.c_str(),
                             tx->ut->to_address.c_str(), (uint64_t) tx->ut->nonce,
                             tx->chain_id, tx->ut->contract_address.c_str(),
                             tx->ut->pay, (uint64_t) tx->ut->fee_count,
                             (uint64_t) tx->ut->fee_price);
    if (ret != 0)
        return 0;

    Binary custom_data;
    unsigned char firmware_data[4096];
    int firmware_size;
    firmware_prepare_data(eth_tx, firmware_data, firmware_size, custom_data);
    tx->eth_tx = eth_tx;
    Binary a;
    a.resize(firmware_size);
    memcpy(&a[0], firmware_data, firmware_size);
    return env->NewStringUTF(Binary::encode(a).c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_btxon_tokencore_TxEth__1firmware_1process_1result(JNIEnv *env, jclass type, jlong handler,
                                                           jstring j_sign_result) {
    TxEth *tx = (TxEth *) handler;
    if (tx == nullptr) {
        return;
    }
    const char *cchar_sign_result = env->GetStringUTFChars(j_sign_result, 0);
    char *char_sign_result = const_cast<char *>(cchar_sign_result);
    std::string str_sign_result = char_sign_result;
    Binary a = Binary::decode(str_sign_result);

    EthereumTxRequest txReq;
    firmware_process_result(tx->eth_tx, (char *) &a[0], a.size(), txReq);
    tx->tx_str = make_sign_tx(tx->eth_tx, txReq);
    env->ReleaseStringUTFChars(j_sign_result, cchar_sign_result);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxEth__1make_1signed_1tx(JNIEnv *env, jclass type, jlong handler) {
    TxEth *tx = (TxEth *) handler;
    if (tx == nullptr) {
        return 0;
    }
    return env->NewStringUTF(tx->tx_str.c_str());
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_btxon_tokencore_TxEth__1sign(JNIEnv *env, jclass type, jlong handler,
                                      jstring j_str_private_key) {
    TxEth *tx = (TxEth *) handler;
    if (tx == nullptr)return -1;
    const char *pconst_char_private_key = env->GetStringUTFChars(j_str_private_key, 0);
    EthereumSignTx eth_tx;
    int result = make_unsign_tx(eth_tx, (unsigned char) tx->minor, tx->ut->from_address.c_str(),
                                tx->ut->to_address.c_str(), (uint64_t) tx->ut->nonce,
                                tx->chain_id, tx->ut->contract_address.c_str(),
                                tx->ut->pay, (uint64_t) tx->ut->fee_count,
                                (uint64_t) tx->ut->fee_price);
    if (result != 0)
        return -2;
    EthereumTxRequest txReq;
    ETHAPI::sign_tx(eth_tx, pconst_char_private_key, txReq);
    tx->tx_str = ETHAPI::make_sign_tx(eth_tx, txReq);
    env->ReleaseStringUTFChars(j_str_private_key, pconst_char_private_key);
    return 0;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxEth__1get_1sign_1result(JNIEnv *env, jclass type, jlong handler) {
    TxEth *tx = (TxEth *) handler;
    if (tx == nullptr) {
        return 0;
    }
    return env->NewStringUTF(tx->tx_str.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxEth__1get_1private_1key(JNIEnv *env, jclass type, jstring j_seed) {
    const char *seed = env->GetStringUTFChars(j_seed, 0);
    string private_key = get_private_key(seed);//TODO 这个函数功能未验证是否正确
    env->ReleaseStringUTFChars(j_seed, seed);
    return env->NewStringUTF(private_key.c_str());
    return nullptr;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxEth__1get_1public_1key(JNIEnv *env, jclass type, jstring j_private_key) {
    const char *private_key = env->GetStringUTFChars(j_private_key, 0);
    string public_key = get_public_key(private_key);
    env->ReleaseStringUTFChars(j_private_key, private_key);
    return env->NewStringUTF(public_key.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxEth__1get_1address(JNIEnv *env, jclass type, jstring j_public_key) {
    const char *public_key = env->GetStringUTFChars(j_public_key, 0);
    string address = get_address(public_key);
    env->ReleaseStringUTFChars(j_public_key, public_key);
    return env->NewStringUTF(address.c_str());
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_btxon_tokencore_TxEth__1make_1game_1tx(JNIEnv *env, jclass type, jlong handler,jstring j_str_private_key,jstring dataStr) {
    TxEth *tx = (TxEth *) handler;
    if (tx == nullptr)return -1;
    const char* pconst_char_private_key = env->GetStringUTFChars(j_str_private_key, 0);
    const char* data_pay_load = env->GetStringUTFChars(dataStr, 0);

    EthereumSignTx eth_tx;
    int result = make_unsign_tx(eth_tx, (uint64_t) tx->ut->nonce,tx->chain_id,
                                tx->ut->to_address.c_str(),tx->ut->pay,
                                (uint64_t) tx->ut->fee_count, (uint64_t) tx->ut->fee_price,data_pay_load);
    if (result != 0)
        return -2;
    EthereumTxRequest txReq;
    ETHAPI::sign_tx(eth_tx, pconst_char_private_key, txReq);
    tx->tx_str = ETHAPI::make_sign_tx(eth_tx, txReq);
    env->ReleaseStringUTFChars(j_str_private_key, pconst_char_private_key);
    return 0;
}


