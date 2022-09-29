#include "BtxonAPI.h"

#include "BWallet.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

int BtxonAPI::parseResult(const QByteArray& result, QJsonDocument& doc, QJsonObject& data)
{
	int code = -1;
    QJsonParseError jsonError;
    doc = QJsonDocument::fromJson(result, &jsonError);

    if (jsonError.error != QJsonParseError::NoError)
        return -1;

    if (!doc.isObject())
        return -1;

    QJsonObject json = doc.object();

    if (json.contains(("error")))
	{
        QJsonValue val = json["error"];
		if (!val.isObject())
			return code;

        QJsonObject err = val.toObject();
		if (err.contains("message"))
			error_msg = err["message"].toString();

        if (err.contains("code"))
			code = err["code"].toInt();

        return code;
    }

    if (!json.contains("code"))
        return -1;

	code = json["code"].toInt();

    if (!json.contains("data"))
        return -1;

    QJsonValue val = json["data"];
    if (!val.isObject())
        return -1;

    data = val.toObject();

    return code;
}

int BtxonAPI::parseArrayResult(const QByteArray& result, QJsonDocument& doc, QJsonArray& data)
{
    QJsonParseError jsonError;
    doc = QJsonDocument::fromJson(result, &jsonError);

    if (jsonError.error != QJsonParseError::NoError) {
        return -1;

    }
    if (!doc.isObject()) {
        return -1;
    }

    QJsonObject json = doc.object();

    if (json.contains(("error"))) {
        QJsonValue val = json["error"];
        if (!val.isObject()) {
            return -1;
        }
        QJsonObject err = val.toObject();
        if (err.contains("code")) {
            return err["code"].toInt();
        }
        return -1;
    }

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

int BtxonAPI::getEOSInfo(const CoinType& coinType, std::string& infoStr)
{
	error_msg.clear();
	QString coinID = GetCoinIDByType(coinType);
	if (coinID.isEmpty())
        return -1;

    QString baseUrl(BASE_URL);
    QString fullUrl = baseUrl.append(QString("/Wallet/getEOSInfo?coin_id=%1").arg(coinID));

    const QByteArray& result = get(fullUrl, 15000);

    QJsonDocument doc;
    QJsonObject jsonData;
    if (parseResult(result, doc, jsonData) == 200) {
        infoStr = QString(QJsonDocument(jsonData).toJson(QJsonDocument::Compact)).toStdString();
        return 0;
    }

    return -1;
}

int BtxonAPI::encodeEOSABI(const CoinType& coinType, const std::string &code, const std::string &action, const std::string &args, std::string& binArg)
{
	error_msg.clear();
	QString coinID = GetCoinIDByType(coinType);
	if (coinID.isEmpty()) {
		return -1;
	}

	QString baseUrl(BASE_URL);
	QString fullUrl = baseUrl.append("/Wallet/eos-encode-abi");

	QJsonObject obj;
	obj.insert("coin_id", coinID);
	obj.insert("code", code.c_str());
	obj.insert("action", action.c_str());
	obj.insert("args", args.c_str());

	QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact);
	const QByteArray& result = post(fullUrl, data, 30000);

	QJsonDocument doc;
	QJsonObject jsonData;
	if (parseResult(result, doc, jsonData) == 200)
 {
		if (jsonData.contains("encodeABI")) {
			binArg = jsonData.value("encodeABI").toString().toStdString();
			return 0;
		}
	}

	return -1;
}

int BtxonAPI::getEOSAccount(const CoinType & coinType, const std::string & account, QJsonValue& accountInfo)
{
	error_msg.clear();
	QString coinID = GetCoinIDByType(coinType);
	if (coinID.isEmpty()) {
        return -1;
    }

    QString baseUrl(BASE_URL);

    QString fullUrl = baseUrl.append(QString("/Wallet/getEOSAccount?coin_id=%1&account=%2").arg(coinID).arg(account.c_str()));

    const QByteArray& result = get(fullUrl, 15000);

    QJsonDocument doc;
    QJsonObject jsonData;
    if (parseResult(result, doc, jsonData) == 200) {
        if (jsonData.contains("account")) {
            accountInfo = jsonData.value("account");
            return 0;
        }
    }

    return -1;
}

