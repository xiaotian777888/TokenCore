#include "GateIOAsyncAPI.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <qvector.h>

#include "CoinApiAdapter.h"

using namespace std;

#define GATEIO_URL "https://data.gateio.io/api2/1"

static pair<CurrencyType, QString> CurrencyTypePairArray[] =
{
	make_pair(CurrencyType::CNY, "CNY"),
	make_pair(CurrencyType::USD, "USDT"),
};

static std::map<CurrencyType, QString> s_CurrencyTypeMap(CurrencyTypePairArray, CurrencyTypePairArray + sizeof(CurrencyTypePairArray) / sizeof(CurrencyTypePairArray[0]));

/*
[
1417564800000,	timestamp
384.47,		volume
387.13,		close
383.5,		high
387.13,		low
1062.04,	open
]
*/
static const int COLSE_IDX = 2;

double GateIOAsyncAPI::usdt_cny = -1;
uint64_t GateIOAsyncAPI::update_time = 0;

int GateIOAsyncAPI::get24HLine(const CoinType& coin_type, CurrencyType currency_type, const function<void(bool, double*, int)> &callback)
{
	checkUSDT_CNY([this, callback, coin_type](bool success)
	{
		QString baseUrl(GATEIO_URL);

		QString coinSymbol = GetCoinByType(coin_type).symbol;
		QString fullUrl;
		if (coin_type == CT_USDT)
			fullUrl = baseUrl.append(QString("/candlestick2/usdt_cnyx?group_sec=3600&range_hour=24"));
		else
			fullUrl = baseUrl.append(QString("/candlestick2/%1_USDT?group_sec=3600&range_hour=24").arg(coinSymbol));

		GateIOAsyncAPI* self = this;
		self->callbackFun = [callback, coin_type](bool success, const QByteArray result)
		{
			if (success)
			{
				QJsonParseError jsonError;
				QJsonDocument json = QJsonDocument::fromJson(result, &jsonError);
				if (jsonError.error == QJsonParseError::NoError)
				{
					if (json.isObject())
					{
						QJsonObject jsonObj = json.object();
						QJsonValue data = jsonObj["data"];
						if (data.isArray())
						{
							QJsonArray arrayObj = data.toArray();
							int size = arrayObj.size();
							double * datas = new double[size];

							for (int i = 0; i < size; i++)
							{
								QJsonValue item = arrayObj.at(i);
								if (coin_type == CT_USDT)
								{
									if (language == "zh-CN")
										datas[i] = item.toArray().at(COLSE_IDX).toString().toDouble();
									else
										datas[i] = item.toArray().at(COLSE_IDX).toString().toDouble() / usdt_cny;
								}
								else
								{
									if (language == "zh-CN")
									{
										/*
										double xxx = item.toArray().at(COLSE_IDX).toString().toDouble();
										xxx = xxx * usdt_cny;
										datas[i] = xxx;
										*/
										datas[i] = item.toArray().at(COLSE_IDX).toString().toDouble() * usdt_cny;
									}
									else
										datas[i] = item.toArray().at(COLSE_IDX).toString().toDouble();
								}
							}
							return callback(true, datas, size);
						}
					}
				}
			}
			return callback(false, nullptr, 0);
		};
		self->get(fullUrl);
	});

	return 0;
}

//https://data.gateio.io/api2/1/ticker/usdt_cny

void GateIOAsyncAPI::checkUSDT_CNY(const function<void(bool)> &callback)
{
    QDateTime time = QDateTime::currentDateTime();
	uint64_t now = time.toSecsSinceEpoch();
	if (now - update_time <= 10 * 60 * 60) {
		return callback(true);
	}

	this->callbackFun = [callback](bool success, const QByteArray result) {
		if (success) {
			QJsonParseError jsonError;
			QJsonDocument json = QJsonDocument::fromJson(result, &jsonError);
			if (jsonError.error == QJsonParseError::NoError) {
				if (json.isObject()) {
					QJsonObject rootObj = json.object();
					if (rootObj["result"] == "true") {
						//usdt_cny = rootObj["last"].toString().toDouble();
						usdt_cny = rootObj["last"].toDouble();
						QDateTime time = QDateTime::currentDateTime();
						update_time = time.toSecsSinceEpoch();
						return callback(true);
					}
				}
			}
		}
		return callback(false);
	};

	QString baseUrl(GATEIO_URL);
	//先取usdt价格，而后转成CNY
	QString fullUrl = baseUrl.append(QString("/ticker/usdt_cny"));

	get(fullUrl);
}


void GateIOAsyncAPI::requestFinished(QNetworkReply *reply, const QByteArray data, const int statusCode)
{
	this->callbackFun(statusCode == 200, data);
}
