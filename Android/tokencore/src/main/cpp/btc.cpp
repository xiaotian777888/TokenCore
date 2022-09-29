//================================================
// 比特币 BTC
//================================================
#include <jni.h>
#include <json/json.h>
#include <iostream>
#include "BtcApi.h"

using namespace BTCAPI;
using namespace std;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_btxon_tokencore_TxBtc__1createTx(JNIEnv *env, jclass type, jstring param_) {
    jlong result = 0;
    const char *json_param = env->GetStringUTFChars(param_, 0);

    if (param_ == nullptr)
        return -1;

    Json::Reader jReader;
    Json::Value jRoot;

    if (!jReader.parse(json_param, jRoot))
        return -1;
    if (!jRoot.isObject())
        return -1;

    UserTransaction *ut = new UserTransaction();
    Json::Value j_coin_id = jRoot["coin_id"];
    Json::Value j_transaction_hash = jRoot["transaction_hash"];
    Json::Value j_from_address = jRoot["from_address"];
    Json::Value j_to_address = jRoot["to_address"];
    Json::Value j_change_address = jRoot["change_address"];
    Json::Value j_value = jRoot["value"];
    Json::Value j_fee = jRoot["fee"];
    if (j_coin_id.isNull()
        || j_transaction_hash.isNull()
        || j_from_address.isNull()
        || j_to_address.isNull()
        || j_value.isNull()
        || j_fee.isNull()) {
        return -1;
    }

    ut->from_address = j_from_address.asString();
    ut->to_address = j_to_address.asString();
    ut->change_address = j_change_address.asString();
    ut->pay = atoll(j_value.asString().c_str());
    ut->fee_count = 1;//TODO ?
    ut->fee_price = atoll(j_fee.asString().c_str());
    ut->from_wallet_index = 0;
    ut->change_wallet_index = 0;

    auto j_utxo_list = jRoot["utxo_list"];
    size_t utxo_list_size = j_utxo_list.size();
    for (int i = 0; i < utxo_list_size; ++i) {
        auto j_utxo_element = j_utxo_list[i];
        Utxo utxo;
        utxo.hash = j_utxo_element["txid"].asString();
        utxo.index = j_utxo_element["vout"].asUInt();
        utxo.value = j_utxo_element["satoshis"].asUInt64();
        utxo.script = decode_script(j_utxo_element["scriptPubKey"].asString());
        ut->utxo_list.push_back(utxo);
    }

    result = (jlong) ut;
    env->ReleaseStringUTFChars(param_, json_param);
    return result;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxBtc__1firmware_1prepare_1data(JNIEnv *env, jclass type, jlong handler,
                                                         jint index) {
    UserTransaction *ut = (UserTransaction *) handler;
    if (ut == nullptr) {
        return 0;
    }
    make_unsign_tx(ut);
    std::string result = Binary::encode(firmware_prepare_data(false, ut, (int) index));
    if (!result.empty()) {
        return env->NewStringUTF(result.c_str());
    }
    return 0;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxBtc__1firmware_1prepare_1data_1usdt(JNIEnv *env, jclass type,
                                                               jlong handler,
                                                               jint index) {
    auto *ut = (UserTransaction *) handler;
    if (ut == nullptr) {
        return 0;
    }
    USDTAPI::make_unsign_tx(ut);
    std::string result = Binary::encode(USDTAPI::firmware_prepare_data(false, ut, (int) index));
    if (!result.empty()) {
        return env->NewStringUTF(result.c_str());
    }
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_btxon_tokencore_TxBtc__1firmware_1process_1result(JNIEnv *env, jclass type,
                                                           jlong handler,
                                                           jint index, jstring j_sign_result) {
    UserTransaction *ut = (UserTransaction *) handler;
    if (ut == nullptr)return;
    const char *cchar_sign_result = env->GetStringUTFChars(j_sign_result, 0);
    char *char_sign_result = const_cast<char *>(cchar_sign_result);
    std::string str_sign_result = char_sign_result;
    Binary a = Binary::decode(str_sign_result);
    firmware_process_result(ut, (int) index, (char *) &a[0], a.size());
    env->ReleaseStringUTFChars(j_sign_result, cchar_sign_result);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxBtc__1make_1signed_1tx(JNIEnv *env, jclass type, jlong handler) {
    UserTransaction *ut = (UserTransaction *) handler;
    if (ut == nullptr)return 0;
    make_sign_tx(false, ut);
    return env->NewStringUTF(ut->tx_str.c_str());
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_btxon_tokencore_TxBtc__1sign(JNIEnv *env, jclass type, jlong handler,
                                      jstring j_str_private_key) {
    UserTransaction *ut = (UserTransaction *) handler;
    if (ut == nullptr)return -1;
    const char *pconst_char_private_key = env->GetStringUTFChars(j_str_private_key, 0);
    int ret = BTCAPI::make_unsign_tx(ut);
    if (ret != 0)
        return -2;
    sign_tx(false, ut, pconst_char_private_key);
    make_sign_tx(false, ut);
    env->ReleaseStringUTFChars(j_str_private_key, pconst_char_private_key);
    return 0;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxBtc__1get_1sign_1result(JNIEnv *env, jclass type, jlong handler) {
    UserTransaction *ut = (UserTransaction *) handler;
    if (ut == nullptr)return nullptr;
    jstring jret = env->NewStringUTF(ut->tx_str.c_str());
    return jret;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_btxon_tokencore_TxBtc__1sign_1usdt(JNIEnv *env, jclass type, jlong handler,
                                      jstring j_str_private_key) {
    UserTransaction *ut = (UserTransaction *) handler;
    if (ut == nullptr)return -1;
    const char *pconst_char_private_key = env->GetStringUTFChars(j_str_private_key, 0);
    int ret = USDTAPI::make_unsign_tx(ut);
    if (ret != 0)
        return -2;
    sign_tx(false, ut, pconst_char_private_key);
    make_sign_tx(false, ut);
    env->ReleaseStringUTFChars(j_str_private_key, pconst_char_private_key);
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_btxon_tokencore_TxBtc__1destroyTx(JNIEnv *env, jclass type, jlong handler) {
    UserTransaction *ut = (UserTransaction *) handler;
    delete ut;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxBtc__1get_1private_1key(JNIEnv *env, jclass type, jstring j_seed) {
    const char *seed = env->GetStringUTFChars(j_seed, 0);
    string private_key = get_private_key(false, seed);
    env->ReleaseStringUTFChars(j_seed, seed);
    return env->NewStringUTF(private_key.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxBtc__1get_1public_1key(JNIEnv *env, jclass type, jboolean j_compress,
                                                  jstring j_private_key) {
    const char *private_key = env->GetStringUTFChars(j_private_key, 0);
    string public_key = get_public_key(private_key, j_compress);
    env->ReleaseStringUTFChars(j_private_key, private_key);
    return env->NewStringUTF(public_key.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxBtc__1get_1address(JNIEnv *env, jclass type, jboolean j_is_test_net,
                                              jstring j_public_key) {
    const char *public_key = env->GetStringUTFChars(j_public_key, 0);
    string address = get_address(j_is_test_net, public_key);
    env->ReleaseStringUTFChars(j_public_key, public_key);
    return env->NewStringUTF(address.c_str());
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_btxon_tokencore_TxBtc__1get_1tx_1len(JNIEnv *env, jclass type, jlong handler) {
    UserTransaction *ut = (UserTransaction *) handler;
    if (ut == nullptr)return -1;
    int ret = BTCAPI::get_tx_len(ut);
    return ret;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_btxon_tokencore_TxBtc__1get_1usdt_1len(JNIEnv *env, jclass type, jlong handler) {
    UserTransaction *ut = (UserTransaction *) handler;
    if (ut == nullptr)return -1;
    int ret = USDTAPI::get_tx_len(ut);
    return ret;
}

