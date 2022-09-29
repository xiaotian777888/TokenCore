#pragma once

#include <stdint.h>
#include <QString>
#include <QDate>
#include <QLocale>
#include <QTranslator>

#include <vector>
#include <deque>
#include <map>

using namespace std;

#include <TokenCommon.h>

//#include <boost/multiprecision/cpp_int.hpp>
//using u256 = boost::multiprecision::uint256_t;

//数字货币类型
enum CoinTypeMajor
{
	UNKNOW = -1,
	BTC = 0,
	ETH = 1,
	EOS = 2,
	BHP = 3,
	TRX = 4,
};

enum CoinTypeMinor
{
	BTC_BTC = 0,
	EOS_EOS = 0,
	ETH_ETH = 0,
	BHP_BHP = 0,
	TRX_TRX = 0,
	BTC_USDT = 1,
	ETH_AE = 1,
	ETH_BAT = 2,
	ETH_BTM = 3,
	ETH_HT = 4,
	ETH_OMG = 5,
	ETH_NAS = 6,
	ETH_ELF = 7,
	ETH_ZIL = 8,
	ETH_BLZ = 9,
	ETH_VEN = 10,
	ETH_THETA = 11,
	ETH_ZRX = 12,
	ETH_BNB = 13,
	ETH_ICX = 14,
	ETH_WAX = 15,
	ETH_POLY = 16,
	ETH_LBA = 17,
	ETH_KNC = 18,
	ETH_RUFF = 19,
	ETH_CMT = 20,
	ETH_WTC = 21,
	ETH_QSP = 22,
	ETH_SNT = 23,
	ETH_RRC = 24,
	ETH_GUSD = 25,
	ETH_PLY = 26,
	ETH_LRC = 27,
	ETH_POWR = 28,
	ETH_RNT = 29,
	ETH_STORM = 30,
	ETH_OST = 31,
	ETH_DELC = 32,
	ETH_GGC = 33,
	ETH_GGT = 34,
};

class CoinType
{
public:
	unsigned short major;
	unsigned short minor;
	unsigned long chain_id;

	constexpr CoinType() : major(-1), minor(0), chain_id(0) { }

	constexpr CoinType(unsigned short major, unsigned short minor, unsigned long chain_id) :
		major(major), minor(minor), chain_id(chain_id)
	{
	}

	CoinType(const CoinType& coin_type)
	{
		major = coin_type.major;
		minor = coin_type.minor;
		chain_id = coin_type.chain_id;
	}

	CoinType& operator=(const CoinType& coin_type)
	{
		if (this != &coin_type)
		{
			major = coin_type.major;
			minor = coin_type.minor;
			chain_id = coin_type.chain_id;
		}
		return *this;
	}

	bool operator== (const CoinType& coin_type) const
	{
		return (major == coin_type.major) && (minor == coin_type.minor) && (chain_id == coin_type.chain_id);
	}

	bool operator!= (const CoinType& coin_type) const
	{
		return(!operator==(coin_type));
	}

	bool operator < (const CoinType& coin_type) const
	{
		return (major == coin_type.major) ? (minor == coin_type.minor ? chain_id < coin_type.chain_id : minor < coin_type.minor) : major < coin_type.major;
	}

	bool is_testnet() const
	{
		return((major == BTC && chain_id != 0) || (major == ETH && chain_id != 1) || (major == EOS && chain_id != 0));
	}
};

class Coin
{
public:
	QString symbol;				// 符号名
	QString display_name;		// 中文名(或某种指定语言的名字)
	QString contract_address;	// 合约地址
	int precision;				// 小数点后 0 的个数
	CoinType type;				// 类型

	Coin() {}

	Coin(const char* symbol, const char* display_name, const char* contract_address, const int precision, const CoinType& type) :
		symbol(symbol)
		, display_name(QObject::trUtf8(display_name))
		, contract_address(QObject::trUtf8(contract_address))
		, precision(precision)
		, type(type)
	{
	}

	Coin& operator=(const Coin& coin)
	{
		if (this != &coin)
		{
			symbol = coin.symbol;
			type = coin.type;
		}
		return *this;
	}

