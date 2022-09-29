#include <jni.h>
#include "../../../../../TokenCore/TokenCommon.h"

using namespace std;


extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_Common__1gen_1seed(JNIEnv *env, jclass type, jint _size) {
    int size = _size;
    string result = gen_seed(size);
    jstring ra = env->NewStringUTF(result.c_str());
    return ra;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_Common__1entropy_1to_1mnemonic(JNIEnv *env, jclass type,
                                                        jstring _entropy) {

    const char *entropy = env->GetStringUTFChars(_entropy, 0);
    string result = entropy_to_mnemonic(entropy);
    jstring ra = env->NewStringUTF(result.c_str());
    env->ReleaseStringUTFChars(_entropy, entropy);
    return ra;
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_btxon_tokencore_Common__1mnemonic_1to_1seed(JNIEnv *env, jclass type, jstring _mnemonic) {

    const char *mnemonic = env->GetStringUTFChars(_mnemonic, 0);
    string result = mnemonic_to_seed(mnemonic, "");
    jstring ra = env->NewStringUTF(result.c_str());
    env->ReleaseStringUTFChars(_mnemonic, mnemonic);
    return ra;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_btxon_tokencore_Common__1check_1mnemonic(JNIEnv *env, jclass type, jstring _mnemonic) {
    const char *mnemonic = env->GetStringUTFChars(_mnemonic, 0);
    bool result = check_mnemonic(mnemonic);
    return result ? JNI_TRUE : JNI_FALSE;
}
