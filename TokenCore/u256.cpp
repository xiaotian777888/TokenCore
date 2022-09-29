#include <mbedtls/bignum.h>
#include "u256.h"

/*
// ��Ч���ֵĳ���
int u256::length() const
{
	return (int)mbedtls_mpi_size(&mpi);
}
*/

void u256::init()
{
	mbedtls_mpi_init(&mpi);
	mbedtls_mpi_grow(&mpi, 8);
}

void u256::to_unsigned()
{
	if (mpi.s == -1)
	{
		u256 abs_value = *this;
		abs_value.mpi.s = 1;
		neg1();
		mpi.s = 1;
		*this -= abs_value;
		*this += 1;
	}
}

// ����
u256::u256()
{
	init();
}

// ����
u256::~u256()
{
	mbedtls_mpi_free(&mpi);
}

u256::u256(const u256& uv)
{
	init();
	mbedtls_mpi_copy(&mpi, &(uv.mpi));
}

u256::u256(const unsigned char bin_data[32])
{
	init();
	memcpy(&(mpi.p[0]), bin_data, 32);
}

u256::u256(const char* base10_str)
{
	init();
	operator=(base10_str);
}

u256::u256(const string& base10_str)
{
	init();
	operator=(base10_str);
}

unsigned char* u256::data()
{
	return (unsigned char*)&(mpi.p[0]);
}

// ����
void u256::zero()
{
	for (int i = 0; i < mpi.n; i++)
		mpi.p[i] = 0;
}

// ����Ϊ -1
void u256::neg1()
{
	for (int i = 0; i < mpi.n; i++)
		mpi.p[i] = 0xffffffff;
}

// �ж��Ƿ�Ϊ 0
bool u256::is_zero() const
{
	for (int i = 0; i < mpi.n; i++)
		if (mpi.p[i])
			return false;

	return true;
}

// ������ֵ
u256& u256::operator=(const u256& uv)
{
	if (this != &uv)
		mbedtls_mpi_copy(&mpi, &(uv.mpi));

	return *this;
}

// �ַ�����ֵ
u256& u256::operator=(const char* base10_str)
{
	if (0 != mbedtls_mpi_read_string(&mpi, 10, base10_str))
		zero();
	return *this;
}

// �ַ�����ֵ
u256& u256::operator=(const string& base10_str)
{
	if (0 != mbedtls_mpi_read_string(&mpi, 10, base10_str.c_str()))
		zero();
	return *this;
}

// ����
u256& u256::operator<<=(const unsigned int shift_count)
{
	mbedtls_mpi_shift_l(&mpi, shift_count);
	return *this;
}

// ����
u256 u256::operator<<(const unsigned int shift_count) const
{
	u256 uv(*this);

	uv <<= shift_count;
	return(uv);
}

// ����
u256& u256::operator>>=(const unsigned int shift_count)
{
	mbedtls_mpi_shift_r(&mpi, shift_count);
	return *this;
}

// ����
u256 u256::operator>>(const unsigned int shift_count) const
{
	u256 uv(*this);

	uv >>= shift_count;
	return(uv);
}

// �ӷ�
u256& u256::operator+=(const u256& addend)
{
	mbedtls_mpi_add_mpi(&mpi, &mpi, &addend.mpi);
	return *this;
}

// �ӷ�
u256 u256::operator+(const u256& addend) const
{
	u256 uv(*this);

	uv += addend;
	return(uv);
}

// ����
u256& u256::operator-=(const u256& subtrahend)
{
	mbedtls_mpi_sub_mpi(&mpi, &mpi, &subtrahend.mpi);
	to_unsigned();
	return *this;
}

// ����
u256 u256::operator-(const u256& subtrahend) const
{
	u256 uv(*this);

	uv -= subtrahend;
	return(uv);
}

// �˷�
u256& u256::operator*=(const u256& multiplier)
{
	mbedtls_mpi_mul_mpi(&mpi, &mpi, &multiplier.mpi);
	return(*this);
}

// �˷�
u256 u256::operator*(const u256& multiplier) const
{
	u256 uv(*this);

	uv *= multiplier;
	return(uv);
}

// ����
u256& u256::operator/=(const u256& divisor)
{
	mbedtls_mpi_div_mpi(&mpi, nullptr, &mpi, &divisor.mpi);
	return *this;
}

// ����
u256 u256::operator/(const u256& divisor) const
{
	u256 uv(*this);

	uv /= divisor;
	return(uv);
}

// ȡ��
u256& u256::operator%=(const u256& divisor)
{
	mbedtls_mpi_div_mpi(nullptr, &mpi, &mpi, &divisor.mpi);
	return *this;
}

// ȡ��
u256 u256::operator%(const u256& divisor) const
{
	u256 uv(*this);

	uv %= divisor;
	return(uv);
}

// ȡģ��ȡ��Ĳ��
// https://baike.baidu.com/item/%E5%8F%96%E6%A8%A1%E8%BF%90%E7%AE%97/10739384
// ȡģ
u256 u256::mod(const u256& divisor) const
{
	u256 uv;

	mbedtls_mpi_mod_mpi(&uv.mpi, &this->mpi, &divisor.mpi);
	return(uv);
}

// ���
u256& u256::operator ^=(const u256& uv)
{
	for (int i = 0; i < mpi.n; i++)
		mpi.p[i] ^= uv.mpi.p[i];

	return *this;
}

