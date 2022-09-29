#include <crypto/tx_util.h>
#include <jni.h>

extern "C" JNIEXPORT jint JNICALL
Java_com_btxon_tokencore_Tx_txGetVersion(JNIEnv *env, jclass self) {
    return 1;
}
