//#include <string.h>
#include "pbkdf2.h"
#include "sha2.h"
#include "utility_tools.h"
#include "hmac.h"

void pbkdf2_hmac_sha512_Init(PBKDF2_HMAC_SHA512_CTX *pctx, const uint8_t *pass, int passlen, const uint8_t *salt, int saltlen)
{
	SHA512_CTX ctx;
	uint32_t blocknr = 1;

#if BYTE_ORDER == LITTLE_ENDIAN
	blocknr = bswap_32(blocknr);
#endif

	hmac_sha512_prepare(pass, passlen, pctx->odig, pctx->idig);
	memset(pctx->g, 0, sizeof(pctx->g));
	pctx->g[8] = 0x8000000000000000;
	pctx->g[15] = (SHA512_BLOCK_LENGTH + SHA512_DIGEST_LENGTH) * 8;

	memcpy (ctx.state, pctx->idig, sizeof(pctx->idig));
	ctx.bitcount[0] = SHA512_BLOCK_LENGTH * 8;
	ctx.bitcount[1] = 0;
	sha512_Update(&ctx, salt, saltlen);
	sha512_Update(&ctx, (uint8_t*)&blocknr, sizeof(blocknr));
	sha512_Final(&ctx, (uint8_t*)pctx->g);

#if BYTE_ORDER == LITTLE_ENDIAN
	for (uint32_t k = 0; k < SHA512_DIGEST_LENGTH / sizeof(uint64_t); k++)
	{
		pctx->g[k] = bswap_64(pctx->g[k]);
	}
#endif
	sha512_Transform(pctx->odig, pctx->g, pctx->g);
	memcpy(pctx->f, pctx->g, SHA512_DIGEST_LENGTH);
	pctx->first = 1;
}

void pbkdf2_hmac_sha512_Update(PBKDF2_HMAC_SHA512_CTX *pctx, uint32_t iterations)
{
	for (uint32_t i = pctx->first; i < iterations; i++)
	{
		sha512_Transform(pctx->idig, pctx->g, pctx->g);
		sha512_Transform(pctx->odig, pctx->g, pctx->g);
		for (uint32_t j = 0; j < SHA512_DIGEST_LENGTH / sizeof(uint64_t); j++)
		{
			pctx->f[j] ^= pctx->g[j];
		}
	}
	pctx->first = 0;
}

void pbkdf2_hmac_sha512_Final(PBKDF2_HMAC_SHA512_CTX *pctx, uint8_t *key)
{
#if BYTE_ORDER == LITTLE_ENDIAN
	for (uint32_t k = 0; k < SHA512_DIGEST_LENGTH/sizeof(uint64_t); k++)
	{
		pctx->f[k] = bswap_64(pctx->f[k]);
	}
#endif
	memcpy(key, pctx->f, SHA512_DIGEST_LENGTH);
	memset(pctx, 0, sizeof(PBKDF2_HMAC_SHA512_CTX));
}

void pbkdf2_hmac_sha512(const uint8_t *pass, int passlen, const uint8_t *salt, int saltlen, uint32_t iterations, uint8_t *key)
{
	PBKDF2_HMAC_SHA512_CTX pctx;
	pbkdf2_hmac_sha512_Init(&pctx, pass, passlen, salt, saltlen);
	pbkdf2_hmac_sha512_Update(&pctx, iterations);
	pbkdf2_hmac_sha512_Final(&pctx, key);
}
