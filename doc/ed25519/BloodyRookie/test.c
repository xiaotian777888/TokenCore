#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

/* #define ED25519_DLL */
#include "src/ed25519.h"

#include "src/ge.h"
#include "src/sc.h"

#include "src/sha3.h"

/* data for shared secret test 1 */
const static uint8_t privateKey1[32] = {
	0xc0, 0x01, 0x12, 0xd3, 0x4d, 0x19, 0x26, 0xb3,
	0x72, 0x41, 0xc6, 0xb2, 0x17, 0x7f, 0x1b, 0x53,
	0xd7, 0x75, 0x33, 0xa1, 0x5c, 0xc2, 0xf9, 0x55,
	0x2d, 0xa4, 0xdb, 0x5e, 0xc7, 0x72, 0x1f, 0x43
};

const static uint8_t publicKey1[32] = {
	0x1e, 0xde, 0xec, 0xfb, 0x30, 0x17, 0xca, 0x78,
	0xaf, 0xa5, 0x13, 0x4d, 0xcb, 0x9f, 0x34, 0xf0,
	0x10, 0x42, 0xea, 0xf6, 0xe9, 0xe7, 0x23, 0xd5,
	0xd0, 0x8a, 0x78, 0xc8, 0xaf, 0xac, 0x6a, 0x95
};

const static uint8_t salt1[32] = {
	0xb0, 0xca, 0x42, 0x06, 0x0e, 0x14, 0x0a, 0xfd,
	0xf8, 0x90, 0x6b, 0x63, 0xd0, 0x5d, 0xed, 0x47,
	0x13, 0xed, 0x41, 0xe6, 0x48, 0x83, 0xd1, 0xf2,
	0x19, 0xe8, 0xe9, 0x9e, 0x2b, 0xd7, 0x1d, 0xba
};

const static uint8_t expected_shared_secret1[32] = {
	0x5d, 0xd2, 0x59, 0xdb, 0xc9, 0x6b, 0x14, 0x18,
	0x46, 0xce, 0xd2, 0x52, 0x3e, 0xd2, 0x07, 0x07,
	0x48, 0x4f, 0x2c, 0x6a, 0xa5, 0xfe, 0x06, 0x2a,
	0xcc, 0x76, 0x9d, 0xc1, 0xb7, 0xfa, 0x02, 0x30
};

/* data for shared secret test 2 */
const static uint8_t privateKey2[32] = {
	0x68, 0x85, 0x5b, 0xb7, 0x1d, 0x3f, 0x7a, 0xd7,
	0xd6, 0x0d, 0xea, 0xf4, 0x5c, 0x63, 0x00, 0xa0,
	0xad, 0xee, 0xee, 0x3a, 0x3d, 0xe6, 0x60, 0x07,
	0x14, 0x12, 0xe4, 0xba, 0x9d, 0x62, 0x99, 0x44
};

const static uint8_t publicKey2[32] = {
	0x01, 0x5b, 0x27, 0x3c, 0xae, 0x61, 0x55, 0x47,
	0x8e, 0x5e, 0x56, 0xff, 0x64, 0xbd, 0xa2, 0xa3,
	0x02, 0x50, 0xf0, 0xf3, 0x75, 0x2e, 0x06, 0x3f,
	0x8d, 0xa9, 0x15, 0x70, 0x77, 0xb3, 0x2d, 0x6a
};

const static uint8_t salt2[32] = {
	0x54, 0xe5, 0x26, 0x40, 0xdc, 0xeb, 0x23, 0x85,
	0x7a, 0x4f, 0xdb, 0xa4, 0x5d, 0x81, 0x48, 0x47,
	0xe7, 0xcb, 0x49, 0x61, 0x8e, 0xb6, 0x96, 0x16,
	0xb9, 0xd4, 0x6e, 0x68, 0xf6, 0x1d, 0xad, 0x4c
};

const static uint8_t expected_shared_secret2[32] = {
	0x27, 0xae, 0x9c, 0xf4, 0xb1, 0xc0, 0xf9, 0x24,
	0x68, 0xa9, 0xe4, 0x29, 0xaa, 0xf8, 0xd3, 0x24,
	0x14, 0xd3, 0x78, 0x3b, 0x3a, 0x5b, 0x4d, 0x17,
	0x44, 0x42, 0x1e, 0xf2, 0xaa, 0x17, 0x82, 0xed
};

