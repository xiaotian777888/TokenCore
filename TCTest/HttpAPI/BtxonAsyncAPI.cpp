#include "BtxonAsyncAPI.h"

#include "BWallet.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

using namespace std;

int BtxonAsyncAPI::parseArrayResult(const QByteArray &result, QJsonDocument& doc, QJsonArray& data) {
    QJsonParseError jsonError;
    doc = QJsonDocument::fromJson(result, &jsonError);

    if (jsonError.error != QJsonParseError::NoError) {
        return -1;

    }
    if (!doc.isObject()) {
        return -1;
    }

    QJsonObject json = doc.object();

    if (!json.contains("code")) {
        return -1;
    }

    if (!json.contains("data")) {
        return -1;
    }

    QJsonValue val = json["data"];
    if (!val.isArray()) {
        return -1;
    }

    data = val.toArray();

    const int code = json["code"].toInt();

    return code;
}

int BtxonAsyncAPI::parseResult(const QByteArray& result, QJsonDocument& doc, QJsonObject& data) {

    QJsonParseError jsonError;
    doc = QJsonDocument::fromJson(result, &jsonError);

    if (jsonError.error != QJsonParseError::NoError) {
        return -1;

    }
    if (!doc.isObject()) {
        return -1;
    }

    QJsonObject json = doc.object();

    if (!json.contains("code")) {
        return -1;
    }

    if (!json.contains("data")) {
        return -1;
    }

    QJsonValue val = json["data"];
    if (!val.isObject()) {
        return -1;
    }

    data = val.toObject();

    const int code = json["code"].toInt();

    return code;
}

void BtxonAsyncAPI::getConfig(const QString& url, const function<void(bool, const QJsonDocument*)> &callback) {
    this->callbackFun = [callback] (bool success, const QByteArray result) {
        if (success) {
            QJsonParseError jsonError;
            QJsonDocument json = QJsonDocument::fromJson(result, &jsonError);
            if (jsonError.error == QJsonParseError::NoError) {
                return callback(true, &json);
            }
        }

        return callback(false, NULL);
    };

    get(url);
}

void BtxonAsyncAPI::queryBalance(const vector<BWallet>& wallet_list, const function<void(bool)> &callback)
{
    this->callbackFun = [callback, &wallet_list] (bool success, const QByteArray result) {
        if (success) {
            QJsonDocument doc;
            QJsonObject jsonData;
            if (parseResult(result, doc, jsonData) == 200) {

                if (!jsonData.contains("balance")) {
                    return callback(false);
                }

                QJsonArray arrayObj = jsonData["balance"].toArray();
                int size = arrayObj.size();
                for (int i = 0; i < size; i++) {
                    QJsonValue item = arrayObj.at(i);
                    QString coin_id = item["coin_id"].toString();
                    auto it = gCoin.find(coin_id);
                    if (it != gCoin.end()) {
                        for(auto wallet : wallet_list) {
                            if (wallet.coinType() == it->second.type) {
								u256 value = item["balance"].toString().toStdString();
								u256 froze = item["froze_amount"].toString().toStdString();

								if (froze >= value) {
									value = 0;
								} else {
									value -= froze; //假定balance包含冻结部分
								}

                                wallet.setBalance(value, froze);
                            }
                        }
                    }
                }
                return callback(true);
            }
        }
        return callback(false);
    };

    QString baseUrl(BASE_URL);

    QJsonObject keys;
    for (size_t i = 0; i < wallet_list.size(); i++)
    {
        const BWallet& wallet = wallet_list[i];
		QString coinID = GetCoinIDByType(wallet.coinType());
		if (!coinID.isEmpty()) {
            keys.insert(coinID, wallet.wallet_address);
        }
    }

    QByteArray data = QJsonDocument(keys).toJson(QJsonDocument::Compact);

    QString fullUrl = baseUrl.append("/Wallet/getBalanceOnBatch");

    post(fullUrl, data);
}

void BtxonAsyncAPI::createClient(const std::string &clientID, const std::string &pubkey, const vector<BWallet>& wallet_list, const function<void(bool)> &callback)
{
	this->callbackFun = [callback] (bool success, const QByteArray result) {
		return callback(success);
	};

	QString baseUrl(BASE_URL);
    QString fullUrl = baseUrl.append("/Relation/createClient");

	QJsonObject obj;
	obj.insert("clientid", clientID.c_str());

	QJsonArray keys;

    if (!pubkey.empty()) {
        QJsonObject coin;
        coin.insert("coinid", "BTX-OOOO");
        coin.insert("addr", pubkey.c_str());
        keys.append(coin);
    }

    for (size_t i = 0; i < wallet_list.size(); i++)
	{
		QJsonObject coin;
		const BWallet& wallet = wallet_list[i];
		QString coinID = GetCoinIDByType(wallet.coinType());
		if (!coinID.isEmpty()) {
			coin.insert("coinid", coinID);
			coin.insert("addr", wallet.wallet_address);
			keys.append(coin);
		}
	}
	obj.insert("keys", keys);

	QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact);

	post(fullUrl, data);
}


