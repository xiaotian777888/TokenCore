#include "hash.h"
#include "sha1.h"
#include "sha256.h"
#include "sha512.h"
#include "ripemd160.h"

Binary sha160_hash(const Binary& in)
{	
	Binary hash(20);
	SHA1_(in.data(), in.size(), hash.data());
	return hash;
}

Binary sha256_hash(const Binary& in)
{
	Binary hash(32);
	SHA256_(in.data(), in.size(), hash.data());
	return hash;
}

Binary sha512_hash(const Binary& in)
{
	Binary hash(64);
	SHA512_(in.data(), in.size(), hash.data());
	return hash;
}

Binary bitcoin160(const Binary& in)
{
	return ripemd160(sha256_hash(in));
}

Binary bitcoin256(const Binary& in)
{
	return sha256_hash(sha256_hash(in));
}

Binary hash160(const Binary& in)
{
	return bitcoin160(in);
}

Binary hash256(const Binary& in)
{
	return bitcoin256(in);
}
