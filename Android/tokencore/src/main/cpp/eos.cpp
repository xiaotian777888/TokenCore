#include <jni.h>
#include "EosApi.h"
#include "crypto/tx_util.h"
using namespace EOSAPI;

//extern "C"
//JNIEXPORT jlong JNICALL
//Java_com_btxon_tokencore_TxEOS__1createTx(JNIEnv *env, jclass type, jstring code_,
//                                          jstring action_, jstring account_, jboolean isOwner,
//                                          jstring binArgs_, jstring chainInfo_) {
////    const char *code = env->GetStringUTFChars(code_, 0);
////    const char *action = env->GetStringUTFChars(action_, 0);
////    const char *account = env->GetStringUTFChars(account_, 0);
////    const char *binArgs = env->GetStringUTFChars(binArgs_, 0);
////    const char *chainInfo = env->GetStringUTFChars(chainInfo_, 0);
////
////    jlong result = 0;
////    EOS_TX *tx = nullptr;
////    if (0 == EOSAPI::TxCreate(&tx, isOwner, code, action, account, binArgs, chainInfo)) {
////        result = (jlong) (tx);
////    }
////
////    env->ReleaseStringUTFChars(code_, code);
////    env->ReleaseStringUTFChars(action_, action);
////    env->ReleaseStringUTFChars(account_, account);
////    env->ReleaseStringUTFChars(binArgs_, binArgs);
////    env->ReleaseStringUTFChars(chainInfo_, chainInfo);
////
////    return result;
//
//    return 0;
//}