int BtxonAPI::getKeyAccounts(const CoinType& coinType, const std::string &pubKey, vector<std::string>& account_list)
{
	error_msg.clear();
    QString baseUrl(BASE_URL);
    QString fullUrl = baseUrl.append(QString("/Wallet/getKeyAccounts?public_key=%1&is_test=%2").arg(pubKey.c_str()).arg(coinType.chain_id));

    const QByteArray& result = get(fullUrl, 15000);
    QJsonDocument doc;
    QJsonObject jsonData;
    account_list.clear();
    if (parseResult(result, doc, jsonData) == 200) {
        if (jsonData.contains("account_names")) {
            QJsonArray jsonAry = jsonData.value("account_names").toArray();
            for (int i = 0; i < jsonAry.size(); i++) {
                account_list.push_back(jsonAry.at(i).toString().toStdString());
            }
            return 0;
        }
    }

    return -1;
}

// 获取账户余额
int BtxonAPI::fetchBalance(const CoinType& coinType, const std::string &wallet_address, u256& balance, u256& froze)
{
	error_msg.clear();
	QString coinID = GetCoinIDByType(coinType);
	if (coinID.isEmpty()) {
		return -1;
	}

	QString baseUrl(BASE_URL);
	QString fullUrl = baseUrl.append(QString("/Wallet/getBalance?coin_id=%1&address=%2").arg(coinID).arg(wallet_address.c_str()));

	const QByteArray& result = get(fullUrl, 15000);

    QJsonDocument doc;
    QJsonObject jsonData;
    if (parseResult(result, doc, jsonData) == 200) {
        if (jsonData.contains("balance") && jsonData.contains("froze_amount")) {

            QJsonValue value = jsonData.value("balance");
            QString valueString = value.toString();
			balance = valueString.toStdString();

            value = jsonData.value("froze_amount");
            valueString = value.toString();
			froze = valueString.toStdString();

			if (froze >= balance)
				balance = 0;
			else
				balance -= froze; //假定balance包含冻结部分

            return 0;
		}
	}
	return -1;
}

// 新版本的取钱包账户余额的接口
int BtxonAPI::fetchBalanceV2(const CoinType& coinType, const std::string &wallet_address, bool is_force_refresh, u256& balance, u256& froze)
{
	error_msg.clear();
	QString coinID = GetCoinIDByType(coinType);
	if (coinID.isEmpty()) {
		return -1;
	}

	QString baseUrl(BASE_URL);
	QString fullUrl = baseUrl.append(QString("/Wallet/getBalanceV2?coin_id=%1&address=%2&is_force_refresh=%3").arg(coinID).arg(wallet_address.c_str()).arg(is_force_refresh?1:0));

	const QByteArray& result = get(fullUrl, 15000);

	QJsonDocument doc;
	QJsonObject jsonData;
	if (parseResult(result, doc, jsonData) == 200) {
		if (jsonData.contains("balance") && jsonData.contains("froze_amount")) {

			QJsonValue value = jsonData.value("balance");
			QString valueString = value.toString();
			balance = valueString.toStdString();

			value = jsonData.value("froze_amount");
			valueString = value.toString();
			froze = valueString.toStdString();

			if (froze >= balance)
				balance = 0;
			else
				balance -= froze; //假定balance包含冻结部分

			return 0;
		}
	}
	return -1;
}

int BtxonAPI::getNonce(const CoinType& coinType, const std::string &wallet_address, u256& count)
{
	error_msg.clear();
	if (coinType.major != CoinTypeMajor::ETH) {
		return -1;
	}

	QString coin_id = coinType.is_testnet() ? "tETH" : "ETH";

	QString baseUrl(BASE_URL);
	QString fullUrl = baseUrl.append(QString("/Wallet/getTransactionCount?coin_id=%1&address=%2").arg(coin_id).arg(wallet_address.c_str()));

	const QByteArray& result = get(fullUrl, 15000);

    QJsonDocument doc;
    QJsonObject jsonData;
    if (parseResult(result, doc, jsonData) == 200) {
        if (jsonData.contains("nonce")){
            QJsonValue value = jsonData["nonce"];
            count = value.toInt();
            return 0;
        }
	}

	return -1;
}

