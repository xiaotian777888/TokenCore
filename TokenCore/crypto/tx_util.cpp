#include "tx_util.h"
#include <uEcc/macroECC.h>
#include <Eos/eos_key_encode.h>

namespace tx {

    void* alloc(_in size_t size) {
        unsigned char* p = new unsigned char[size];
        if(p == nullptr)
            return nullptr;

        memset(p, 0, size);
        return (void*)p;
    }

    void free(_in void* p) {
        if (NULL != p)
            delete (unsigned char *) p;
    }

    // 根据私钥计算对应公钥，由compress指定公钥是否进行压缩
    int prikey_to_pubkey(
            _in const std::string& str_prikey,
            _in bool compress,
            _out std::string& str_pubkey
    ) {
        Binary bin_prikey = Binary::decode(str_prikey);
        if(bin_prikey.size() != 32)
            return -1;

        Binary bin_tmp(64);
        if(1 != mECC_compute_public_key(bin_prikey.data(), bin_tmp.data()))
            return -1;
		
        if(compress) {
            Binary _tmp(33);
            mECC_compress(bin_tmp.data(), _tmp.data());
            str_pubkey = Binary::encode(_tmp);
        } else {
            str_pubkey = Binary::encode(bin_tmp);
        }

        return 0;
    }

}
