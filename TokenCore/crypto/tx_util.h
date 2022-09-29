#ifndef __TX_UTIL_H__
#define __TX_UTIL_H__

#include "TokenCommon.h"

#define _in
#define _out
#define _inout

namespace tx {

    void* alloc(_in size_t size);
    void free(_in void* p);

    // ����˽Կ�����Ӧ��Կ����compressָ����Կ�Ƿ����ѹ��
    // prikey: ˽Կ��ʮ�������ַ�����32�ֽڣ���64�ַ�
    int prikey_to_pubkey(
            _in const std::string& prikey,
            _in bool compress,
            _out std::string& pubkey
    );

}

#endif // __TX_UTIL_H__
