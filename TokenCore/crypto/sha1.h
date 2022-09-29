
#ifndef SHA1_H
#define SHA1_H

#include <TokenCommon.h>
#include <stddef.h>

#define SHA1_STATE_LENGTH 5U
#define SHA1_BLOCK_LENGTH 64U
#define SHA1_DIGEST_LENGTH 20U

typedef struct SHA1CTX
{
	uint32_t state[SHA1_STATE_LENGTH];
	uint8_t block[SHA1_BLOCK_LENGTH];
	size_t length;
	size_t index;
} SHA1CTX;

void SHA1_(const uint8_t* message, size_t length, uint8_t digest[SHA1_DIGEST_LENGTH]);

void SHA1Init(SHA1CTX* context);
void SHA1Update(SHA1CTX* context, const uint8_t* message, size_t length);
void SHA1Final(SHA1CTX* context, uint8_t digest[SHA1_DIGEST_LENGTH]);

#endif // SHA1_H
