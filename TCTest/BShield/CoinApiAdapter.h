#pragma once

#include "global.h"
#include <stdint.h>
#include <string>
#include <functional>
#include <QByteArray>


//数字货币操作API
class CoinApiAdapter
{
public:
	//取余额
    static int getBalance(const CoinType& type, const std::string &wallet_address, u256& balance, u256& froze);
	//取费率
	static int geFeeInfo(const CoinType& type, FeeInfo& feeinfo);
	//发起签名后的转账
    static int sendSignedTransaction(const CoinType& coinType, const string &from, const string &to, const u256 nonce, const u256 &pay, const u256 &fee, const std::string &action, const std::string &signedTx, u256 &balance, u256 &froze);
	
	//ETH的nonce
	static int getNonce(const CoinType& coinType, const std::string &wallet_address, u256& count);

	//UI Model用API
	static int queryTransactions(const CoinType& coinType, const std::string &wallet_address, int offset, const std::function<void(bool, TransactionData*, int)> &callback);
    static int queryMessages(const std::string &clientID, int offset, int limit, const std::function<void(bool, MessageData*, int)> &callback);
    static int queryBalance(const vector<class BWallet>& wallet_list, const function<void(bool)> &callback);

private:
	CoinApiAdapter();
	~CoinApiAdapter();
};

