
#ifndef HMACSHA512_H
#define HMACSHA512_H

#include <TokenCommon.h>
#include <stddef.h>
#include "sha512.h"

#define HMACSHA512_DIGEST_LENGTH 64U

typedef struct HMACSHA512CTX
{
	SHA512CTX ictx;
	SHA512CTX octx;
} HMACSHA512CTX;

void HMACSHA512(const uint8_t* input, size_t length, const uint8_t* key, size_t key_length, uint8_t digest[HMACSHA512_DIGEST_LENGTH]);

void HMACSHA512Final(HMACSHA512CTX* context, uint8_t digest[HMACSHA512_DIGEST_LENGTH]);
void HMACSHA512Init(HMACSHA512CTX* context, const uint8_t* key,	size_t key_length);
void HMACSHA512Update(HMACSHA512CTX* context, const uint8_t* input,	size_t length);

#endif // HMACSHA512_H
