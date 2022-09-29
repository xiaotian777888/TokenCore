#ifndef __TX_UTIL_H__
#define __TX_UTIL_H__

#include "TokenCommon.h"

#define _in
#define _out
#define _inout

namespace tx {

    void* alloc(_in size_t size);
    void free(_in void* p);

    // 根据私钥计算对应公钥，由compress指定公钥是否进行压缩
    // prikey: 私钥的十六进制字符串，32字节，即64字符
    int prikey_to_pubkey(
            _in const std::string& prikey,
            _in bool compress,
            _out std::string& pubkey
    );

}

#endif // __TX_UTIL_H__
