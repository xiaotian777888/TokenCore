#include "hmac.h"
#include "sha2.h"
#include "utility_tools.h"

void hmac_sha512_Init(HMAC_SHA512_CTX *hctx, const uint8_t *key, const uint32_t keylen)
{
	uint8_t i_key_pad[SHA512_BLOCK_LENGTH];
	memset(i_key_pad, 0, SHA512_BLOCK_LENGTH);
	if (keylen > SHA512_BLOCK_LENGTH) {
		sha512_Raw(key, keylen, i_key_pad);
	} else {
		memcpy(i_key_pad, key, keylen);
	}
	for (int i = 0; i < SHA512_BLOCK_LENGTH; i++) {
		hctx->o_key_pad[i] = i_key_pad[i] ^ 0x5c;
		i_key_pad[i] ^= 0x36;
	}
	sha512_Init(&(hctx->ctx));
	sha512_Update(&(hctx->ctx), i_key_pad, SHA512_BLOCK_LENGTH);
	memset(i_key_pad, 0, sizeof(i_key_pad));
}

void hmac_sha512_Update(HMAC_SHA512_CTX *hctx, const uint8_t *msg, const uint32_t msglen)
{
	sha512_Update(&(hctx->ctx), msg, msglen);
}

void hmac_sha512_Final(HMAC_SHA512_CTX *hctx, uint8_t *hmac)
{
	sha512_Final(&(hctx->ctx), hmac);
	sha512_Init(&(hctx->ctx));
	sha512_Update(&(hctx->ctx), hctx->o_key_pad, SHA512_BLOCK_LENGTH);
	sha512_Update(&(hctx->ctx), hmac, SHA512_DIGEST_LENGTH);
	sha512_Final(&(hctx->ctx), hmac);
	memset(hctx, 0, sizeof(HMAC_SHA512_CTX));
}

void hmac_sha512(const uint8_t *key, const uint32_t keylen, const uint8_t *msg, const uint32_t msglen, uint8_t *hmac)
{
	HMAC_SHA512_CTX hctx;
	hmac_sha512_Init(&hctx, key, keylen);
	hmac_sha512_Update(&hctx, msg, msglen);
	hmac_sha512_Final(&hctx, hmac);
}

void hmac_sha512_prepare(const uint8_t *key, const uint32_t keylen, uint64_t *opad_digest, uint64_t *ipad_digest)
{
	uint64_t key_pad[SHA512_BLOCK_LENGTH/sizeof(uint64_t)];

	memset(key_pad, 0, sizeof(key_pad));
	if (keylen > SHA512_BLOCK_LENGTH)
	{
		static SHA512_CTX context;
		sha512_Init(&context);
		sha512_Update(&context, key, keylen);
		sha512_Final(&context, (uint8_t*)key_pad);
	}
	else
	{
		memcpy(key_pad, key, keylen);
	}

	/* compute o_key_pad and its digest */
	for (int i = 0; i < SHA512_BLOCK_LENGTH/(int)sizeof(uint64_t); i++)
	{
		uint64_t data;
#if BYTE_ORDER == LITTLE_ENDIAN
		data = bswap_64(key_pad[i]);
#else
		data = key_pad[i];
#endif
		key_pad[i] = data ^ 0x5c5c5c5c5c5c5c5c;
	}

	sha512_Transform(sha512_initial_hash_value, key_pad, opad_digest);

	/* convert o_key_pad to i_key_pad and compute its digest */
	for (int i = 0; i < SHA512_BLOCK_LENGTH/(int)sizeof(uint64_t); i++) {
		key_pad[i] = key_pad[i] ^ 0x5c5c5c5c5c5c5c5c ^ 0x3636363636363636;
	}

	sha512_Transform(sha512_initial_hash_value, key_pad, ipad_digest);
	memset(key_pad, 0, sizeof(key_pad));
}
