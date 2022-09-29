#include "hmac_sha512.h"
#include "sha512.h"

void HMACSHA512(const uint8_t* input, size_t length, const uint8_t* key, size_t key_length, uint8_t digest[HMACSHA512_DIGEST_LENGTH])
{
	HMACSHA512CTX context;
	HMACSHA512Init(&context, key, key_length);
	HMACSHA512Update(&context, input, length);
	HMACSHA512Final(&context, digest);
}

void HMACSHA512Final(HMACSHA512CTX* context, uint8_t digest[HMACSHA512_DIGEST_LENGTH])
{
	uint8_t hash[HMACSHA512_DIGEST_LENGTH];

	SHA512Final(&context->ictx, hash);
	SHA512Update(&context->octx, hash, HMACSHA512_DIGEST_LENGTH);
	SHA512Final(&context->octx, digest);

	memset((void*)hash, 0, sizeof hash);
}

void HMACSHA512Init(HMACSHA512CTX* context, const uint8_t* key, size_t key_length)
{
	size_t i;
	uint8_t pad[SHA512_BLOCK_LENGTH];
	uint8_t key_hash[SHA512_DIGEST_LENGTH];

	if (key_length > SHA512_BLOCK_LENGTH)
	{
		SHA512Init(&context->ictx);
		SHA512Update(&context->ictx, key, key_length);
		SHA512Final(&context->ictx, key_hash);
		key = key_hash;
		key_length = SHA512_DIGEST_LENGTH;
	}

	memset(pad, 0x36, SHA512_BLOCK_LENGTH);
	for (i = 0; i < key_length; i++)
		pad[i] ^= key[i];

	SHA512Init(&context->ictx);
	SHA512Update(&context->ictx, pad, SHA512_BLOCK_LENGTH);

	memset(pad, 0x5c, SHA512_BLOCK_LENGTH);
	for (i = 0; i < key_length; i++)
		pad[i] ^= key[i];

	SHA512Init(&context->octx);
	SHA512Update(&context->octx, pad, SHA512_BLOCK_LENGTH);

	memset((void*)key_hash, 0, sizeof key_hash);
}

void HMACSHA512Update(HMACSHA512CTX* context, const uint8_t* input, size_t length)
{
	SHA512Update(&context->ictx, input, length);
}
