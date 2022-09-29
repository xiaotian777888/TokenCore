
#ifndef RIPEMD160_H
#define RIPEMD160_H

#include <TokenCommon.h>
#include <stddef.h>

#define RMD160_STATE_LENGTH 5U
#define RMD160_CHUNK_LENGTH 16U
#define RMD160_BLOCK_LENGTH 64U
#define RMD160_DIGEST_LENGTH 20U

typedef struct RMD160CTX
{
	uint32_t state[RMD160_STATE_LENGTH];
	uint32_t chunk[RMD160_CHUNK_LENGTH];
} RMD160CTX;

void RMD160(const uint8_t* message, size_t length, uint8_t digest[RMD160_DIGEST_LENGTH]);
void RMDInit(RMD160CTX* context);
void RMDUpdate(RMD160CTX* context, const uint8_t* message, size_t length);
void RMDFinal(RMD160CTX* context, uint8_t digest[RMD160_DIGEST_LENGTH]);

Binary ripemd160(const Binary& in);

#endif // RIPEMD160_H
