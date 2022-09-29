/* Copyright 2014, Kenneth MacKay. Licensed under the BSD 2-clause license. */

#ifndef _MICRO_ECC_H_
#define _MICRO_ECC_H_

#include <stdint.h>

/* Platform selection options.
If mECC_PLATFORM is not defined, the code will try to guess it based on compiler macros.
Possible values for mECC_PLATFORM are defined below: */
#define mECC_arch_other 0
#define mECC_x86        1
#define mECC_x86_64     2
#define mECC_arm        3
#define mECC_arm_thumb  4
#define mECC_avr        5
#define mECC_arm_thumb2 6

/* If desired, you can define mECC_WORD_SIZE as appropriate for your platform (1, 4, or 8 bytes).
If mECC_WORD_SIZE is not explicitly defined then it will be automatically set based on your
platform. */

/* Inline assembly options.
mECC_asm_none  - Use standard C99 only.
mECC_asm_small - Use GCC inline assembly for the target platform (if available), optimized for
                 minimum size.
mECC_asm_fast  - Use GCC inline assembly optimized for maximum speed. */
#define mECC_asm_none  0
#define mECC_asm_small 1
#define mECC_asm_fast  2
#ifndef mECC_ASM
    #define mECC_ASM mECC_asm_none
#endif

/* Curve selection options. */
#define mECC_secp160r1 1
#define mECC_secp192r1 2
#define mECC_secp256r1 3
#define mECC_secp256k1 4
#define mECC_secp224r1 5
#ifndef mECC_CURVE
    #define mECC_CURVE mECC_secp256k1
#endif

/* mECC_SQUARE_FUNC - If enabled (defined as nonzero), this will cause a specific function to be
used for (scalar) squaring instead of the generic multiplication function. This will make things
faster by about 8% but increases the code size. */
#ifndef mECC_SQUARE_FUNC
    #define mECC_SQUARE_FUNC 1
#endif

#define mECC_CONCAT1(a, b) a##b
#define mECC_CONCAT(a, b) mECC_CONCAT1(a, b)

#define mECC_size_1 20 /* secp160r1 */
#define mECC_size_2 24 /* secp192r1 */
#define mECC_size_3 32 /* secp256r1 */
#define mECC_size_4 32 /* secp256k1 */
#define mECC_size_5 28 /* secp224r1 */

#define mECC_BYTES mECC_CONCAT(mECC_size_, mECC_CURVE)

