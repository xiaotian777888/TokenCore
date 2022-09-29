#include "ed25519.h"
#include "ed25519_sha512.h"
#include "ge.h"


void ed25519_create_keypair(unsigned char *public_key, unsigned char *private_key, const unsigned char *seed) {
    ge_p3 A;

    sha512(seed, 32, private_key);
    private_key[0] &= 248;
    private_key[31] &= 63;
    private_key[31] |= 64;

    ge_scalarmult_base(&A, private_key);
    ge_p3_tobytes(public_key, &A);
}

void ed25519_public_key(unsigned char *public_key, const unsigned char *private_key) {
	ge_p3 A;

	unsigned char _private_key[64] = { 0 };
	sha512(private_key, 32, _private_key);
	_private_key[0] &= 248;
	_private_key[31] &= 63;
	_private_key[31] |= 64;

	ge_scalarmult_base(&A, _private_key);
	ge_p3_tobytes(public_key, &A);
}

void ed25519_get_sign_private(unsigned char *sign_private_key, const unsigned char *raw_private_key) {
	// unsigned char _private_key[64] = { 0 };
	sha512(raw_private_key, 32, sign_private_key);
	sign_private_key[0] &= 248;
	sign_private_key[31] &= 63;
	sign_private_key[31] |= 64;

	// memcpy(sign_private_key, _private_key, sizeof(_private_key));
}

