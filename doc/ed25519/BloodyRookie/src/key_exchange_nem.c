#include "ed25519.h"
#include "sha3.h"
#include "ge.h"
#include "sc.h"

void negate(ge_p3 *A) {
	fe_neg(A->X, A->X);
	fe_neg(A->T, A->T);
}

void ed25519_key_exchange_nem(unsigned char *shared_secret, const unsigned char *public_key, const unsigned char *private_key, const unsigned char *salt) {
	ge_p3 A;
	ge_p2 R;
	unsigned char r[32];
	unsigned char e[32];
	unsigned char zero[32];
	unsigned int i;

	/* initialize zero and shared secret */
	for (i = 0; i < 32; ++i) {
		zero[i] = 0;
		shared_secret[i] = 0;
	}

	/* decode group element */
	if (ge_frombytes_negate_vartime(&A, public_key) != 0) {
		return;
	}

	/* need to negate A again */
	negate(&A);

	/* copy the private key and make sure it's valid */
	for (i = 0; i < 32; ++i) {
		e[i] = private_key[i];
	}

	e[0] &= 248;
	e[31] &= 63;
	e[31] |= 64;

	/* R = e * A + 0 * B */
	ge_double_scalarmult_vartime(&R, e, &A, zero);

	/* encode group element */
	ge_tobytes(r, &R);

	/* apply salt */
	for (i = 0; i < 32; ++i) {
		r[i] ^= salt[i];
	}

	/* hash the result */
	sha3_256(r, 32, shared_secret);
}