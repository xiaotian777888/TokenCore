#include <mbedtls/bignum.h>
#include <mbedtls/ecp.h>
#include <crypto/hmac_sha512.h>
#include <uEcc/macroECC.h>
#include "bip.h"

const char* g_bip32_name = "Bitcoin seed";

bool hdnode_from_seed(const Binary& bin_seed, uint8_t curve_id, HDNode *out)
{
    if(out == nullptr)
        return false;

    uint8_t keys[64] = { 0 };
    memset(out, 0, sizeof(HDNode));
    out->depth = 0;
    out->child_num = 0;

    if(curve_id == CURVE_ID_SECP256K1) {
        out->curve = uECC_secp256k1();
    } else if(curve_id == CURVE_ID_SECP256R1) {
        out->curve = uECC_secp256r1();
    } else {
        return false;
    }
    out->curve_id = curve_id;

//    // CURVE_ID_SECP256K1
//    if (!alg_ecc_init(curve_id, &out->curve))
//        return false;

    //hmac_sha512((const uint8_t*)g_bip32_name, strlen(g_bip32_name), seed, seed_len, keys);
    //HMACSHA512((const uint8_t*)g_bip32_name, strlen(g_bip32_name), bin_seed.data(), bin_seed.size(), keys);
    HMACSHA512(bin_seed.data(), bin_seed.size(), (const uint8_t*)g_bip32_name, strlen(g_bip32_name), keys);

    uint8_t i = 0;
    for (i = 0; i < 30; ++i) {
        //if (alg_ecc_check_pri_key(&out->curve, keys, 32))
        if(uECC_valid_private_key(keys, out->curve))
            break;
//        hmac_sha512((const uint8_t*)g_bip32_name, strlen(g_bip32_name), keys, sizeof(keys), keys);
        HMACSHA512(keys, sizeof(keys), (const uint8_t*)g_bip32_name, strlen(g_bip32_name), keys);
    }
    if (i == 30)
        return false;

    memcpy(out->private_key, keys, 32);
    memcpy(out->chain_code, keys + 32, 32);

    memset(out->public_key, 0, sizeof(out->public_key));
    memset(keys, 0, sizeof(keys));

    return true;
}

// RET = LEFT + RIGHT
static bool _ecc_bn_add(
        uint8_t curve_id,
        uint8_t* ret_buf, uint8_t *ret_len,
        const uint8_t* left_buf, uint8_t left_len,
        const uint8_t* right_buf, uint8_t right_len
) {
    if(ret_buf == nullptr || ret_len == nullptr || *ret_len == 0)
        return false;
    if(left_buf == nullptr || left_len == 0)
        return false;
    if(right_buf == nullptr || right_len == 0)
        return false;

    mbedtls_ecp_group grp;
    memset(&grp, 0, sizeof(mbedtls_ecp_group));
    if (curve_id == CURVE_ID_SECP256K1) {
        if (0 != mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256K1)) {
            return false;
        }
    } else if (curve_id == CURVE_ID_SECP256R1) {
        if (0 != mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1)) {
            return false;
        }
    }
    else {
        return false;
    }

    mbedtls_mpi bn_result, bn_left, bn_right;
    mbedtls_mpi_init(&bn_result);
    mbedtls_mpi_init(&bn_left);
    mbedtls_mpi_init(&bn_right);
    mbedtls_mpi_read_binary(&bn_left, left_buf, left_len);
    mbedtls_mpi_read_binary(&bn_right, right_buf, right_len);


    bool ret = (0 == mbedtls_mpi_add_mpi(&bn_result, &bn_left, &bn_right));
    if (ret)
        ret = (0 == mbedtls_mpi_mod_mpi(&bn_result, &bn_result, &grp.N));

    if (ret) {
        uint8_t len = (uint8_t)mbedtls_mpi_size(&bn_result);
        if(*ret_len < len)
            return false;
        *ret_len = len;

		int offset = 32 - len;
		if (offset > 0)
			memset(ret_buf, 0, 32);
		ret = (0 == mbedtls_mpi_write_binary(&bn_result, ret_buf + offset, *ret_len));
    }

    mbedtls_mpi_free(&bn_result);
    mbedtls_mpi_free(&bn_left);
    mbedtls_mpi_free(&bn_right);
    return ret;
}

bool test_bn_add(uint8_t* ret_buf, const uint8_t* left_buf, const uint8_t* right_buf)
{
	uint8_t ret_len = 32;
	uint8_t left_len = 32;
	uint8_t right_len = 32;

	return _ecc_bn_add(CURVE_ID_SECP256K1, ret_buf, &ret_len, left_buf, left_len, right_buf, right_len);

}

bool hdnode_private_ckd(HDNode *inout, uint32_t i)
{
    uint8_t data[1 + 32 + 4] = { 0 };
    uint8_t I[32 + 32] = { 0 };
    uint8_t _tmp_pubkey[64] = {0};

    if (i & 0x80000000) {
        // 派生增强子密钥
        data[0] = 0;
        memcpy(data + 1, inout->private_key, 32);
    }
    else {
        // 派生一般子密钥
        //uint8_t pubkey_len = 33;
        //alg_ecc_calc_pub_key(&inout->curve, inout->private_key, 32, true, inout->public_key, &pubkey_len);

        uECC_compute_public_key(inout->private_key, _tmp_pubkey, inout->curve);
        uECC_compress(_tmp_pubkey, inout->public_key, inout->curve);

        int ret = mECC_compute_public_key(inout->private_key, _tmp_pubkey);
        if (1 == ret) {
            mECC_compress(_tmp_pubkey, inout->public_key);
        } else {
            return false;
        }

        memcpy(data, inout->public_key, 33);
    }
    data[33] = (uint8_t)(i >> 24);
    data[34] = (uint8_t)(i >> 16);
    data[35] = (uint8_t)(i >> 8);
    data[36] = (uint8_t)i;

    //hmac_sha512(inout->chain_code, 32, data, sizeof(data), I);
    HMACSHA512(data, sizeof(data), inout->chain_code, 32, I);

    while (true) {
        int ok = 0;

        //ok = alg_ecc_check_pri_key(&inout->curve, I, 32);
        ok = uECC_valid_private_key(I, inout->curve);
        if (ok) {
            //ok = alg_ecc_bn_add(&inout->curve, I, 32, inout->private_key, 32);
            //uECC_bn_addMod(inout->curve, I, I, inout->private_key);
            uint8_t len = 32;
            if(!_ecc_bn_add(inout->curve_id, I, &len, I, 32, inout->private_key, 32))
                ok = 0;
        }

        if (ok) {
            break;
        }

        data[0] = 1;
        memcpy(data + 1, I + 32, 32);
        //hmac_sha512(inout->chain_code, 32, data, sizeof(data), I);
        HMACSHA512(data, sizeof(data), inout->chain_code, 32, I);
    }

    memcpy(inout->private_key, I, 32);
    memcpy(inout->chain_code, I + 32, 32);
    inout->depth++;
    inout->child_num = i;
    memset(inout->public_key, 0, sizeof(inout->public_key));

    memset(I, 0, sizeof(I));
    memset(data, 0, sizeof(data));
    return true;
}