void BtxonAsyncAPI::queryTransactions(const CoinType& coinType, const std::string &wallet_address, int offset, const function<void(bool, TransactionData*, int)> &callback)
{
	QString coinID = GetCoinIDByType(coinType);
	if (coinID.isEmpty()) {
		return callback(false, nullptr, 0);
	}
	
	this->callbackFun = [coinType, callback, wallet_address](bool success, const QByteArray result) {
		if (success) {
            QJsonDocument doc;
            QJsonObject jsonData;
            if (parseResult(result, doc, jsonData) == 200) {

                if (!jsonData.contains("txs")) {
                    return callback(false, nullptr, 0);
                }

                QJsonArray arrayObj = jsonData["txs"].toArray();
                int size = arrayObj.size();
                TransactionData * datas = size > 0 ? new TransactionData[size] : nullptr;
                QString self_address(wallet_address.c_str());

                for (int i = 0; i < size; i++) {
                    TransactionData& data = datas[i];
                    QJsonValue item = arrayObj.at(i);
                    data.flag = item["status"].toInt();
                    data.hash = item["transaction_hash"].toString().toLower();
                    data.from = item["from_address"].toString();
                    data.to = item["to_address"].toString();

                    if (coinType.major == ETH) {
                        data.from = data.from.toLower();
                        data.to = data.to.toLower();
                        self_address = self_address.toLower();
                    }
					data.value = item["value"].toString().toStdString();

                    if (!(coinType.major == ETH && coinType.minor != ETH_ETH) && data.from == self_address) {
                        u256 fee(item["fee"].toString().toStdString());
                        data.value += fee;
                    }
                    data.time = QDateTime::fromMSecsSinceEpoch(item["created_at"].toDouble());
				}
                return callback(true, datas, size);
            }
		}
		return callback(false, nullptr, 0);
	};

	QString baseUrl(BASE_URL);

    //先按每页10条处理
    QString fullUrl = baseUrl.append(QString("/Wallet/getTxList?coin_id=%1&address=%2&offset=%3&limit=10").arg(coinID).arg(wallet_address.c_str()).arg(offset));
	get(fullUrl);
}

void BtxonAsyncAPI::checkMessages(const std::string &clientID, int mid, const function<void(bool, int)> &callback) {
    if (clientID.empty()) {
        return callback(false, 0);
    }
    this->callbackFun = [callback](bool success, const QByteArray result) {
        if (success) {
            QJsonDocument doc;
            QJsonObject jsonData;
            if (parseResult(result, doc, jsonData) == 200) {

                if (!jsonData.contains("count")) {
                    return callback(false, 0);
                }

                return callback(true, jsonData["count"].toInt());
            }
        }
        return callback(false, 0);
    };

    QString baseUrl(BASE_URL);

    QString fullUrl = baseUrl.append(QString("/Message/getMsgInfo?clientid=%1&mid=%2&msgfilter=0001").arg(clientID.c_str()).arg(mid));
    get(fullUrl);
}

void BtxonAsyncAPI::setMessagesRead(const std::string &clientID, const function<void(bool, int)> &callback) {
    if (clientID.empty()) {
        return callback(false, 0);
    }

    this->callbackFun = [callback](bool success, const QByteArray result) {
        if (success) {
            QJsonDocument doc;
            QJsonObject jsonData;
            if (parseResult(result, doc, jsonData) == 200) {
                return callback(true, 0);
            }
        }
        return callback(false, 0);
    };

    QString baseUrl(BASE_URL);

    QString fullUrl = baseUrl.append(QString("/Message/setMsgState"));
    post(fullUrl, nullptr);
}

