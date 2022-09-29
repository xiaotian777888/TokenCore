
#ifndef HASH_H
#define HASH_H

#include <TokenCommon.h>

Binary sha160_hash(const Binary& in);
Binary sha256_hash(const Binary& in);
Binary sha512_hash(const Binary& in);
Binary bitcoin160(const Binary& in);
Binary bitcoin256(const Binary& in);
Binary hash160(const Binary& in);
Binary hash256(const Binary& in);

#endif // !HASH_H