void check_for_difference(uint8_t *shared_secret, const uint8_t *expected_shared_secret) {
	uint8_t i;
	for (i = 0; i < 32; i++) {
		if (shared_secret[i] != expected_shared_secret[i]) {
			printf("difference in shared secrets found at position %d: expected %d, found %d.\n", i, expected_shared_secret[i], shared_secret[i]);
			return;
		}
	}

	printf("shared secret is as expected.\n");
}

void ed25519_key_exchange_nem_test() {
	uint8_t shared_secret[32];

	ed25519_key_exchange_nem(shared_secret, publicKey1, privateKey1, salt1);
	check_for_difference(shared_secret, expected_shared_secret1);
	ed25519_key_exchange_nem(shared_secret, publicKey2, privateKey2, salt2);
	check_for_difference(shared_secret, expected_shared_secret2);
}

/* *************************** Self Tests ************************ */

/*
* There are two set of mutually exclusive tests, based on SHA3_USE_KECCAK,
* which is undefined in the production version.
*
* Known answer tests are from NIST SHA3 test vectors at
* http://csrc.nist.gov/groups/ST/toolkit/examples.html
*
* SHA3-256:
*   http://csrc.nist.gov/groups/ST/toolkit/documents/Examples/SHA3-256_Msg0.pdf
*   http://csrc.nist.gov/groups/ST/toolkit/documents/Examples/SHA3-256_1600.pdf
* SHA3-384:
*   http://csrc.nist.gov/groups/ST/toolkit/documents/Examples/SHA3-384_1600.pdf
* SHA3-512:
*   http://csrc.nist.gov/groups/ST/toolkit/documents/Examples/SHA3-512_1600.pdf
*
* These are refered to as [FIPS 202] tests.
*
* -----
*
* A few Keccak algorithm tests (when M and not M||01 is hashed) are
* added here. These are from http://keccak.noekeon.org/KeccakKAT-3.zip,
* ShortMsgKAT_256.txt for sizes even to 8. There is also one test for
* ExtremelyLongMsgKAT_256.txt.
*
* These will work with this code when SHA3_USE_KECCAK converts Finalize
* to use "pure" Keccak algorithm.
*
*
* These are referred to as [Keccak] test.
*
* -----
*
* In one case the input from [Keccak] test was used to test SHA3
* implementation. In this case the calculated hash was compared with
* the output of the sha3sum on Fedora Core 20 (which is Perl's based).
*
*/
#define SHA3_USE_KECCAK

