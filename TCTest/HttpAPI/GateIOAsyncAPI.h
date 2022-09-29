#pragma once

#include "HttpAPI/AsyncHttpClient.h"
#include "qvector.h"
#include <functional>

using namespace std;

class GateIOAsyncAPI : public AsyncHttpClient
{
public:
	int get24HLine(const CoinType& coinType, CurrencyType currencyType, const function<void(bool, double*, int)> &callback);

protected:
	void requestFinished(QNetworkReply* reply, const QByteArray data, const int statusCode);

private:
	function<void(bool, const QByteArray)> callbackFun;

	void checkUSDT_CNY(const function<void(bool)> &callback); //检查更新USDT兑换CNY的比例

	static double usdt_cny; //USDT 兑换 CNY
	static uint64_t update_time; //汇率最后更新时间
};