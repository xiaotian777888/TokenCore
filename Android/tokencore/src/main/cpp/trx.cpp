//================================================
// 波场币 TRON
//================================================
#include <jni.h>
#include "TronApi.h"
#include "crypto/tx_util.h"
#include "crypto/keccak256.h"

using namespace TRX_API;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_btxon_tokencore_TxTRX__1createTx(JNIEnv *env, jclass type, jstring param_) {
    const char *param = env->GetStringUTFChars(param_, 0);

    jlong result = 0;
    TRX_TX *tx = nullptr;
    if (0 == TxCreate(param, &tx)) {
        result = (jlong) (tx);
    }

    env->ReleaseStringUTFChars(param_, param);

    return  result;
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_btxon_tokencore_TxTRX__1sign(JNIEnv *env, jclass type, jlong handler, jstring param_) {

    const char *param = env->GetStringUTFChars(param_, 0);

    TRX_TX *tx = (TRX_TX *)handler;
    int result = TxSign(tx, param);

    env->ReleaseStringUTFChars(param_, param);

    return result;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxTRX__1getResult(JNIEnv *env, jclass type, jlong handler, jstring param_) {
    const char *param = env->GetStringUTFChars(param_, 0);
    TRX_TX *tx = (TRX_TX *)handler;

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
Java_com_btxon_tokencore_TxTRX__1destroyTx(JNIEnv *env, jclass type, jlong handler) {
    TRX_TX *tx = (TRX_TX *)handler;
    TxDestory(&tx);
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxTRX__1getPriKey(JNIEnv *env, jclass type, jstring param_) {
    const char *param = env->GetStringUTFChars(param_, 0);

    jstring jret = nullptr;
    char* result = nullptr;
    int ret = TxGetPriKey(param, &result);
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
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxTRX__1getPubKey(JNIEnv *env, jclass type, jstring param_) {
    const char *param = env->GetStringUTFChars(param_, 0);

    jstring jret = nullptr;
    char* result = nullptr;
    int ret = TxGetPubKey(param, &result);
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
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxTRX__1getAddress(JNIEnv *env, jclass type, jstring param_) {
    const char *param = env->GetStringUTFChars(param_, 0);

    jstring jret = nullptr;
    char* result = nullptr;
    int ret = TxGetAddress(param, &result);
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
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxTRX__1make_1unsigned_1tx(JNIEnv *env, jclass type, jlong handler) {
    jstring jret = nullptr;
    char *result = nullptr;
    TRX_TX *tx = (TRX_TX *) handler;
    jret = env->NewStringUTF(GetUnsignedTx(tx).c_str());
    tx::free(result);
    return jret;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxTRX__1make_1signed_1tx(JNIEnv *env, jclass type, jlong handler,
                                                  jstring j_sign_result) {
    const char *const_char_sign_result = env->GetStringUTFChars(j_sign_result, 0);
    jstring jret = nullptr;
    TRX_TX *tx = (TRX_TX *) handler;
    int ret = TxSetSignature(tx, const_char_sign_result);
    if (ret == 0) {
        char *result = nullptr;
        char *param = nullptr;//unused
        ret = TxGetTransacton(tx, param, &result);
        if (ret == 0) {
            jret = env->NewStringUTF(result);
            tx::free(result);
        }
    }
    env->ReleaseStringUTFChars(j_sign_result, const_char_sign_result);
    return jret;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxTRX__1sign_1tx(JNIEnv *env, jclass type, jstring j_str_unsign_tx,jstring j_str_prikey) {
    const char *const_char_unsign_tx = env->GetStringUTFChars(j_str_unsign_tx, 0);
    const char *const_char_prikey = env->GetStringUTFChars(j_str_prikey, 0);
    std::string sign_result = TRONAPI::sign_tx(const_char_unsign_tx, const_char_prikey);
    jstring jret = env->NewStringUTF(sign_result.c_str());
    env->ReleaseStringUTFChars(j_str_unsign_tx, const_char_unsign_tx);
    env->ReleaseStringUTFChars(j_str_prikey, const_char_prikey);
    return jret;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_TxTRX__1sign_1message(JNIEnv *env, jclass type, jboolean use_tron_header,
                                               jstring j_str_transaction, jstring j_str_prikey) {
    const char *const_char_transaction = env->GetStringUTFChars(j_str_transaction, 0);
    const char *const_char_prikey = env->GetStringUTFChars(j_str_prikey, 0);
    std::string sign_result = TRONAPI::sign_message(use_tron_header, const_char_transaction,
                                                    const_char_prikey);
    jstring jret = env->NewStringUTF(sign_result.c_str());
    env->ReleaseStringUTFChars(j_str_transaction, const_char_transaction);
    env->ReleaseStringUTFChars(j_str_prikey, const_char_prikey);
    return jret;
}