int sha3_test()
{
	uint8_t buf[200];
	sha3_context c;
	unsigned char hash[64];
	unsigned i;
	const uint8_t c1 = 0xa3;

#ifndef SHA3_USE_KECCAK
	/* [FIPS 202] KAT follow */
	const static uint8_t sha3_256_empty[256 / 8] = {
		0xa7, 0xff, 0xc6, 0xf8, 0xbf, 0x1e, 0xd7, 0x66,
		0x51, 0xc1, 0x47, 0x56, 0xa0, 0x61, 0xd6, 0x62,
		0xf5, 0x80, 0xff, 0x4d, 0xe4, 0x3b, 0x49, 0xfa,
		0x82, 0xd8, 0x0a, 0x4b, 0x80, 0xf8, 0x43, 0x4a
	};
	const static uint8_t sha3_256_0xa3_200_times[256 / 8] = {
		0x79, 0xf3, 0x8a, 0xde, 0xc5, 0xc2, 0x03, 0x07,
		0xa9, 0x8e, 0xf7, 0x6e, 0x83, 0x24, 0xaf, 0xbf,
		0xd4, 0x6c, 0xfd, 0x81, 0xb2, 0x2e, 0x39, 0x73,
		0xc6, 0x5f, 0xa1, 0xbd, 0x9d, 0xe3, 0x17, 0x87
	};
	const static uint8_t sha3_384_0xa3_200_times[384 / 8] = {
		0x18, 0x81, 0xde, 0x2c, 0xa7, 0xe4, 0x1e, 0xf9,
		0x5d, 0xc4, 0x73, 0x2b, 0x8f, 0x5f, 0x00, 0x2b,
		0x18, 0x9c, 0xc1, 0xe4, 0x2b, 0x74, 0x16, 0x8e,
		0xd1, 0x73, 0x26, 0x49, 0xce, 0x1d, 0xbc, 0xdd,
		0x76, 0x19, 0x7a, 0x31, 0xfd, 0x55, 0xee, 0x98,
		0x9f, 0x2d, 0x70, 0x50, 0xdd, 0x47, 0x3e, 0x8f
	};
	const static uint8_t sha3_512_0xa3_200_times[512 / 8] = {
		0xe7, 0x6d, 0xfa, 0xd2, 0x20, 0x84, 0xa8, 0xb1,
		0x46, 0x7f, 0xcf, 0x2f, 0xfa, 0x58, 0x36, 0x1b,
		0xec, 0x76, 0x28, 0xed, 0xf5, 0xf3, 0xfd, 0xc0,
		0xe4, 0x80, 0x5d, 0xc4, 0x8c, 0xae, 0xec, 0xa8,
		0x1b, 0x7c, 0x13, 0xc3, 0x0a, 0xdf, 0x52, 0xa3,
		0x65, 0x95, 0x84, 0x73, 0x9a, 0x2d, 0xf4, 0x6b,
		0xe5, 0x89, 0xc5, 0x1c, 0xa1, 0xa4, 0xa8, 0x41,
		0x6d, 0xf6, 0x54, 0x5a, 0x1c, 0xe8, 0xba, 0x00
	};
#endif

	memset(buf, c1, sizeof(buf));

#ifdef SHA3_USE_KECCAK          /* run tests against "pure" Keccak
		* algorithm; from [Keccak] */

	sha3_init256(&c);
	sha3_update(&c, "\xcc", 1);
	sha3_finalize(&c, &hash);
	if (memcmp(hash, "\xee\xad\x6d\xbf\xc7\x34\x0a\x56"
		"\xca\xed\xc0\x44\x69\x6a\x16\x88"
		"\x70\x54\x9a\x6a\x7f\x6f\x56\x96"
		"\x1e\x84\xa5\x4b\xd9\x97\x0b\x8a", 256 / 8) != 0) {
		printf("SHA3-256(cc) "
			"doesn't match known answer (single buffer)\n");
		return 11;
	}

	sha3_init256(&c);
	sha3_update(&c, "\x41\xfb", 2);
	sha3_finalize(&c, &hash);
	if (memcmp(hash, "\xa8\xea\xce\xda\x4d\x47\xb3\x28"
		"\x1a\x79\x5a\xd9\xe1\xea\x21\x22"
		"\xb4\x07\xba\xf9\xaa\xbc\xb9\xe1"
		"\x8b\x57\x17\xb7\x87\x35\x37\xd2", 256 / 8) != 0) {
		printf("SHA3-256(41fb) "
			"doesn't match known answer (single buffer)\n");
		return 12;
	}

	sha3_init256(&c);
	sha3_update(&c,
		"\x52\xa6\x08\xab\x21\xcc\xdd\x8a"
		"\x44\x57\xa5\x7e\xde\x78\x21\x76", 128 / 8);
	sha3_finalize(&c, &hash);
	if (memcmp(hash, "\x0e\x32\xde\xfa\x20\x71\xf0\xb5"
		"\xac\x0e\x6a\x10\x8b\x84\x2e\xd0"
		"\xf1\xd3\x24\x97\x12\xf5\x8e\xe0"
		"\xdd\xf9\x56\xfe\x33\x2a\x5f\x95", 256 / 8) != 0) {
		printf("SHA3-256(52a6...76) "
			"doesn't match known answer (single buffer)\n");
		return 13;
	}

	sha3_init256(&c);
	sha3_update(&c,
		"\x43\x3c\x53\x03\x13\x16\x24\xc0"
		"\x02\x1d\x86\x8a\x30\x82\x54\x75"
		"\xe8\xd0\xbd\x30\x52\xa0\x22\x18"
		"\x03\x98\xf4\xca\x44\x23\xb9\x82"
		"\x14\xb6\xbe\xaa\xc2\x1c\x88\x07"
		"\xa2\xc3\x3f\x8c\x93\xbd\x42\xb0"
		"\x92\xcc\x1b\x06\xce\xdf\x32\x24"
		"\xd5\xed\x1e\xc2\x97\x84\x44\x4f"
		"\x22\xe0\x8a\x55\xaa\x58\x54\x2b"
		"\x52\x4b\x02\xcd\x3d\x5d\x5f\x69"
		"\x07\xaf\xe7\x1c\x5d\x74\x62\x22"
		"\x4a\x3f\x9d\x9e\x53\xe7\xe0\x84" "\x6d\xcb\xb4\xce", 800 / 8);
	sha3_finalize(&c, &hash);
	if (memcmp(hash, "\xce\x87\xa5\x17\x3b\xff\xd9\x23"
		"\x99\x22\x16\x58\xf8\x01\xd4\x5c"
		"\x29\x4d\x90\x06\xee\x9f\x3f\x9d"
		"\x41\x9c\x8d\x42\x77\x48\xdc\x41", 256 / 8) != 0) {
		printf("SHA3-256(433C...CE) "
			"doesn't match known answer (single buffer)\n");
		return 14;
	}

	/* SHA3-256 byte-by-byte: 16777216 steps. ExtremelyLongMsgKAT_256
	* [Keccak] */
	/* Bloody Rookie: this is taking too long :) */
	/*i = 16777216;
	sha3_init256(&c);
	while (i--) {
		sha3_update(&c,
			"abcdefghbcdefghicdefghijdefghijk"
			"efghijklfghijklmghijklmnhijklmno", 64);
	}
	hash = sha3_finalize(&c);
	if (memcmp(hash, "\x5f\x31\x3c\x39\x96\x3d\xcf\x79"
		"\x2b\x54\x70\xd4\xad\xe9\xf3\xa3"
		"\x56\xa3\xe4\x02\x17\x48\x69\x0a"
		"\x95\x83\x72\xe2\xb0\x6f\x82\xa4", 256 / 8) != 0) {
		printf("SHA3-256( abcdefgh...[16777216 times] ) "
			"doesn't match known answer\n");
		return 15;
	}*/
#else                           /* SHA3 testing begins */

	/* SHA-256 on an empty buffer */
	sha3_init256(&c);
	hash = sha3_finalize(&c);
	if (memcmp(sha3_256_empty, hash, sizeof(sha3_256_empty)) != 0) {
		printf("SHA3-256() doesn't match known answer\n");
		return 1;
	}

	/* SHA3-256 as a single buffer. [FIPS 202] */
	sha3_init256(&c);
	sha3_update(&c, buf, sizeof(buf));
	hash = sha3_finalize(&c);
	if (memcmp(sha3_256_0xa3_200_times, hash,
		sizeof(sha3_256_0xa3_200_times)) != 0) {
		printf("SHA3-256( 0xa3 ... [200 times] ) "
			"doesn't match known answer (1 buffer)\n");
		return 1;
	}

	/* SHA3-256 in two steps. [FIPS 202] */
	sha3_init256(&c);
	sha3_update(&c, buf, sizeof(buf) / 2);
	sha3_update(&c, buf + sizeof(buf) / 2, sizeof(buf) / 2);
	hash = sha3_finalize(&c);
	if (memcmp(sha3_256_0xa3_200_times, hash,
		sizeof(sha3_256_0xa3_200_times)) != 0) {
		printf("SHA3-256( 0xa3 ... [200 times] ) "
			"doesn't match known answer (2 steps)\n");
		return 2;
	}

	/* SHA3-256 byte-by-byte: 200 steps. [FIPS 202] */
	i = 200;
	sha3_init256(&c);
	while (i--) {
		sha3_update(&c, &c1, 1);
	}
	hash = sha3_finalize(&c);
	if (memcmp(sha3_256_0xa3_200_times, hash,
		sizeof(sha3_256_0xa3_200_times)) != 0) {
		printf("SHA3-256( 0xa3 ... [200 times] ) "
			"doesn't match known answer (200 steps)\n");
		return 3;
	}

	/* SHA3-256 byte-by-byte: 135 bytes. Input from [Keccak]. Output
	* matched with sha3sum. */
	sha3_init256(&c);
	sha3_update(&c,
		"\xb7\x71\xd5\xce\xf5\xd1\xa4\x1a"
		"\x93\xd1\x56\x43\xd7\x18\x1d\x2a"
		"\x2e\xf0\xa8\xe8\x4d\x91\x81\x2f"
		"\x20\xed\x21\xf1\x47\xbe\xf7\x32"
		"\xbf\x3a\x60\xef\x40\x67\xc3\x73"
		"\x4b\x85\xbc\x8c\xd4\x71\x78\x0f"
		"\x10\xdc\x9e\x82\x91\xb5\x83\x39"
		"\xa6\x77\xb9\x60\x21\x8f\x71\xe7"
		"\x93\xf2\x79\x7a\xea\x34\x94\x06"
		"\x51\x28\x29\x06\x5d\x37\xbb\x55"
		"\xea\x79\x6f\xa4\xf5\x6f\xd8\x89"
		"\x6b\x49\xb2\xcd\x19\xb4\x32\x15"
		"\xad\x96\x7c\x71\x2b\x24\xe5\x03"
		"\x2d\x06\x52\x32\xe0\x2c\x12\x74"
		"\x09\xd2\xed\x41\x46\xb9\xd7\x5d"
		"\x76\x3d\x52\xdb\x98\xd9\x49\xd3"
		"\xb0\xfe\xd6\xa8\x05\x2f\xbb", 1080 / 8);
	hash = sha3_finalize(&c);
	if (memcmp(hash, "\xa1\x9e\xee\x92\xbb\x20\x97\xb6"
		"\x4e\x82\x3d\x59\x77\x98\xaa\x18"
		"\xbe\x9b\x7c\x73\x6b\x80\x59\xab"
		"\xfd\x67\x79\xac\x35\xac\x81\xb5", 256 / 8) != 0) {
		printf("SHA3-256( b771 ... ) doesn't match the known answer\n");
		return 4;
	}

	/* SHA3-384 as a single buffer. [FIPS 202] */
	sha3_init384(&c);
	sha3_update(&c, buf, sizeof(buf));
	hash = sha3_finalize(&c);
	if (memcmp(sha3_384_0xa3_200_times, hash,
		sizeof(sha3_384_0xa3_200_times)) != 0) {
		printf("SHA3-384( 0xa3 ... [200 times] ) "
			"doesn't match known answer (1 buffer)\n");
		return 5;
	}

	/* SHA3-384 in two steps. [FIPS 202] */
	sha3_init384(&c);
	sha3_update(&c, buf, sizeof(buf) / 2);
	sha3_update(&c, buf + sizeof(buf) / 2, sizeof(buf) / 2);
	hash = sha3_finalize(&c);
	if (memcmp(sha3_384_0xa3_200_times, hash,
		sizeof(sha3_384_0xa3_200_times)) != 0) {
		printf("SHA3-384( 0xa3 ... [200 times] ) "
			"doesn't match known answer (2 steps)\n");
		return 6;
	}

	/* SHA3-384 byte-by-byte: 200 steps. [FIPS 202] */
	i = 200;
	sha3_init384(&c);
	while (i--) {
		sha3_update(&c, &c1, 1);
	}
	hash = sha3_finalize(&c);
	if (memcmp(sha3_384_0xa3_200_times, hash,
		sizeof(sha3_384_0xa3_200_times)) != 0) {
		printf("SHA3-384( 0xa3 ... [200 times] ) "
			"doesn't match known answer (200 steps)\n");
		return 7;
	}

	/* SHA3-512 as a single buffer. [FIPS 202] */
	sha3_init512(&c);
	sha3_update(&c, buf, sizeof(buf));
	hash = sha3_finalize(&c);
	if (memcmp(sha3_512_0xa3_200_times, hash,
		sizeof(sha3_512_0xa3_200_times)) != 0) {
		printf("SHA3-512( 0xa3 ... [200 times] ) "
			"doesn't match known answer (1 buffer)\n");
		return 8;
	}

	/* SHA3-512 in two steps. [FIPS 202] */
	sha3_init512(&c);
	sha3_update(&c, buf, sizeof(buf) / 2);
	sha3_update(&c, buf + sizeof(buf) / 2, sizeof(buf) / 2);
	hash = sha3_finalize(&c);
	if (memcmp(sha3_512_0xa3_200_times, hash,
		sizeof(sha3_512_0xa3_200_times)) != 0) {
		printf("SHA3-512( 0xa3 ... [200 times] ) "
			"doesn't match known answer (2 steps)\n");
		return 9;
	}

	/* SHA3-512 byte-by-byte: 200 steps. [FIPS 202] */
	i = 200;
	sha3_init512(&c);
	while (i--) {
		sha3_update(&c, &c1, 1);
	}
	hash = sha3_finalize(&c);
	if (memcmp(sha3_512_0xa3_200_times, hash,
		sizeof(sha3_512_0xa3_200_times)) != 0) {
		printf("SHA3-512( 0xa3 ... [200 times] ) "
			"doesn't match known answer (200 steps)\n");
		return 10;
	}
#endif

	printf("SHA3-256, SHA3-384, SHA3-512 tests passed OK\n");

	return 0;
}

