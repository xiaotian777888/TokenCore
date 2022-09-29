#pragma once

#include "global.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QMetaType>

class BCoinCfg {
public:
    enum State {
        NONE = 0,
        SHOW = 1,
    };

    BCoinCfg(const Coin& coin, const QString& display) :display_symbol(display), wallet_index(0), mCoin(&coin), state(State::NONE) {};

    const Coin& coin() const {
        return *mCoin;
    }

    const CoinType& coinType() const {
        return mCoin->type;
    }

    const QString& symbol() const {
        return mCoin->symbol;
    }

    const QString& displaySymbol() const {
        return display_symbol;
    }

    const QString coinName() const;

    void loadFromJson(QJsonObject json);
    QJsonObject toJson() const;

    BCoinCfg::State state;

    int wallet_index;
	int order;

private:
    QString display_symbol;
    const Coin* mCoin;
};

Q_DECLARE_METATYPE(BCoinCfg*);


class BWallet
{
public:
	QString wallet_address;

    u256 balance; //不含冻结部分
    u256 froze; //冻结部分

	double price; //汇率
    enum QueryState {
		INIT,
		BUSY,
        ERR,
		DONE,
	} query_state;

    //费率信息
	FeeInfo fee;

    BWallet(BCoinCfg& coin): coinCfg(&coin), balance(0), froze(0), price(0), query_state(QueryState::INIT) {}

	void set_wallet_address(const char* _wallet_address);

	double getCurrencyBalance() const;	//取得法币余额
	static QString formatMoney(double money, int dig);
	QString getCurrencyDisplayBalance() const;		//可显示的法币余额
    const u256& getBalance() const;
    void setBalance(const u256& balance, const u256& froze);
	QString getCoinDisplayBalance() const;		//可显示的代币余额(不同的币balance转换比例可能不同)

    const Coin& coin() const {
        return coinCfg->coin();
    }

    const CoinType& coinType() const {
        return coinCfg->coinType();
    }

    const QString& displaySymbol() const {
        return coinCfg->displaySymbol();
    }

    const QString& symbol() const {
        return coinCfg->symbol();
    }

    const QString coinName() const {
        return coinCfg->coinName();
    }

    int walletKeyIdx() const {
        return coinCfg->wallet_index;
    }

    BCoinCfg::State state() const {
        return coinCfg->state;
    }

    int order() const {
        return coinCfg->order;
    }

private:
    BCoinCfg* coinCfg;
};

Q_DECLARE_METATYPE(BWallet*);