	double to_double(u256 value) const
	{
		double dvalue = (double)value;
		for (int i = 0; i < precision; i++)
			dvalue = dvalue / 10;
		return dvalue;
	}

	QString to_display(u256 value, bool trim = true) const
	{
		QString str(QString::fromStdString(value.str()));
		int len = str.length();
		if (len > precision)
			str = str.left(len - precision) + QString(".") + str.right(precision);
		else
		{
			QString prefix("0.");
			for (int i = len; i < precision; i++)
				prefix.append("0");
			str = prefix + str;
		}

		if (str.length() > 10)
			str = str.left(10);

		if (!trim) {
			return str;
		}

		int i;
		for (i = str.length() - 1; i >= 0; i--)
		{
			//找末尾第一个不是0的地方
			if (str[i] != '0')
				break;
		}

		if (i == -1)
			return QString("0");	//全是0，那就是0吧

		if (str[i] == '.')
			i--;

		str = str.left(i + 1);

		return str;
	}

	u256 from_display(QString val) const
	{
		int i, dot = -1;
		for (i = 0; i < val.length(); i++)
		{
			if (val[i] == '.')
				dot = i;
			else if (((val[i] < '0') || (val[i] > '9')))
				break;
		}
		if (i != val.length())
			val = val.left(i);

		QString lstr, rstr;
		if (dot != -1)
		{
			lstr = val.left(dot);
			rstr = val.right(val.length() - dot - 1);
		}
		else
		{
			lstr = val;
			rstr = QString("");
		}

		QChar ch;
		for (i = 0; i < precision; i++)
		{
			if (i < rstr.length())
				ch = rstr[i];
			else
				ch = '0';
			lstr.append(ch);
		}

		for (i = 0; i < lstr.length(); i++)
		{
			//找开始第一个不是0的地方
			if (lstr[i] != '0')
				break;
		}
		val = lstr.right(lstr.length() - i);

		u256 value(val.toStdString());
		return value;
	}
};

extern map<QString, Coin> gCoin;
extern Coin _COIN_UNKNOW;

