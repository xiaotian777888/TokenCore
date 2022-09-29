#pragma once

#include "HttpAPI/AsyncHttpClient.h"
#include <functional>

class BWallet;

using namespace std;

class BtxonAsyncAPI : public AsyncHttpClient
{
    Q_OBJECT
public:
	void createClient(const std::string &clientID, const std::string &pubkey, const vector<BWallet>& wallet_list, const function<void(bool)> &callback);
    void getConfig(const QString& url, const function<void(bool, const QJsonDocument*)> &callback);

    void queryBalance(const vector<BWallet>& wallet_list, const function<void(bool)> &callback);

	void queryTransactions(const CoinType& coinType, const std::string &wallet_address, int offset, const function<void(bool, TransactionData*, int)> &callback);

    void queryMessages(const std::string &clientID, int offset, int limit, const function<void(bool, MessageData*, int)> &callback);
    void checkMessages(const std::string &clientID, int mid, const function<void(bool, int)> &callback);
    void setMessagesRead(const std::string &clientID, const function<void(bool, int)> &callback);

    void queryRewards(const std::string &clientID, int offset, int limit, const function<void(bool, RewardData*, int)> &callback);

protected:
	void requestFinished(QNetworkReply* reply, const QByteArray data, const int statusCode);

private:
    static int parseResult(const QByteArray &result, QJsonDocument& doc, QJsonObject& data);
    static int parseArrayResult(const QByteArray &result, QJsonDocument& doc, QJsonArray& data);

private:
	function<void(bool, const QByteArray)> callbackFun;    
};