extern "C"
JNIEXPORT jlong JNICALL
Java_com_btxon_tokencore_TxEOS__1createTx(JNIEnv *env, jclass type, jstring param_) {
    const char *param = env->GetStringUTFChars(param_, 0);

    jlong result = 0;
    EOS_TX *tx = nullptr;
    if (0 == TxCreate(param, &tx)) {
        result = (jlong) (tx);
    }

    env->ReleaseStringUTFChars(param_, param);

    return result;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_btxon_tokencore_TxEOS__1addAction(JNIEnv *env, jclass type, jlong handler, jstring param_) {
    const char *param = env->GetStringUTFChars(param_, 0);

    EOS_TX *tx = (EOS_TX *) handler;
    jint result = TxAddAction(tx, param);
    env->ReleaseStringUTFChars(param_, param);

    return result;
}

//extern "C"
//JNIEXPORT jlong JNICALL
//Java_com_btxon_tokencore_TxEOS__1createTxFromJson(JNIEnv *env, jclass type, jstring chain_id_,
//                                                  jstring json_raw_) {
////    const char *chain_id = env->GetStringUTFChars(chain_id_, 0);
////    const char *json_raw = env->GetStringUTFChars(json_raw_, 0);
////
////    jlong result = 0;
////    EOS_TX *tx = nullptr;
////    if (0 == EOSAPI::TxCreateFromJson(&tx, chain_id, json_raw)) {
////        result = (jlong) (tx);
////    }
////
////    env->ReleaseStringUTFChars(chain_id_, chain_id);
////    env->ReleaseStringUTFChars(json_raw_, json_raw);
////
////    return result;
//
//    return 0;
//}

//extern "C"
//JNIEXPORT jint JNICALL
//Java_com_btxon_tokencore_TxEOS__1sign(JNIEnv *env, jclass type, jlong handler,
//                                      jstring wifKey_) {
//
////    const char *wifKey = env->GetStringUTFChars(wifKey_, 0);
////
////    EOS_TX *tx = (EOS_TX *) handler;
////    int result = EOSAPI::TxSign(tx, wifKey);
////
////    env->ReleaseStringUTFChars(wifKey_, wifKey);
////
////    return result;
//
//    return 0;
//}


extern "C"
JNIEXPORT jint JNICALL
Java_com_btxon_tokencore_TxEOS__1sign(JNIEnv *env, jclass type, jlong handler, jstring param_) {

    const char *param = env->GetStringUTFChars(param_, 0);

    EOS_TX *tx = (EOS_TX *)handler;
    int result = TxSign(tx, param);

    env->ReleaseStringUTFChars(param_, param);

    return result;
}

//extern "C"
//JNIEXPORT jstring JNICALL
//Java_com_btxon_tokencore_TxEOS__1getResult(JNIEnv *env, jclass type, jlong handler) {
//
////    EOS_TX *tx = (EOS_TX *) handler;
////    char *rawTransaction = nullptr;
////
////    int size = 0;
////    int retcode = EOSAPI::TxPush(tx, &rawTransaction, &size, nullptr, 0);
////    if (retcode == 0) {
////        jstring result = env->NewStringUTF(rawTransaction);
////        EOSAPI::Free(rawTransaction);
////        rawTransaction = nullptr;
////        return result;
////    }
//
//    return env->NewStringUTF("");
//}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxEOS__1getResult(JNIEnv *env, jclass type, jlong handler, jstring param_) {
    const char *param = env->GetStringUTFChars(param_, 0);
    EOS_TX *tx = (EOS_TX *)handler;

    jstring jret = nullptr;
    char* result = nullptr;
    int ret = TxGetTransacton(tx, param, &result);
    if (ret == 0) {
        jret = env->NewStringUTF(result);
        tx::free(result);
    } else {
        jret = env->NewStringUTF("");
    }

    env->ReleaseStringUTFChars(param_, param);

    return jret;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_btxon_tokencore_TxEOS__1destroy(JNIEnv *env, jclass type, jlong handler) {
    if(handler != 0) {
        EOS_TX *tx = (EOS_TX *) handler;
        EOSAPI::release(&tx);
    }
}

//extern "C"
//JNIEXPORT jint JNICALL
//Java_com_btxon_tokencore_TxEOS__1addAction(JNIEnv *env, jclass type, jlong handler,
//                                           jstring code_, jstring action_, jstring account_,
//                                           jboolean asOwner, jstring binArgs_) {
////    const char *code = env->GetStringUTFChars(code_, 0);
////    const char *action = env->GetStringUTFChars(action_, 0);
////    const char *account = env->GetStringUTFChars(account_, 0);
////    const char *binArgs = env->GetStringUTFChars(binArgs_, 0);
////
////    EOS_TX *tx = (EOS_TX *) handler;
////    int result = EOSAPI::TxAddAction(tx, asOwner, code, action, account, binArgs);
////
////    env->ReleaseStringUTFChars(code_, code);
////    env->ReleaseStringUTFChars(action_, action);
////    env->ReleaseStringUTFChars(account_, account);
////    env->ReleaseStringUTFChars(binArgs_, binArgs);
////
////    return result;
//
//    return 0;
//}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxEOS__1getPrivateKey(JNIEnv *env, jclass type, jstring param_) {
    const char *param = env->GetStringUTFChars(param_, 0);

    jstring jret = nullptr;
    char* result = nullptr;
    int ret = EOSAPI::TxGetPriKey(param, &result);
    if (ret == 0) {
        jret = env->NewStringUTF(result);
        tx::free(result);
    } else {
        jret = nullptr;
    }

    env->ReleaseStringUTFChars(param_, param);

    return jret;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxEOS__1getPublicKey(JNIEnv *env, jclass type, jstring param_) {
    const char *param = env->GetStringUTFChars(param_, 0);

    jstring jret = nullptr;
    char* result = nullptr;
    int ret = EOSAPI::TxGetPubKey(param, &result);
    if (ret == 0) {
        jret = env->NewStringUTF(result);
        tx::free(result);
    } else {
        jret = nullptr;
    }

    env->ReleaseStringUTFChars(param_, param);

    return jret;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxEOS__1longToName(JNIEnv *env, jclass type, jlong val) {

    jstring jret = nullptr;
    char* result = nullptr;
    int ret = EOSAPI::TxLongToName(val, &result);
    if (ret == 0) {
        jret = env->NewStringUTF(result);
        tx::free(result);
    } else {
        jret = nullptr;
    }

    return jret;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxEOS__1make_1unsigned_1tx(JNIEnv *env, jclass type, jlong handler) {
    EOS_TX *tx = (EOS_TX *) handler;
    if(tx == nullptr) {
        return 0;
    }
    return env->NewStringUTF(Binary::encode(EOSAPI::get_data(tx)).c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxEOS__1make_1signed_1tx(JNIEnv *env, jclass type, jlong handler,
                                                  jstring j_sign_result) {
    EOS_TX *tx = (EOS_TX *) handler;
    if (tx == nullptr) {
        return 0;
    }
    const char *const_char_sign_result = env->GetStringUTFChars(j_sign_result, 0);
    Binary sign_data = Binary::decode(const_char_sign_result);
    auto ret = env->NewStringUTF(EOSAPI::make_sign_tx(tx, sign_data).c_str());
    env->ReleaseStringUTFChars(j_sign_result, const_char_sign_result);
    return ret;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxEOS__1sign_1message(JNIEnv *env, jclass type, jstring raw_json, jstring j_str_prikey) {
    const char *const_raw_json = env->GetStringUTFChars(raw_json, 0);
    const char *const_char_prikey = env->GetStringUTFChars(j_str_prikey, 0);
    std::string raw_json_str = const_raw_json;
    std::string pro_key_str = const_char_prikey;
    std::string sign_result = EOSAPI::sign_eos_message(raw_json_str, pro_key_str);
    env->ReleaseStringUTFChars(raw_json, const_raw_json);
    env->ReleaseStringUTFChars(j_str_prikey, const_char_prikey);
    return env->NewStringUTF(sign_result.c_str());
}
