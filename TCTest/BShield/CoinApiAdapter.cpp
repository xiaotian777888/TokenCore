#include "CoinApiAdapter.h"

#include <EthApi.h>
#include <BtcApi.h>

#include <memory>
#include <map>

#include "HttpAPI/BtxonAPI.h"
#include "HttpAPI/BtxonAsyncAPI.h"

int CoinApiAdapter::getBalance(const CoinType& type, const std::string &wallet_address, u256& balance, u256& froze) {
    BtxonAPI api;
    u256 value;
    u256 frozeValue;
    int ret = api.fetchBalance(type, wallet_address, value, frozeValue);
    if (ret == 0) {
        balance = value;
        froze = frozeValue;
    }
    return ret;
}

int CoinApiAdapter::geFeeInfo(const CoinType& type, FeeInfo& feeinfo) {
	BtxonAPI api;
    FeeInfo value;
    int ret = api.fetchFee(type, value);
    if (ret == 0) {
		feeinfo = value;
	}
    return ret;
}

int CoinApiAdapter::sendSignedTransaction(const CoinType& type, const string &from, const string &to, const u256 nonce, const u256 &pay, const u256 &fee, const std::string &action, const std::string &signedTx, u256 &balance, u256& froze) {
    u256 value;
    u256 frozeValue;
    if (type.major == CoinTypeMajor::BTC) {
        string txhash = BTCAPI::tx_hash(signedTx);
        BtxonAPI api;
        int ret = api.sendSignedTransaction(type, from, to, nonce, txhash, pay, fee, "transfer", signedTx, value, frozeValue);
        if (ret == 0) {
            balance = value;
            froze = frozeValue;
        }
        return ret;
	}
	else if (type.major == CoinTypeMajor::ETH) {
        BtxonAPI api;
        int ret = api.sendSignedTransaction(type, from, to, nonce, "eth", pay, fee, action, signedTx, value, frozeValue);
        if (ret == 0) {
            balance = value;
            froze = frozeValue;
        }
        return ret;
	}
    else if (type.major == CoinTypeMajor::EOS) {
        BtxonAPI api;
        int ret = api.sendSignedTransaction(type, from, to, nonce, "eos", pay, fee, action, signedTx, value, frozeValue);
        if (ret == 0) {
            balance = value;
            froze = frozeValue;
        }
        return ret;
    }
    return -1;
}

int CoinApiAdapter::getNonce(const CoinType& coinType, const std::string &wallet_address, u256& count) {
	BtxonAPI api;
	u256 value;
	int ret = api.getNonce(coinType, wallet_address, value);
	if (ret == 0) {
		count = value;
	}
	return ret;
}

int CoinApiAdapter::queryTransactions(const CoinType& type, const std::string &wallet_address, int offset, const function<void(bool, TransactionData*, int)> &callback) {
    BtxonAsyncAPI *api = new BtxonAsyncAPI();
    api->queryTransactions(type, wallet_address, offset, [api, callback, offset](bool success, TransactionData* datas, int size) {
        //TODO 这个方式有点hack
        callback(success, datas, size);
        delete[] datas;
        api->deleteLater();
    });
    return 0;
}


int CoinApiAdapter::queryMessages(const std::string &clientID, int offset, int limit, const std::function<void(bool, MessageData*, int)> &callback) {
	BtxonAsyncAPI *api = new BtxonAsyncAPI();
    api->queryMessages(clientID, offset, limit, [api, callback, offset, limit](bool success, MessageData* datas, int size) {
		//TODO 这个方式有点hack
		callback(success, datas, size);
		delete[] datas;
		api->deleteLater();
	});
	return 0;
}

int CoinApiAdapter::queryBalance(const vector<BWallet>& wallet_list, const function<void(bool)> &callback) {
    BtxonAsyncAPI *api = new BtxonAsyncAPI();
    api->queryBalance(wallet_list, [api, callback](bool success) {
        callback(success);
        api->deleteLater();
    });
    return 0;
}


CoinApiAdapter::CoinApiAdapter()
{
}

CoinApiAdapter::~CoinApiAdapter()
{
}