int BtxonAPI::getUTXO(const CoinType& coinType, const std::string &wallet_address, vector<Utxo>& utxo_list)
{
	error_msg.clear();
	QString coinID = GetCoinIDByType(coinType);
	if (coinID.isEmpty()) {
		return -1;
	}

	QString baseUrl(BASE_URL);
	QString fullUrl = baseUrl.append(QString("/Wallet/getUtxo?coin_id=%1&address=%2").arg(coinID).arg(wallet_address.c_str()));

	const QByteArray& result = get(fullUrl, 15000);
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(result, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
		return -1;

	QJsonObject root = jsonDoc.object();
	int code = root.value("code").toInt();
	if (code != 200)
		return code;

	QString message = QString::fromUtf8(root.value("message").toString().toStdString().c_str());
	QJsonObject data = root.value("data").toObject();
	QJsonArray jsonAry = data.value("utxos").toArray();

	if (coinType.major == BTC)
	{
		for (int i = 0; i < jsonAry.size(); i++)
		{
			Utxo ut;
			QJsonObject obj = jsonAry.at(i).toObject();
			ut.hash = obj.value("txid").toString().toStdString();
			ut.index = obj.value("vout").toInt();
			ut.value = obj.value("satoshis").toVariant().toULongLong();
			QString scriptHex = obj.value("scriptPubKey").toString();

			ut.script = BTCAPI::decode_script(scriptHex.toStdString());
			utxo_list.push_back(ut);
		}
	}
	else if (coinType.major == BHP)
	{
		for (int i = 0; i < jsonAry.size(); i++)
		{
			Utxo ut;
			QJsonObject obj = jsonAry.at(i).toObject();
			QString txid = obj.value("txid").toString();
			if (txid.startsWith("0x", Qt::CaseInsensitive))
				ut.hash = txid.mid(2).toStdString();
			else
				ut.hash = txid.toStdString();
			ut.index = obj.value("n").toInt();
			double dvalue = obj.value("value").toVariant().toString().toDouble();
			for (int j = 0; j < gCoin[coinID].precision; j++)
			{
				dvalue *= 10;
			}

			ut.value = (uint64_t)(dvalue+0.00000002);
			utxo_list.push_back(ut);
		}
	}

    return 0;
}

int BtxonAPI::getScript(const std::string &txid, std::string& script)
{
	error_msg.clear();
	QString baseUrl(BASE_URL);
	QString fullUrl = baseUrl.append(QString("/Wallet/getScript?tx_id=%1").arg(txid.c_str()));

	const QByteArray& result = get(fullUrl, 15000);
	QJsonParseError jsonError;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(result, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
		return -1;

	QJsonObject root = jsonDoc.object();
	int code = root.value("code").toInt();
	if (code != 200)
		return code;

	QString message = QString::fromUtf8(root.value("message").toString().toStdString().c_str());
	QJsonObject data = root.value("data").toObject();
	QJsonObject scriptObj = data.value("script").toObject();
	QString script_hex = scriptObj.value("hex").toString();

	script = BTCAPI::decode_script(script_hex.toStdString());
	return 0;
}

int BtxonAPI::getTxList(const CoinType& coinType, const std::string &wallet_address, vector<TransactionNet>& transaction_list)
{
	error_msg.clear();
	QString coinID = GetCoinIDByType(coinType);
	if (coinID.isEmpty()) {
		return -1;
	}

	QString baseUrl(BASE_URL);
	int offset = 0;
	int limit = 30;
	QString fullUrl = baseUrl.append(QString("/Wallet/getTxList?coin_id=%1&address=%2&offset=%3&limit=%4").arg(coinID).arg(wallet_address.c_str()).arg(offset).arg(limit));

	const QByteArray& result = get(fullUrl, 15000);
	QJsonParseError jsonError;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(result, &jsonError);
	if (jsonError.error != QJsonParseError::NoError)
		return -1;

	QJsonObject root = jsonDoc.object();
	int code = root.value("code").toInt();
	if (code != 200)
		return code;

	QString message = QString::fromUtf8(root.value("message").toString().toStdString().c_str());
	QJsonObject data = root.value("data").toObject();
	QJsonArray jsonAry = data.value("txs").toArray();

	for (int i = 0; i < jsonAry.size(); i++)
	{
		TransactionNet tn;
		QJsonObject obj = jsonAry.at(i).toObject();
		tn.hash = obj.value("transaction_hash").toString().toStdString();
		tn.from_address = obj.value("from_address").toString().toStdString();
		tn.to_address = obj.value("to_address").toString().toStdString();
		tn.value = u256(obj.value("value").toString().toStdString());
		tn.fee = u256(obj.value("fee").toString().toStdString());
		tn.script = obj.value("script_pub_key").toString().toStdString();

		QVariant spent_index = obj.value("spent_index").toVariant();
		if (spent_index.isNull())
			tn.spent_index = -1;
		else
			tn.spent_index = spent_index.toInt();

		tn.status = obj.value("status").toInt();
		tn.created = obj.value("created_at").toInt();

		transaction_list.push_back(tn);
	}

	return 0;
}

int BtxonAPI::fetchFee(const CoinType& coinType, FeeInfo& feeinfo)
{
	error_msg.clear();
	QString coinID = GetCoinIDByType(coinType);
	if (coinID.isEmpty()) {
		return -1;
	}

	QString baseUrl(BASE_URL);
	QString fullUrl = baseUrl.append(QString("/Wallet/getFee?coin_id=%1").arg(coinID));

	const QByteArray& result = get(fullUrl, 15000);

    QJsonDocument doc;
    QJsonObject jsonData;
    if (parseResult(result, doc, jsonData) == 200) {

        if (coinType.major ==  CoinTypeMajor::ETH)
		{
            feeinfo.minFee = jsonData["low_gas_price"].toString().toULongLong();
            feeinfo.midFee = jsonData["medium_gas_price"].toString().toULongLong();
            feeinfo.maxFee = jsonData["high_gas_price"].toString().toULongLong();
            feeinfo.param = jsonData["gas_limit"].toString().toULongLong();

			if (feeinfo.minFee <= 0 || feeinfo.midFee <= 0 || feeinfo.maxFee <= 0 || feeinfo.param <= 0) {
				return -1;
			}
        }
		else if (coinType.major == CoinTypeMajor::BHP)
		{
			feeinfo.minFee = jsonData["fee"].toString().toULongLong();
			feeinfo.midFee = feeinfo.minFee;
			feeinfo.maxFee = feeinfo.minFee;
			feeinfo.param = 0;
		}
		else
		{
            feeinfo.minFee = jsonData["hourFee"].toString().toULongLong();
            feeinfo.midFee = jsonData["halfHourFee"].toString().toULongLong();
            feeinfo.maxFee = jsonData["fastestFee"].toString().toULongLong();
            feeinfo.param = 0;

			if (feeinfo.minFee <= 0 || feeinfo.midFee <= 0 || feeinfo.maxFee <= 0) {
				return -1;
			}
		}
        return 0;
	}

	return -1;
}

int BtxonAPI::sendSignedTransaction(const CoinType& coinType, const string &from, const string &to, const u256 nonce, const string &hash, const u256 &pay, const u256 &fee, const std::string &action, const std::string &signedTx, u256 &balance, u256& froze)
{
	error_msg.clear();
	QString coinID = GetCoinIDByType(coinType);
	if (coinID.isEmpty())
		return -1;

	QString baseUrl(BASE_URL);
	QString fullUrl = baseUrl.append("/Wallet/sendSignedTransaction");

	QJsonObject obj;
	obj.insert("coin_id", coinID);
    obj.insert("action", action.c_str());
    obj.insert("transaction_hash", hash.c_str());
    obj.insert("from_address", from.c_str());
    obj.insert("to_address", to.c_str());
	obj.insert("nonce", nonce.str().c_str());
    obj.insert("value", pay.str().c_str());
    obj.insert("fee", fee.str().c_str());
    obj.insert("data", signedTx.c_str());

	QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact);

    const QByteArray& result = post(fullUrl, data, 30000);

    QJsonDocument doc;
    QJsonObject jsonData;
    const int code = parseResult(result, doc, jsonData);
    if (code == 200)
	{
        if (jsonData.contains("balance") && jsonData.contains("froze_amount"))
		{
            QJsonValue value = jsonData.value("balance");
            QString valueString = value.toString();
			balance = valueString.toStdString();

            value = jsonData.value("froze_amount");
            valueString = value.toString();
			froze = valueString.toStdString();

			if (froze >= balance)
				balance = 0;
			else
				balance -= froze; //假定balance包含冻结部分

            return 0;
        }
    }

	return code;
}

int BtxonAPI::getLastBlockNumber(const CoinType& coinType, u256& bn)
{
	error_msg.clear();
	QString coinID = GetCoinIDByType(coinType);
	if (coinID.isEmpty()) {
		return -1;
	}

	QString baseUrl(BASE_URL);
	QString fullUrl = baseUrl.append(QString("/Wallet/getLastBlockNumber?coin_id=%1").arg(coinID));

	const QByteArray& result = get(fullUrl, 15000);

    QJsonDocument doc;
    QJsonObject jsonData;
    if (parseResult(result, doc, jsonData) == 200) {
        if (jsonData.contains("lastBlockNumber")) {
            bn = jsonData["lastBlockNumber"].toInt();
            return 0;
        }
    }

    return -1;
}

static double usdt_cny = -1;
static double usdt_usd = -1;
static uint64_t update_time = 0;

static std::map<CoinType, double> price_cache;

// 取汇率
int BtxonAPI::getPrice(const CoinType& type, CurrencyType currencyType, double &price)
{
	if (updateHTicker() == 0) {
		price = 0;

		auto it = price_cache.find(type);
		if (it != price_cache.end())
		{
			price = it->second;
			if (currencyType == CurrencyType::CNY)
				price *= usdt_cny;
			if (currencyType == CurrencyType::USD)
				price *= usdt_usd;
		}
		return 0;
	}

	return -1;
}

int BtxonAPI::updateHTicker()
{
	QDateTime time = QDateTime::currentDateTime();
	uint64_t now = time.toSecsSinceEpoch();
	if (now - update_time > 1 * 60 * 60) 
	{
		mutex.lock();
		QString baseUrl(BASE_URL);
		QString fullUrl = baseUrl.append("/open/market");

		const QByteArray& result = get(fullUrl, 15000);
		QJsonParseError jsonError;
		QJsonDocument json = QJsonDocument::fromJson(result, &jsonError);
		if (jsonError.error == QJsonParseError::NoError)
		{
			if (json.isObject())
			{
				QJsonObject rootObj = json.object();

				if (rootObj["status"] != "ok")
				{
					mutex.unlock();
					return -1;
				}
				
				usdt_cny = rootObj["usdt_cny"].toString().toDouble();
				usdt_usd = rootObj["usdt_usd"].toString().toDouble();
				price_cache.clear();

                QJsonObject exchange = rootObj["exchange"].toObject();

				const char* change_base[] = { "_usdt", "_eth", "_btc" };
				double base_rate[] = { 1.0 , 0.0, 0.0 };

                for (size_t baseIdx = 0; baseIdx < sizeof(change_base)/sizeof(*change_base); baseIdx++)
				{
					const char *curBase = change_base[baseIdx];

                    for(auto it = exchange.begin(); it != exchange.end(); ++it)
					{
                        QJsonObject exchangeData = it.value().toObject();
							
                        for (auto &coinPair : gCoin)
						{
                            QString pair_key = coinPair.second.symbol.toLower().append(curBase);
                            if (exchangeData.contains(pair_key))
							{
                                QJsonObject pair = exchangeData[pair_key].toObject();
                                double price = pair["btxon"].toDouble();
                                const CoinType& coinType = coinPair.second.type;
                                price_cache.emplace(coinType, price * base_rate[baseIdx]);
                                if (baseIdx == 0)
								{ //usdt 轮需要更新eth&btc对比价
                                    if (coinType.major == CoinTypeMajor::ETH && coinType.minor == 0)
                                        base_rate[1] = price;
									else if (coinType.major == CoinTypeMajor::BTC && coinType.minor == 0)
                                        base_rate[2] = price;
                                }
                            }
                        }
					}
				}

				if (price_cache.find(CT_USDT) == price_cache.end())
				{
					price_cache.emplace(CT_USDT, 1);
				}

				update_time = now;
				mutex.unlock();
				return 0;
			}
		}
		mutex.unlock();
		return -1;
	}

	return 0;
}

int BtxonAPI::checkActiveCard(const QString &clientID, bool &active, QString &code, QString &psw)
{
	error_msg.clear();
    QString baseUrl(BASE_URL);
    QString fullUrl = baseUrl.append(QString("/ActiveCard?clientId=%1").arg(clientID));

    const QByteArray& result = get(fullUrl, 15000);

    QJsonDocument doc;
    QJsonObject jsonData;
    if (parseResult(result, doc, jsonData) == 200) {
        active = jsonData["isActive"].toBool();
        code = jsonData["code"].toString();
        psw = jsonData["password"].toString();
        return 0;
    }

    return -1;
}

int BtxonAPI::activeCard(const QString &clientID, const QString& sn, const QString &code, const QString &psw, QString& message)
{
	error_msg.clear();
    QString baseUrl(BASE_URL);
    QString fullUrl = baseUrl.append("/ActiveCard/active");

    QJsonObject obj;
    obj.insert("clientId", clientID);
    obj.insert("code", code.toUpper());
	obj.insert("sn", sn.toUpper());
    obj.insert("password", psw.toUpper());

    QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact);

    const QByteArray& result = post(fullUrl, data, 30000);

    QJsonDocument doc;
    QJsonObject jsonData;

    const int status = parseResult(result, doc, jsonData);
    if (status < 0) {
        message = trUtf8("网络错误");
        return status;
    }

    if (status == 200) {
        message = trUtf8("绑定成功");
        return 0;
    }
    else {
        message = jsonData["message"].toString();
    }
    return status;
}

