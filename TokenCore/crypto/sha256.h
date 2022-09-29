
#ifndef SHA256_H
#define SHA256_H

#include <TokenCommon.h>
#include <stddef.h>

#define SHA256_STATE_LENGTH 8U
#define SHA256_COUNT_LENGTH 2U
#define SHA256_BLOCK_LENGTH 64U
#define SHA256_DIGEST_LENGTH 32U

typedef struct SHA256CTX
{
	uint32_t state[SHA256_STATE_LENGTH];
	uint32_t count[SHA256_COUNT_LENGTH];
	uint8_t buf[SHA256_BLOCK_LENGTH];
} SHA256CTX;

void SHA256_(const uint8_t* input, size_t length, uint8_t digest[SHA256_DIGEST_LENGTH]);

void SHA256Init(SHA256CTX* context);
void SHA256Update(SHA256CTX* context, const uint8_t* input, size_t length);
void SHA256Final(SHA256CTX* context, uint8_t digest[SHA256_DIGEST_LENGTH]);

#endif // SHA256_H