void BtxonAsyncAPI::queryMessages(const std::string &clientID, int offset, int limit, const function<void(bool, MessageData*, int)> &callback) {
    //std::string clientID = "024c14e7d43b195898121227fb863f0394e3df185a995faab2ae06b165a405bc9d";
    if (clientID.empty()) {
        return callback(false, nullptr, 0);
    }

    this->callbackFun = [callback](bool success, const QByteArray result) {
		if (success) {
            QJsonDocument doc;
            QJsonArray arrayObj;
            if (parseArrayResult(result, doc, arrayObj) == 200) {
                int size = arrayObj.size();
                MessageData * datas = size > 0 ? new MessageData[size] : nullptr;

                for (int i = 0; i < size; i++) {
                    MessageData& data = datas[i];
                    QJsonValue item = arrayObj.at(i);

                    data.time = QDateTime::fromMSecsSinceEpoch(item["created_at"].toDouble());
                    data.mid = item["mid"].toInt();
                    QString msgt = item["msgt"].toString();
                    if (msgt != "json") {
                        data.flag = -1;
                        data.message = trUtf8("未知消息");
                        continue;
                    }

                    QJsonParseError jsonError;
                    QJsonDocument doc = QJsonDocument::fromJson(item["msgm"].toString().toLatin1(), &jsonError);

                    if (jsonError.error != QJsonParseError::NoError) {
                        data.flag = -1;
                        data.message = trUtf8("未知消息");
                        continue;
                    }

                    QJsonObject msg = doc.object();

                    QString valueStr = msg["value"].toString();
                    QString coin_id = msg["coin_id"].toString();
                    auto it = gCoin.find(coin_id);
                    if (it != gCoin.end()) {
                        const Coin& coin = it->second;
                        coin_id = coin.symbol;
                        u256 value = valueStr.toStdString();
                        valueStr = coin.to_display(value);
                        data.coinType = it->second.type;
                    }


                    data.flag = msg["flag"].toInt();
                    if (data.flag == 0) {
                        data.message = trUtf8("您转账的 %1%2已经汇出！")
                            .arg(valueStr).arg(coin_id);
                    } else if (data.flag == 1) {
                        data.message = trUtf8("您充值的 %1%2已经到账，请在账户内查看！")
                            .arg(valueStr).arg(coin_id);
                    } else if (data.flag == 2) {
                        data.message = trUtf8("您转账的 %1%2发送失败，该笔资金已解冻。")
                            .arg(valueStr).arg(coin_id);
                    } else if (data.flag == 3) {
                        data.message = trUtf8("此笔交易(%1%2)还未进入队列资产已经解冻，交易可能失败建议1天后再次查看。")
                            .arg(valueStr).arg(coin_id);
                    } else if (data.flag == 4) {
                        data.message = trUtf8("您的交易(%1%2)已经上链并处于排队等待中。")
                            .arg(valueStr).arg(coin_id);
                    } else {
                        data.message = trUtf8("未知消息");
                    }

                }

                return callback(true, datas, size);
			}
		}
		return callback(false, nullptr, 0);
	};

	QString baseUrl(BASE_URL);

    QString fullUrl = baseUrl.append(QString("/Message/getPrivateMsg?clientid=%1&offset=%2&limit=%3&msgfilter=0001").arg(clientID.c_str()).arg(offset).arg(limit));
	get(fullUrl);
}

void BtxonAsyncAPI::queryRewards(const std::string &clientID, int offset, int limit, const function<void(bool, RewardData*, int)> &callback) {
    //std::string clientID = "024c14e7d43b195898121227fb863f0394e3df185a995faab2ae06b165a405bc9d";
    if (clientID.empty()) {
        return callback(false, nullptr, 0);
    }

    this->callbackFun = [callback](bool success, const QByteArray result) {
        if (success) {
            QJsonDocument doc;
            QJsonArray arrayObj;
            if (parseArrayResult(result, doc, arrayObj) == 200) {
                int size = arrayObj.size();
                RewardData * datas = size > 0 ? new RewardData[size] : nullptr;

                for (int i = 0; i < size; i++) {
                    RewardData& data = datas[i];
                    QJsonValue item = arrayObj.at(i);

                    data.time = QDateTime::fromMSecsSinceEpoch(item["createdAt"].toDouble());
                    data.cny = item["cny"].toString();
                    data.usd = item["usd"].toString();
                    data.action = item["action"].toString();

                    QString valueStr = item["value"].toString();
					data.value = valueStr.toStdString();
                }

                return callback(true, datas, size);
            }
        }
        return callback(false, nullptr, 0);
    };

    QString baseUrl(BASE_URL);

    QString fullUrl = baseUrl.append(QString("/InoutHistory?clientId=%1&kind=0&offset=%2&count=%3").arg(clientID.c_str()).arg(offset).arg(limit));
    get(fullUrl);
}

void BtxonAsyncAPI::requestFinished(QNetworkReply *reply, const QByteArray data, const int statusCode)
{
	this->callbackFun(statusCode == 200, data);
}

