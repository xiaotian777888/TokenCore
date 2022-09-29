#include <mbedtls/bignum.h>
#include "u256.h"

/*
// 有效数字的长度
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

// 构造
u256::u256()
{
	init();
}

// 析构
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

// 清零
void u256::zero()
{
	for (int i = 0; i < mpi.n; i++)
		mpi.p[i] = 0;
}

// 设置为 -1
void u256::neg1()
{
	for (int i = 0; i < mpi.n; i++)
		mpi.p[i] = 0xffffffff;
}

// 判断是否为 0
bool u256::is_zero() const
{
	for (int i = 0; i < mpi.n; i++)
		if (mpi.p[i])
			return false;

	return true;
}

// 拷贝赋值
u256& u256::operator=(const u256& uv)
{
	if (this != &uv)
		mbedtls_mpi_copy(&mpi, &(uv.mpi));

	return *this;
}

// 字符串赋值
u256& u256::operator=(const char* base10_str)
{
	if (0 != mbedtls_mpi_read_string(&mpi, 10, base10_str))
		zero();
	return *this;
}

// 字符串赋值
u256& u256::operator=(const string& base10_str)
{
	if (0 != mbedtls_mpi_read_string(&mpi, 10, base10_str.c_str()))
		zero();
	return *this;
}

// 左移
u256& u256::operator<<=(const unsigned int shift_count)
{
	mbedtls_mpi_shift_l(&mpi, shift_count);
	return *this;
}

// 左移
u256 u256::operator<<(const unsigned int shift_count) const
{
	u256 uv(*this);

	uv <<= shift_count;
	return(uv);
}

// 右移
u256& u256::operator>>=(const unsigned int shift_count)
{
	mbedtls_mpi_shift_r(&mpi, shift_count);
	return *this;
}

// 右移
u256 u256::operator>>(const unsigned int shift_count) const
{
	u256 uv(*this);

	uv >>= shift_count;
	return(uv);
}

// 加法
u256& u256::operator+=(const u256& addend)
{
	mbedtls_mpi_add_mpi(&mpi, &mpi, &addend.mpi);
	return *this;
}

// 加法
u256 u256::operator+(const u256& addend) const
{
	u256 uv(*this);

	uv += addend;
	return(uv);
}

// 减法
u256& u256::operator-=(const u256& subtrahend)
{
	mbedtls_mpi_sub_mpi(&mpi, &mpi, &subtrahend.mpi);
	to_unsigned();
	return *this;
}

// 减法
u256 u256::operator-(const u256& subtrahend) const
{
	u256 uv(*this);

	uv -= subtrahend;
	return(uv);
}

// 乘法
u256& u256::operator*=(const u256& multiplier)
{
	mbedtls_mpi_mul_mpi(&mpi, &mpi, &multiplier.mpi);
	return(*this);
}

// 乘法
u256 u256::operator*(const u256& multiplier) const
{
	u256 uv(*this);

	uv *= multiplier;
	return(uv);
}

// 除法
u256& u256::operator/=(const u256& divisor)
{
	mbedtls_mpi_div_mpi(&mpi, nullptr, &mpi, &divisor.mpi);
	return *this;
}

// 除法
u256 u256::operator/(const u256& divisor) const
{
	u256 uv(*this);

	uv /= divisor;
	return(uv);
}

// 取余
u256& u256::operator%=(const u256& divisor)
{
	mbedtls_mpi_div_mpi(nullptr, &mpi, &mpi, &divisor.mpi);
	return *this;
}

// 取余
u256 u256::operator%(const u256& divisor) const
{
	u256 uv(*this);

	uv %= divisor;
	return(uv);
}

// 取模与取余的差别
// https://baike.baidu.com/item/%E5%8F%96%E6%A8%A1%E8%BF%90%E7%AE%97/10739384
// 取模
u256 u256::mod(const u256& divisor) const
{
	u256 uv;

	mbedtls_mpi_mod_mpi(&uv.mpi, &this->mpi, &divisor.mpi);
	return(uv);
}

// 异或
u256& u256::operator ^=(const u256& uv)
{
	for (int i = 0; i < mpi.n; i++)
		mpi.p[i] ^= uv.mpi.p[i];

	return *this;
}

// 异或
u256 u256::operator^(const u256& value) const
{
	u256 uv(*this);

	uv ^= value;
	return(uv);
}

// 与
u256& u256::operator &=(const u256& uv)
{
	for (int i = 0; i < mpi.n; i++)
		mpi.p[i] &= uv.mpi.p[i];

	return *this;
}

// 与
u256 u256::operator&(const u256& value) const
{
	u256 uv(*this);

	uv &= value;
	return(uv);
}

// 或
u256& u256::operator |=(const u256& uv)
{
	for (int i = 0; i < mpi.n; i++)
		mpi.p[i] |= uv.mpi.p[i];

	return *this;
}

// 或
u256 u256::operator|(const u256& value) const
{
	u256 uv(*this);

	uv |= value;
	return(uv);
}

// 比较大小
bool u256::operator<(const u256& uv) const
{
	return mbedtls_mpi_cmp_mpi(&mpi, &uv.mpi) == -1;
}

// 比较大小
bool u256::operator>(const u256& uv) const
{
	return mbedtls_mpi_cmp_mpi(&mpi, &uv.mpi) == 1;
}

// 比较大小
bool u256::operator<=(const u256& uv) const
{
	return(!operator>(uv));
}

// 比较大小
bool u256::operator>=(const u256& uv) const
{
	return(!operator<(uv));
}

// 比较大小
bool u256::operator==(const u256& uv) const
{
	return mbedtls_mpi_cmp_mpi(&mpi, &uv.mpi) == 0;
}

// 比较大小
bool u256::operator!=(const u256& uv) const
{
	return(!operator==(uv));
}

// 获取 10 进制字符串
string u256::str() const 
{
	char buf[1024];
	size_t buflen = 1024, olen = 0;
	mbedtls_mpi_write_string(&mpi, 10, buf, buflen, &olen);

	return(string(buf));
}

// 获取 16 进制字符串
string u256::hex_str() const
{
	char buf[1024];
	size_t buflen = 1024, olen = 0;
	mbedtls_mpi_write_string(&mpi, 16, buf, buflen, &olen);

	return(string(buf));
}

// 牛顿迭代法快速寻找平方根
// 求出根号的近似值：首先随便猜一个近似值x，然后不断令x等于x和n/x的平均数，迭代个六七次后x的值就已经相当精确了。
// 用二分逼近算法求平方根
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

// 同余运算: 若a-b可被n整除，则a，b对n同余。
// 幂次取模运算  a^e % n
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

// 阶乘运算
u256 factorial(int n)
{
	u256 fac(1);
	for (int i = 0; i < n; i++)
	{
		fac = fac * (u256)(i + 1);
	}
	return(fac);
}

// 奇数费马测试。
// 费马测试并不能确定它是否质数，但通过费马测试以后的数不是质数的概率微乎其微。
// 下面是费马测试的细节： 
// 1. 待测奇数 n
// 2. 在质数集合中依次选取一个质数 b ，b = 2,3,5,7......
// 3. 计算 w = b^(n-1) % n
// 4. 如果对于所有 b ，w都为1，n 很可能是质数。否则 n 一定是合数。
// 取前四个质数的测试称为四阶费马测试，PGP采用的就是它。
// 一阶测试误判的概率是 10^-13，二阶后是 10^-26，作完四阶测试后是 10^-52。
// 而且它绝对不会漏掉一个质数。当然将合数误判为质数的可能性是存在的，
// 这种能通过费马测试的合数被称为Carmichael 数，象 561,1105,1729 等等。
// 它们很稀少，在 10^9 范围内只有 255 个。 

// 100000 以内的 Carmichael 数
// 561, 1105, 1729, 2465, 2821, 6601, 8911, 10585, 15841, 29341, 41041, 46657, 52633, 62745, 63973, and 75361. 

// 同余运算: 若a-b可被m整除，则a，b对m同余。
// http://primes.utm.edu/ 质数主页

bool fermat(u256 n)	// n 为大奇数
{
	if (mod_exp(2, n - (u256)1, n) != 1) return(false);
	if (mod_exp(3, n - (u256)1, n) != 1) return(false);
	if (mod_exp(5, n - (u256)1, n) != 1) return(false);
	if (mod_exp(7, n - (u256)1, n) != 1) return(false);
	return(true);
}
