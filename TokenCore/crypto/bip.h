#ifndef __BIP_H__
#define __BIP_H__

#include "../TokenCommon.h"
#include <stdint.h>
#include <uEcc/uECC.h>
//#include <mbedtls/ecp.h>

#define CURVE_ID_SECP256K1   0x01
#define CURVE_ID_SECP256R1   0x02

typedef struct HDNode{
    uint32_t depth;
    uint32_t child_num;
    uint8_t chain_code[32];
    uint8_t private_key[32];
    uint8_t public_key[33];
    //const curve_info *curve;
    //CURVE_CTX_T curve;
    uint8_t curve_id;
    uECC_Curve curve;
} HDNode;

bool hdnode_from_seed(const Binary& bin_seed, uint8_t curve_id, HDNode *out);

bool hdnode_private_ckd(HDNode *inout, uint32_t i);


#endif // __BIP_H__
