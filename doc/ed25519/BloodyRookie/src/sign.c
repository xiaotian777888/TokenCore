#include "ed25519.h"
#include "sha3.h"
#include "ge.h"
#include "sc.h"
#include "sha512.h"


void ed25519_sign(unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *public_key, const unsigned char *private_key) {
	sha3_context ctx;
	unsigned char hram[64];
    unsigned char r[64];
    ge_p3 R;


	sha3_init512(&ctx);
	sha3_update(&ctx, private_key + 32, 32);
	sha3_update(&ctx, message, message_len);
	sha3_finalize(&ctx, &r);

    sc_reduce(r);
    ge_scalarmult_base(&R, r);
    ge_p3_tobytes(signature, &R);

	sha3_init512(&ctx);
	sha3_update(&ctx, signature, 32);
	sha3_update(&ctx, public_key, 32);
	sha3_update(&ctx, message, message_len);
	sha3_finalize(&ctx, hram);

    sc_reduce(hram);
    sc_muladd(signature + 32, hram, private_key, r);
}

// add by liangjisheng 2018-12-24
void ed25519_sign1(unsigned char *signature, const unsigned char *message, size_t message_len, const unsigned char *public_key, const unsigned char *private_key) {
	SHA512CTX ctx;
	unsigned char hram[64];
	unsigned char r[64];
	ge_p3 R;

	SHA512Init(&ctx);
	SHA512Update(&ctx, private_key + 32, 32);
	SHA512Update(&ctx, message, message_len);
	SHA512Final(&ctx, &r);

	sc_reduce(r);
	ge_scalarmult_base(&R, r);
	ge_p3_tobytes(signature, &R);

	SHA512Init(&ctx);
	SHA512Update(&ctx, signature, 32);
	SHA512Update(&ctx, public_key, 32);
	SHA512Update(&ctx, message, message_len);
	SHA512Final(&ctx, hram);

	sc_reduce(hram);
	sc_muladd(signature + 32, hram, private_key, r);
}