// ���
u256 u256::operator^(const u256& value) const
{
	u256 uv(*this);

	uv ^= value;
	return(uv);
}

// ��
u256& u256::operator &=(const u256& uv)
{
	for (int i = 0; i < mpi.n; i++)
		mpi.p[i] &= uv.mpi.p[i];

	return *this;
}

// ��
u256 u256::operator&(const u256& value) const
{
	u256 uv(*this);

	uv &= value;
	return(uv);
}

// ��
u256& u256::operator |=(const u256& uv)
{
	for (int i = 0; i < mpi.n; i++)
		mpi.p[i] |= uv.mpi.p[i];

	return *this;
}

// ��
u256 u256::operator|(const u256& value) const
{
	u256 uv(*this);

	uv |= value;
	return(uv);
}

// �Ƚϴ�С
bool u256::operator<(const u256& uv) const
{
	return mbedtls_mpi_cmp_mpi(&mpi, &uv.mpi) == -1;
}

// �Ƚϴ�С
bool u256::operator>(const u256& uv) const
{
	return mbedtls_mpi_cmp_mpi(&mpi, &uv.mpi) == 1;
}

// �Ƚϴ�С
bool u256::operator<=(const u256& uv) const
{
	return(!operator>(uv));
}

// �Ƚϴ�С
bool u256::operator>=(const u256& uv) const
{
	return(!operator<(uv));
}

// �Ƚϴ�С
bool u256::operator==(const u256& uv) const
{
	return mbedtls_mpi_cmp_mpi(&mpi, &uv.mpi) == 0;
}

// �Ƚϴ�С
bool u256::operator!=(const u256& uv) const
{
	return(!operator==(uv));
}

// ��ȡ 10 �����ַ���
string u256::str() const 
{
	char buf[1024];
	size_t buflen = 1024, olen = 0;
	mbedtls_mpi_write_string(&mpi, 10, buf, buflen, &olen);

	return(string(buf));
}

// ��ȡ 16 �����ַ���
string u256::hex_str() const
{
	char buf[1024];
	size_t buflen = 1024, olen = 0;
	mbedtls_mpi_write_string(&mpi, 16, buf, buflen, &olen);

	return(string(buf));
}

// ţ�ٵ���������Ѱ��ƽ����
// ������ŵĽ���ֵ����������һ������ֵx��Ȼ�󲻶���x����x��n/x��ƽ���������������ߴκ�x��ֵ���Ѿ��൱��ȷ�ˡ�
// �ö��ֱƽ��㷨��ƽ����
u256 u256::sqrt()
{
	u256 x, p;

	x = *this >> 1;
	do
	{
		p = x;
		x = (x + *this / x) >> 1;
	} while (x < p);

	if (x != p)
		return(x - (u256)1);

	return(x);
}

// ͬ������: ��a-b�ɱ�n��������a��b��nͬ�ࡣ
// �ݴ�ȡģ����  a^e % n
u256 mod_exp(u256 a, u256 e, u256 n)
{
	/*
	u256 x, rr;
	mbedtls_mpi_exp_mod(&x.mpi, &a.mpi, &e.mpi, &n.mpi, &rr.mpi);
	return x;
	*/
	u256 y = 1;

	while (!e.is_zero())
	{
		if (e.mpi.p[0] & 1)
			y = (y*a) % n;
		e = e >> 1;
		a = (a*a) % n;
	}
	return(y);
}

// �׳�����
u256 factorial(int n)
{
	u256 fac(1);
	for (int i = 0; i < n; i++)
	{
		fac = fac * (u256)(i + 1);
	}
	return(fac);
}

// ����������ԡ�
// ������Բ�����ȷ�����Ƿ���������ͨ����������Ժ�������������ĸ���΢����΢��
// �����Ƿ�����Ե�ϸ�ڣ� 
// 1. �������� n
// 2. ����������������ѡȡһ������ b ��b = 2,3,5,7......
// 3. ���� w = b^(n-1) % n
// 4. ����������� b ��w��Ϊ1��n �ܿ��������������� n һ���Ǻ�����
// ȡǰ�ĸ������Ĳ��Գ�Ϊ�Ľ׷�����ԣ�PGP���õľ�������
// һ�ײ������еĸ����� 10^-13�����׺��� 10^-26�������Ľײ��Ժ��� 10^-52��
// ���������Բ���©��һ����������Ȼ����������Ϊ�����Ŀ������Ǵ��ڵģ�
// ������ͨ��������Եĺ�������ΪCarmichael ������ 561,1105,1729 �ȵȡ�
// ���Ǻ�ϡ�٣��� 10^9 ��Χ��ֻ�� 255 ���� 

// 100000 ���ڵ� Carmichael ��
// 561, 1105, 1729, 2465, 2821, 6601, 8911, 10585, 15841, 29341, 41041, 46657, 52633, 62745, 63973, and 75361. 

// ͬ������: ��a-b�ɱ�m��������a��b��mͬ�ࡣ
// http://primes.utm.edu/ ������ҳ

bool fermat(u256 n)	// n Ϊ������
{
	if (mod_exp(2, n - (u256)1, n) != 1) return(false);
	if (mod_exp(3, n - (u256)1, n) != 1) return(false);
	if (mod_exp(5, n - (u256)1, n) != 1) return(false);
	if (mod_exp(7, n - (u256)1, n) != 1) return(false);
	return(true);
}
