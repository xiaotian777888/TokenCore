#include "BWallet.h"

#include <QCoreApplication>

const QString BCoinCfg::coinName() const
{
    return QCoreApplication::translate("Coin", mCoin->display_name.toStdString().c_str());
}

void BCoinCfg::loadFromJson(QJsonObject json)
{
    if (json.contains("index")) {
        wallet_index = json["index"].toInt();
    }
    if (json.contains("state")) {
        state = (BCoinCfg::State)json["state"].toInt();
    }
	if (json.contains("order")) {
		order = json["order"].toInt();
	}
}

QJsonObject BCoinCfg::toJson() const
{
    QJsonObject walletJson;
    walletJson.insert("state", state);

    return walletJson;
};

void BWallet::set_wallet_address(const char* _wallet_address)
{
	wallet_address = QString(QLatin1String(_wallet_address));
    const CoinType& coin_type = coinType();
	if (coin_type.major == ETH) {
		wallet_address = wallet_address.toLower();
	}
}

//取得法币余额
double BWallet::getCurrencyBalance() const
{
	return coinCfg->coin().to_double(getBalance()) * price;
}

QString BWallet::formatMoney(double money, int dig)
{
    double yi_div;
    double wan_div;
    if (language == "zh-CN") {
        yi_div = 100000000.0;
        wan_div = 10000.0;
    }
    else {
        yi_div = 1000000000.0;
        wan_div = 1000000.0;
    }

	QString str = "0";
	QString plus = "";
    double yi = money / yi_div;
	if (yi > 1) {
		str = QString::number(yi, 'f', dig);
        plus = QObject::trUtf8("亿");
	}
	else {
        double wan = money / wan_div;
		if (wan > 1) {
			str = QString::number(wan, 'f', dig);
            plus = QObject::trUtf8("万");
		}
		else {
			str = QString::number(money, 'f', dig);
		}
	}

	if (str.length() > 8) {
		str = str.left(8);
	}

	int i;
	for (i = str.length() - 1; i >= 0; i--)
	{
		//找末尾第一个不是0的地方
		if (str[i] != '0')
			break;
	}

	if (i == -1)
		return QString("0");	//全是0，那就是0吧

	if (str[i] == '.')
		i--;

	str = str.left(i + 1);

	return str + plus;
}

//可显示的法币余额
QString BWallet::getCurrencyDisplayBalance() const
{
	double dbalance = coinCfg->coin().to_double(getBalance());
	double cny = dbalance * price;
    return QObject::trUtf8("%1 %2 \342\211\210 \302\245 %3").arg(displaySymbol()).arg(formatMoney(dbalance, 6)).arg(price > 0 ? formatMoney(cny, 3): "--");
}

const u256& BWallet::getBalance() const
{
	return balance;
}

void BWallet::setBalance(const u256 &balance, const u256 &froze)
{
    this->balance = balance;
    this->froze = froze;
}

//可显示的代币余额(不同的币balance转换比例可能不同)
QString BWallet::getCoinDisplayBalance() const
{
    return coinCfg->coin().to_display(getBalance());
}
