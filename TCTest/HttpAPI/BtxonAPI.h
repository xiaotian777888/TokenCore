#pragma once

#include <QMutex>
#include "HttpAPI/BaseHttpClient.h"
#include <functional>
#include <BtcApi.h>

class BWallet;

using namespace std;

struct TransactionNet
{
	string hash;
	string from_address;
	string to_address;
	u256 value;
	u256 fee;
	string script;
	int spent_index;
	int status;
	long created;
};

struct CentralBalance {
    u256 value;
    QString cny;
    QString usd;
    QString upDown;
};

class BtxonAPI : public BaseHttpClient
{
	Q_OBJECT

public:
	QString error_msg;

    int getKeyAccounts(const CoinType& coinType,const std::string &pubKey, vector<std::string>& account_list);
    int getEOSInfo(const CoinType& coinType, std::string& infoStr);
    int encodeEOSABI(const CoinType& coinType, const std::string &code, const std::string &action, const std::string &args, std::string& binArg);
    int getEOSAccount(const CoinType& coinType,const std::string& account, QJsonValue& accountInfo);
    int fetchBalance(const CoinType& coinType, const std::string &wallet_address, u256& balance, u256 &froze);
	int fetchBalanceV2(const CoinType& coinType, const std::string &wallet_address, bool is_force_refresh, u256& balance, u256& froze);
	int getNonce(const CoinType& coinType, const std::string &wallet_address, u256& count);
	int fetchFee(const CoinType& coinType, FeeInfo& feeinfo);
	int getLastBlockNumber(const CoinType& coinType , u256& bn);
	int getUTXO(const CoinType& coinType, const std::string &wallet_address, vector<Utxo>& utxo_list);
	int getScript(const std::string &txid, std::string& script);
	int getTxList(const CoinType& coinType, const std::string &wallet_address, vector<TransactionNet>& transaction_list);
    int sendSignedTransaction(const CoinType& type, const std::string &from, const std::string &to, const u256 nonce, const std::string &hash, const u256& pay, const u256& fee, const std::string &action, const std::string &signedTx, u256 &balance, u256 &froze);
    int checkActiveCard(const QString &clientID, bool &active, QString &code, QString& psw);	//检查是否激活
    int activeCard(const QString &clientID, const QString& sn, const QString &code, const QString &psw, QString& message);	//激活账户
    int getCentralBalance(const QString &clientID, const int kind, CentralBalance& result);	//获取中央账户余额，先只取BMToken
	int getUnionCount(const QString& clientID, int& softCount, int& hardCount);
	int Unbind(const QString& clientID);

    //以下函数非线程安全
	int getPrice(const CoinType& type, CurrencyType currencyType, double &price);
	int updateHTicker();

private:
	QMutex mutex;

	int parseResult(const QByteArray& result, QJsonDocument& doc, QJsonObject& data);
    int parseArrayResult(const QByteArray& result, QJsonDocument& doc, QJsonArray& data);
};