int BtxonAPI::getCentralBalance(const QString &clientID, const int kind, CentralBalance& balance)
{
	error_msg.clear();
    QString baseUrl(BASE_URL);
    QString fullUrl = baseUrl.append(QString("/Client/getTokenAmount?clientId=%1&kind=%2").arg(clientID).arg(kind));

    balance.cny = "0.0";
    balance.usd = "0.0";
    balance.value = 0;
    balance.upDown = 0.0;

    const QByteArray& result = get(fullUrl, 15000);

    QJsonDocument doc;
    QJsonArray jsonData;
    if (parseArrayResult(result, doc, jsonData) == 200) {
        if (jsonData.size() > 0) {
            QJsonObject item = jsonData[0].toObject();
            balance.cny = item["cny"].toString();
            balance.usd = item["usd"].toString();
			balance.value = item["value"].toString().toStdString();
            balance.upDown = item["upDown"].toString();
        }
        return 0;
    }

    return -1;
}

int BtxonAPI::getUnionCount(const QString& clientID, int& softCount, int& hardCount)
{
	error_msg.clear();
	QString baseUrl(BASE_URL);
	QString fullUrl = baseUrl.append(QString("/RelationUnions/getUnionCount?clientId=%1").arg(clientID));

	const QByteArray& result = get(fullUrl, 15000);

	QJsonDocument doc;
	QJsonObject jsonData;
	if (parseResult(result, doc, jsonData) == 200)
	{
		if (jsonData.contains("softCount"))
		{
			QJsonValue value = jsonData["softCount"];
			softCount = value.toInt();
		}
		if (jsonData.contains("hardCount"))
		{
			QJsonValue value = jsonData["hardCount"];
			hardCount = value.toInt();
		}
		return 0;
	}

	return -1;
}
/*
int BtxonAPI::Unbind(const QString& clientID)
{
	QString baseUrl(BASE_URL);
	QString fullUrl = baseUrl.append("/RelationUnions/createUnion");

	QJsonArray keys;

	QJsonObject softClient;
	softClient.insert("clientid", QJsonValue::Null);
	keys.append(softClient);

	QJsonObject hardClient;
	hardClient.insert("clientid", clientID);
	hardClient.insert("free", "1");
	keys.append(hardClient);

	QJsonObject obj;
	obj.insert("action", "free");
	obj.insert("keys", keys);

	QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact);
	const QByteArray& result = post(fullUrl, data, 30000);

	QJsonDocument doc;
	QJsonObject jsonData;
	if (parseResult(result, doc, jsonData) == 200)
		return 0;

	return -1;
}
*/

int BtxonAPI::Unbind(const QString& clientID)
{
	error_msg.clear();
	QString baseUrl(BASE_URL);
	QString fullUrl = baseUrl.append("/RelationUnions/freeClientInUnion");

	QJsonObject obj;
	obj.insert("clientId", clientID);

	QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact);
	const QByteArray& result = post(fullUrl, data, 30000);

	QJsonDocument doc;
	QJsonObject jsonData;
	int retcode = parseResult(result, doc, jsonData);
	if (retcode == 200)
		return 0;

	return retcode;
}
