// unsigned 256 大数运算

#ifndef __Leex_u256__
#define __Leex_u256__

#pragma warning(disable : 4146)

#include <string>
#include <math.h>
#include <typeinfo>
#include <limits.h>
using namespace std;

#ifndef MBEDTLS_BIGNUM_H
typedef uint32_t mbedtls_mpi_uint;
//typedef uint64_t mbedtls_mpi_uint;

typedef struct
{
	int s;              /*!<  integer sign      */
	size_t n;           /*!<  total # of limbs  */
	mbedtls_mpi_uint *p;          /*!<  pointer to limbs  */
}
mbedtls_mpi;
#endif

class u256
{
private:
	mbedtls_mpi mpi;
	void init();
	void to_unsigned();				// 转化为无符号
	
public:
	u256();							// 构造
	~u256();						// 析构

	u256(const u256& uv);
	u256(const unsigned char bin_data[32]);
	u256(const char* base10_str);
	u256(const string& base10_str);

	string str() const;				// 获取 10 进制字符串
	string hex_str() const;			// 获取 16 进制字符串

	// 获取内部 data
	unsigned char* data();
	void zero();					// 清零
	void neg1();					// 设置为 -1
	bool is_zero() const;			// 判断是否为 0

	// 通用赋值
	template <typename T> u256& operator=(T value)
	{
		zero();

		if (value < 0)
		{
			mpi.s = -1;
			value = -value;
		}

		if ((typeid(T) == typeid(long double)) || (typeid(T) == typeid(double)) || (typeid(T) == typeid(float)))
		{
			T eps;
			if (typeid(T) == typeid(float))
				eps = (T)0.0000001;
			else
				eps = (T)0.000000000000001;

			T q, r, ld = (T)floor(value), u16max = (T)0x10000;
			int m, n;
			for (int i = 0; ld > (T)0.0; i++)
			{
				q = ld / u16max;
				r = (T)floor(q) * u16max;
				r = ld - r + eps;

				n = i / 4;
				m = i % 4;
				if (n >= 4)
					continue;

				mpi.p[n] |= (uint64_t)r << (m * 16);
				ld = (T)floor(q + eps);
			}
		}
		else
			memcpy(&(mpi.p[0]), (void*)&value, sizeof(value));

		to_unsigned();
		return *this;
	}

    // 通用构造
	template <typename T> u256(const T& value)
	{
		init();
		operator=(value);
	}

	// 是否是负值
	bool is_minus() { return (mpi.p[mpi.n-1] & 0x80000000) != 0; }

	u256& operator=(const u256& uv);			// 拷贝赋值
	u256& operator=(const char* base10_str);	// 字符串赋值
	u256& operator=(const string& base10_str);	// 字符串赋值

	// 通用转换
	template <typename T> explicit operator T () const
	{
		T value;

		if ((typeid(T) == typeid(long double)) || (typeid(T) == typeid(double)) || (typeid(T) == typeid(float)))
		{
			long double ld = 0, u32max = (long double)4294967296.0; //ULONG_MAX + 1; ulong可能是32bit可能是64bit
			for (int i = 7; i >= 0; i--)
			{
				ld = ld * u32max + (long double)mpi.p[i];
			}

			value = (T)ld;
		}
		else
			memcpy((void*)&value, &(mpi.p[0]), sizeof(value));

		return value;
	}

	// 移位操作
	u256& operator<<=(const unsigned int shift_count);
	u256 operator<<(const unsigned int shift_count) const;
	u256& operator>>=(const unsigned int shift_count);
	u256 operator>>(const unsigned int shift_count) const;

	// 数学运算
	u256& operator+=(const u256& addend);
	u256 operator+(const u256& addend) const;
	u256& operator-=(const u256& subtrahend);
	u256 operator-(const u256& subtrahend) const;
	u256& operator*=(const u256& multiplier);
	u256 operator*(const u256& multiplierun) const;
	u256& operator/=(const u256& divisor);
	u256 operator/(const u256& divisor) const;
	u256& operator%=(const u256& divisor);
	u256 operator%(const u256& divisor) const;
	u256 mod(const u256& divisor) const;

	// 逻辑运算
	u256& operator ^=(const u256& uv);
	u256 operator^(const u256& value) const;
	u256& operator &=(const u256& uv);
	u256 operator&(const u256& value) const;
	u256& operator |=(const u256& uv);
	u256 operator|(const u256& value) const;
	u256 operator~() { return(u256(0) - (*this)); }		// 取反

	// 比较操作
	bool operator <(const u256& uv) const;
	bool operator <=(const u256& uv) const;
	bool operator >(const u256& uv) const;
	bool operator >=(const u256& uv) const;
	bool operator ==(const u256& uv) const;
	bool operator !=(const u256& uv) const;

	u256 sqrt();					// 求近似平方根

	friend u256 factorial(int n);					// 阶乘运算
	friend u256 mod_exp(u256 a, u256 n, u256 m);	// 幂次取模运算  a^n % m
	friend bool fermat(u256 n);						// 奇数费马测试
};

#endif // __Leex_U256__
