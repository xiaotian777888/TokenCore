#pragma once

#define BTXON_DEV

#include "Coin.h"

#ifndef WIN32
    #ifndef strcpy_s
       #define strcpy_s(dest, size, src) strncpy((dest),(src),(size))
    #endif //strcpy_s
    #ifndef sprintf_s
       #define sprintf_s(str, size, fmt, ...) snprintf((str),(size),(fmt), __VA_ARGS__)
    #endif //sprintf_s
    #ifndef _strdup
       #define _strdup strdup
    #endif //_strdup
#endif //WIN32

#define MIN_FIRMWARE_VER_MAJOR 1
#define MIN_FIRMWARE_VER_MINOR 5

//现实货币类型
enum CurrencyType {
  CNY,
  USD,
};

//费率信息
//用3个值+1个扩展 能表示完整不？
class FeeInfo {
 public:
  FeeInfo() : maxFee(0), midFee(0), minFee(0), param(0) {}

  uint64_t maxFee;  //最大费率
  uint64_t midFee;  //中等费率
  uint64_t minFee;  //最小费率
  uint64_t param;   //附加参数 eth的gaslimit
};

// UI module 数据定义，以后得换个地方
struct TransactionData {
	QString hash;
	QString from;
	QString to;
	QDateTime time;
	int    flag;
	u256 value;
};

struct MessageData {
    int mid;
    CoinType coinType;
	QString message;
	QDateTime time;
	int    flag;
};

struct RewardData {
    u256 value;
    QString cny;
    QString usd;
    QString action;
    QDateTime time;
};

extern QString language; //http 请求时传递的本地语言，用于取回正确本地化的message

#ifdef BTXON_DEV
//#define BASE_URL "http://59.110.125.131:3001/api"		// 原测试服务器
//#define BASE_URL "http://47.91.153.248:3001/api"		// 香港测试服务器
#define BASE_URL "http://test.btxon.com/api"		// 香港测试服务器
//#define BASE_URL "http://192.168.15.61:3001/api"		// 孟晨的电脑
#else
#define BASE_URL "https://app.btxon.com/api"			// 正式服务器
#endif // BTXON_DEV

//QString baseUrl("https://app.btxon.com/hticker.json");