int main() {
	/* shared key test */
	ed25519_key_exchange_nem_test();

	/* sha3 test */
	sha3_test();

	/* other tests */
	unsigned char public_key[32], private_key[64], seed[32], scalar[32];
    unsigned char other_public_key[32], other_private_key[64];
    unsigned char shared_secret[32], other_shared_secret[32];
    unsigned char signature[64];

    clock_t start;
    clock_t end;
    int i;

    const unsigned char message[] = "Hello, world!";
    const int message_len = strlen((char*) message);

    /* create a random seed, and a keypair out of that seed */
    ed25519_create_seed(seed);
    ed25519_create_keypair(public_key, private_key, seed);

    /* create signature on the message with the keypair */
    ed25519_sign(signature, message, message_len, public_key, private_key);

    /* verify the signature */
    if (ed25519_verify(signature, message, message_len, public_key)) {
        printf("valid signature\n");
    } else {
        printf("invalid signature\n");
    }

    /* create scalar and add it to the keypair */
    ed25519_create_seed(scalar);
    ed25519_add_scalar(public_key, private_key, scalar);

    /* create signature with the new keypair */
    ed25519_sign(signature, message, message_len, public_key, private_key);

    /* verify the signature with the new keypair */
    if (ed25519_verify(signature, message, message_len, public_key)) {
        printf("valid signature\n");
    } else {
        printf("invalid signature\n");
    }

    /* make a slight adjustment and verify again */
    signature[44] ^= 0x10;
    if (ed25519_verify(signature, message, message_len, public_key)) {
        printf("did not detect signature change\n");
    } else {
        printf("correctly detected signature change\n");
    }

    /* generate two keypairs for testing key exchange */
    ed25519_create_seed(seed);
    ed25519_create_keypair(public_key, private_key, seed);
    ed25519_create_seed(seed);
    ed25519_create_keypair(other_public_key, other_private_key, seed);

    /* create two shared secrets - from both perspectives - and check if they're equal */
    ed25519_key_exchange(shared_secret, other_public_key, private_key);
    ed25519_key_exchange(other_shared_secret, public_key, other_private_key);

    for (i = 0; i < 32; ++i) {
        if (shared_secret[i] != other_shared_secret[i]) {
            printf("key exchange was incorrect\n");
            break;
        }
    }

    if (i == 32) {
        printf("key exchange was correct\n");
    }

    /* test performance */
    printf("testing seed generation performance: ");
    start = clock();
    for (i = 0; i < 10000; ++i) {
        ed25519_create_seed(seed);
    }
    end = clock();

    printf("%fus per seed\n", ((double) ((end - start) * 1000)) / CLOCKS_PER_SEC / i * 1000);


    printf("testing key generation performance: ");
    start = clock();
    for (i = 0; i < 10000; ++i) {
        ed25519_create_keypair(public_key, private_key, seed);
    }
    end = clock();

    printf("%fus per keypair\n", ((double) ((end - start) * 1000)) / CLOCKS_PER_SEC / i * 1000);

    printf("testing sign performance: ");
    start = clock();
    for (i = 0; i < 10000; ++i) {
        ed25519_sign(signature, message, message_len, public_key, private_key);
    }
    end = clock();

    printf("%fus per signature\n", ((double) ((end - start) * 1000)) / CLOCKS_PER_SEC / i * 1000);

    printf("testing verify performance: ");
    start = clock();
    for (i = 0; i < 10000; ++i) {
        ed25519_verify(signature, message, message_len, public_key);
    }
    end = clock();

    printf("%fus per signature\n", ((double) ((end - start) * 1000)) / CLOCKS_PER_SEC / i * 1000);
    

    printf("testing keypair scalar addition performance: ");
    start = clock();
    for (i = 0; i < 10000; ++i) {
        ed25519_add_scalar(public_key, private_key, scalar);
    }
    end = clock();

    printf("%fus per keypair\n", ((double) ((end - start) * 1000)) / CLOCKS_PER_SEC / i * 1000);

    printf("testing public key scalar addition performance: ");
    start = clock();
    for (i = 0; i < 10000; ++i) {
        ed25519_add_scalar(public_key, NULL, scalar);
    }
    end = clock();

    printf("%fus per key\n", ((double) ((end - start) * 1000)) / CLOCKS_PER_SEC / i * 1000);

    printf("testing key exchange performance: ");
    start = clock();
    for (i = 0; i < 10000; ++i) {
        ed25519_key_exchange(shared_secret, other_public_key, private_key);
    }
    end = clock();

    printf("%fus per shared secret\n", ((double) ((end - start) * 1000)) / CLOCKS_PER_SEC / i * 1000);

    return 0;
}