#ifdef __cplusplus
extern "C"
{
#endif

/* mECC_RNG_Function type
The RNG function should fill 'size' random bytes into 'dest'. It should return 1 if
'dest' was filled with random data, or 0 if the random data could not be generated.
The filled-in values should be either truly random, or from a cryptographically-secure PRNG.

A correctly functioning RNG function must be set (using mECC_set_rng()) before calling
mECC_make_key() or mECC_sign().

Setting a correctly functioning RNG function improves the resistance to side-channel attacks
for mECC_shared_secret() and mECC_sign_deterministic().

A correct RNG function is set by default when building for Windows, Linux, or OS X.
If you are building on another POSIX-compliant system that supports /dev/random or /dev/urandom,
you can define mECC_POSIX to use the predefined RNG. For embedded platforms there is no predefined
RNG function; you must provide your own.
*/
typedef int (*mECC_RNG_Function)(uint8_t *dest, unsigned size);

/* mECC_set_rng() function.
Set the function that will be used to generate random bytes. The RNG function should
return 1 if the random data was generated, or 0 if the random data could not be generated.

On platforms where there is no predefined RNG function (eg embedded platforms), this must
be called before mECC_make_key() or mECC_sign() are used.

Inputs:
    rng_function - The function that will be used to generate random bytes.
*/
void mECC_set_rng(mECC_RNG_Function rng_function);

/* mECC_make_key() function.
Create a public/private key pair.

Outputs:
    public_key  - Will be filled in with the public key.
    private_key - Will be filled in with the private key.

Returns 1 if the key pair was generated successfully, 0 if an error occurred.
*/
int mECC_make_key(uint8_t public_key[mECC_BYTES*2], uint8_t private_key[mECC_BYTES]);

/* mECC_shared_secret() function.
Compute a shared secret given your secret key and someone else's public key.
Note: It is recommended that you hash the result of mECC_shared_secret() before using it for
symmetric encryption or HMAC.

Inputs:
    public_key  - The public key of the remote party.
    private_key - Your private key.

Outputs:
    secret - Will be filled in with the shared secret value.

Returns 1 if the shared secret was generated successfully, 0 if an error occurred.
*/
int mECC_shared_secret(const uint8_t public_key[mECC_BYTES*2],
                       const uint8_t private_key[mECC_BYTES],
                       uint8_t secret[mECC_BYTES]);

/* mECC_sign() function.
Generate an ECDSA signature for a given hash value.

Usage: Compute a hash of the data you wish to sign (SHA-2 is recommended) and pass it in to
this function along with your private key.

Inputs:
    private_key  - Your private key.
    message_hash - The hash of the message to sign.

Outputs:
    signature - Will be filled in with the signature value.

Returns 1 if the signature generated successfully, 0 if an error occurred.
*/
int mECC_sign(const uint8_t private_key[mECC_BYTES],
              const uint8_t message_hash[mECC_BYTES],
              uint8_t signature[mECC_BYTES*2]);

//return the recid
int mECC_sign_forbc(const uint8_t private_key[mECC_BYTES],
              const uint8_t message_hash[mECC_BYTES],
              uint8_t signature[mECC_BYTES*2]);
/* mECC_HashContext structure.
This is used to pass in an arbitrary hash function to mECC_sign_deterministic().
The structure will be used for multiple hash computations; each time a new hash
is computed, init_hash() will be called, followed by one or more calls to
update_hash(), and finally a call to finish_hash() to prudoce the resulting hash.

The intention is that you will create a structure that includes mECC_HashContext
followed by any hash-specific data. For example:

typedef struct SHA256_HashContext {
    mECC_HashContext mECC;
    SHA256_CTX ctx;
} SHA256_HashContext;

void init_SHA256(mECC_HashContext *base) {
    SHA256_HashContext *context = (SHA256_HashContext *)base;
    SHA256_Init(&context->ctx);
}

void update_SHA256(mECC_HashContext *base,
                   const uint8_t *message,
                   unsigned message_size) {
    SHA256_HashContext *context = (SHA256_HashContext *)base;
    SHA256_Update(&context->ctx, message, message_size);
}

void finish_SHA256(mECC_HashContext *base, uint8_t *hash_result) {
    SHA256_HashContext *context = (SHA256_HashContext *)base;
    SHA256_Final(hash_result, &context->ctx);
}

... when signing ...
{
    uint8_t tmp[32 + 32 + 64];
    SHA256_HashContext ctx = {{&init_SHA256, &update_SHA256, &finish_SHA256, 64, 32, tmp}};
    mECC_sign_deterministic(key, message_hash, &ctx.mECC, signature);
}
*/
typedef struct mECC_HashContext {
    void (*init_hash)(struct mECC_HashContext *context);
    void (*update_hash)(struct mECC_HashContext *context,
                        const uint8_t *message,
                        unsigned message_size);
    void (*finish_hash)(struct mECC_HashContext *context, uint8_t *hash_result);
    unsigned block_size; /* Hash function block size in bytes, eg 64 for SHA-256. */
    unsigned result_size; /* Hash function result size in bytes, eg 32 for SHA-256. */
    uint8_t *tmp; /* Must point to a buffer of at least (2 * result_size + block_size) bytes. */
} mECC_HashContext;

/* mECC_sign_deterministic() function.
Generate an ECDSA signature for a given hash value, using a deterministic algorithm
(see RFC 6979). You do not need to set the RNG using mECC_set_rng() before calling
this function; however, if the RNG is defined it will improve resistance to side-channel
attacks.

Usage: Compute a hash of the data you wish to sign (SHA-2 is recommended) and pass it in to
this function along with your private key and a hash context.

Inputs:
    private_key  - Your private key.
    message_hash - The hash of the message to sign.
    hash_context - A hash context to use.

Outputs:
    signature - Will be filled in with the signature value.

Returns 1 if the signature generated successfully, 0 if an error occurred.
*/
int mECC_sign_deterministic(const uint8_t private_key[mECC_BYTES],
                            const uint8_t message_hash[mECC_BYTES],
                            mECC_HashContext *hash_context,
                            uint8_t signature[mECC_BYTES*2]);

/* mECC_verify() function.
Verify an ECDSA signature.

Usage: Compute the hash of the signed data using the same hash as the signer and
pass it to this function along with the signer's public key and the signature values (r and s).

Inputs:
    public_key - The signer's public key
    hash       - The hash of the signed data.
    signature  - The signature value.

Returns 1 if the signature is valid, 0 if it is invalid.
*/
int mECC_verify(const uint8_t public_key[mECC_BYTES*2],
                const uint8_t hash[mECC_BYTES],
                const uint8_t signature[mECC_BYTES*2]);

/* mECC_compress() function.
Compress a public key.

Inputs:
    public_key - The public key to compress.

Outputs:
    compressed - Will be filled in with the compressed public key.
*/
void mECC_compress(const uint8_t public_key[mECC_BYTES*2], uint8_t compressed[mECC_BYTES+1]);

/* mECC_decompress() function.
Decompress a compressed public key.

Inputs:
    compressed - The compressed public key.

Outputs:
    public_key - Will be filled in with the decompressed public key.
*/
void mECC_decompress(const uint8_t compressed[mECC_BYTES+1], uint8_t public_key[mECC_BYTES*2]);

/* mECC_valid_public_key() function.
Check to see if a public key is valid.

Note that you are not required to check for a valid public key before using any other mECC
functions. However, you may wish to avoid spending CPU time computing a shared secret or
verifying a signature using an invalid public key.

Inputs:
    public_key - The public key to check.

Returns 1 if the public key is valid, 0 if it is invalid.
*/
int mECC_valid_public_key(const uint8_t public_key[mECC_BYTES*2]);

/* mECC_compute_public_key() function.
Compute the corresponding public key for a private key.

Inputs:
    private_key - The private key to compute the public key for

Outputs:
    public_key - Will be filled in with the corresponding public key

Returns 1 if the key was computed successfully, 0 if an error occurred.
*/
int mECC_compute_public_key(const uint8_t private_key[mECC_BYTES],
                            uint8_t public_key[mECC_BYTES * 2]);


/* mECC_bytes() function.
Returns the value of mECC_BYTES. Helpful for foreign-interfaces to higher-level languages.
*/
int mECC_bytes(void);

/* mECC_curve() function.
Returns the value of mECC_CURVE. Helpful for foreign-interfaces to higher-level languages.
*/
int mECC_curve(void);


int macro_getRecId(const uint8_t private_key[mECC_BYTES],
            const uint8_t hash[mECC_BYTES],
            const uint8_t signature[mECC_BYTES*2]);

int macro_checkSignature(const uint8_t* public_key,
                    const uint8_t* hash,
                    const uint8_t* signature);
    
#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif /* _MICRO_ECC_H_ */