constexpr CoinType CT_UNKNOW(CoinTypeMajor::UNKNOW, 0, 0);
constexpr CoinType CT_BTC(CoinTypeMajor::BTC, 0, 0);
constexpr CoinType CT_BTC_TEST(CoinTypeMajor::BTC, 0, 1);
constexpr CoinType CT_USDT(CoinTypeMajor::BTC, CoinTypeMinor::BTC_USDT, 0);
constexpr CoinType CT_USDT_TEST(CoinTypeMajor::BTC, CoinTypeMinor::BTC_USDT, 1);
constexpr CoinType CT_ETH(CoinTypeMajor::ETH, CoinTypeMinor::ETH_ETH, 1);
constexpr CoinType CT_ETH_TEST3(CoinTypeMajor::ETH, CoinTypeMinor::ETH_ETH, 3);
constexpr CoinType CT_ETH_AE(CoinTypeMajor::ETH, CoinTypeMinor::ETH_AE, 1);
constexpr CoinType CT_ETH_BAT(CoinTypeMajor::ETH, CoinTypeMinor::ETH_BAT, 1);
constexpr CoinType CT_ETH_BTM(CoinTypeMajor::ETH, CoinTypeMinor::ETH_BTM, 1);
constexpr CoinType CT_ETH_HT(CoinTypeMajor::ETH, CoinTypeMinor::ETH_HT, 1);
constexpr CoinType CT_ETH_OMG(CoinTypeMajor::ETH, CoinTypeMinor::ETH_OMG, 1);
constexpr CoinType CT_ETH_NAS(CoinTypeMajor::ETH, CoinTypeMinor::ETH_NAS, 1);
constexpr CoinType CT_ETH_ELF(CoinTypeMajor::ETH, CoinTypeMinor::ETH_ELF, 1);
constexpr CoinType CT_ETH_ZIL(CoinTypeMajor::ETH, CoinTypeMinor::ETH_ZIL, 1);
constexpr CoinType CT_ETH_BLZ(CoinTypeMajor::ETH, CoinTypeMinor::ETH_BLZ, 1);
constexpr CoinType CT_ETH_VEN(CoinTypeMajor::ETH, CoinTypeMinor::ETH_VEN, 1);
constexpr CoinType CT_ETH_THETA(CoinTypeMajor::ETH, CoinTypeMinor::ETH_THETA, 1);
constexpr CoinType CT_ETH_ZRX(CoinTypeMajor::ETH, CoinTypeMinor::ETH_ZRX, 1);
constexpr CoinType CT_ETH_BNB(CoinTypeMajor::ETH, CoinTypeMinor::ETH_BNB, 1);
constexpr CoinType CT_ETH_ICX(CoinTypeMajor::ETH, CoinTypeMinor::ETH_ICX, 1);
constexpr CoinType CT_ETH_WAX(CoinTypeMajor::ETH, CoinTypeMinor::ETH_WAX, 1);
constexpr CoinType CT_ETH_POLY(CoinTypeMajor::ETH, CoinTypeMinor::ETH_POLY, 1);
constexpr CoinType CT_ETH_LBA(CoinTypeMajor::ETH, CoinTypeMinor::ETH_LBA, 1);
constexpr CoinType CT_ETH_KNC(CoinTypeMajor::ETH, CoinTypeMinor::ETH_KNC, 1);
constexpr CoinType CT_ETH_RUFF(CoinTypeMajor::ETH, CoinTypeMinor::ETH_RUFF, 1);
constexpr CoinType CT_ETH_CMT(CoinTypeMajor::ETH, CoinTypeMinor::ETH_CMT, 1);
constexpr CoinType CT_ETH_WTC(CoinTypeMajor::ETH, CoinTypeMinor::ETH_WTC, 1);
constexpr CoinType CT_ETH_QSP(CoinTypeMajor::ETH, CoinTypeMinor::ETH_QSP, 1);
constexpr CoinType CT_ETH_SNT(CoinTypeMajor::ETH, CoinTypeMinor::ETH_SNT, 1);
constexpr CoinType CT_ETH_RRC(CoinTypeMajor::ETH, CoinTypeMinor::ETH_RRC, 1);
constexpr CoinType CT_ETH_GUSD(CoinTypeMajor::ETH, CoinTypeMinor::ETH_GUSD, 1);
constexpr CoinType CT_ETH_PLY(CoinTypeMajor::ETH, CoinTypeMinor::ETH_PLY, 1);
constexpr CoinType CT_ETH_LRC(CoinTypeMajor::ETH, CoinTypeMinor::ETH_LRC, 1);
constexpr CoinType CT_ETH_POWR(CoinTypeMajor::ETH, CoinTypeMinor::ETH_POWR, 1);
constexpr CoinType CT_ETH_RNT(CoinTypeMajor::ETH, CoinTypeMinor::ETH_RNT, 1);
constexpr CoinType CT_ETH_STORM(CoinTypeMajor::ETH, CoinTypeMinor::ETH_STORM, 1);
constexpr CoinType CT_ETH_OST(CoinTypeMajor::ETH, CoinTypeMinor::ETH_OST, 1);
constexpr CoinType CT_ETH_DELC(CoinTypeMajor::ETH, CoinTypeMinor::ETH_DELC, 1);
constexpr CoinType CT_ETH_GGC(CoinTypeMajor::ETH, CoinTypeMinor::ETH_GGC, 1);
constexpr CoinType CT_ETH_GGT(CoinTypeMajor::ETH, CoinTypeMinor::ETH_GGT, 1);
constexpr CoinType CT_EOS(CoinTypeMajor::EOS, CoinTypeMinor::EOS_EOS, 0);
constexpr CoinType CT_EOS_TEST(CoinTypeMajor::EOS, CoinTypeMinor::EOS_EOS, 1);
constexpr CoinType CT_BHP(CoinTypeMajor::BHP, CoinTypeMinor::BHP_BHP, 0);
constexpr CoinType CT_BHP_TEST(CoinTypeMajor::BHP, CoinTypeMinor::BHP_BHP, 1);
constexpr CoinType CT_TRX(CoinTypeMajor::TRX, CoinTypeMinor::TRX_TRX, 0);
constexpr CoinType CT_TRX_TEST(CoinTypeMajor::TRX, CoinTypeMinor::TRX_TRX, 1);

Coin& GetCoinByType(const CoinType& type);
QString GetCoinIDByType(const CoinType& type);