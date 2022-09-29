
#ifndef SHA512_H
#define SHA512_H

#include <stdint.h>
#include <stddef.h>

#define SHA512_STATE_LENGTH 8U
#define SHA512_COUNT_LENGTH 2U
#define SHA512_BLOCK_LENGTH 128U
#define SHA512_DIGEST_LENGTH 64U

typedef struct SHA512CTX
{
	uint64_t state[SHA512_STATE_LENGTH];
	uint64_t count[SHA512_COUNT_LENGTH];
	uint8_t buf[SHA512_BLOCK_LENGTH];
} SHA512CTX;

void SHA512_(const uint8_t* input, size_t length, uint8_t digest[SHA512_DIGEST_LENGTH]);

void SHA512Init(SHA512CTX* context);
void SHA512Update(SHA512CTX* context, const uint8_t* input, size_t length);
void SHA512Final(SHA512CTX* context, uint8_t digest[SHA512_DIGEST_LENGTH]);

#endif // SHA512_H
