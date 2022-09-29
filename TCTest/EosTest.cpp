#include <QFile>
#include <QFileInfo>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <iostream>
#include <EosApi.h>

#include "CoinApiAdapter.h"
#include "Coin.h"
#include "HttpAPI\BtxonAPI.h"
#include "CosExt.h"

// 验证钱包地址是否有效
static void test_validate_address()
{
	bool ret = EOSAPI::validate_account("btxontest222");
	VF("validate_address", ret);
}

static void test_get_private_key()
{
	string private_key = EOSAPI::get_private_key("37f63c464ed1e319103598012d13b5f48f4712fdd55766390eeb58f3812d71ef3da4d5eec187b80bc3896d95e3e7aaead526bedd999c3dd74ba0b137a9b194ae");
	VF("get_private_key", (private_key == "1a53fb52f459cf07130069a5fef054ccd7917e19879eff8afb723c5b3e97661d"));
}

static void test_get_public_key()
{
	string public_key1 = EOSAPI::get_public_key("5HsSZvcoumpUxZnJoWaqD752MTuqXjNWVoiMJiRQVqupmYfyTMe");
	VF("get_public_key1", (public_key1 == "EOS8Wpto5QktoyFFf11YNLFH1zpXDL69RWQanSG3jnrwuN368w2TX"));	//曾经绑定到 yumenglei123

	//3MVF56nxYcRrzJ7yLheA5p5omxab8Nh6Ai
	//string public_key2 = EOSAPI::get_public_key("5J4zpeaYqKaXD52FeQfuUL1jncfU7AbyqKcHNcp2YzvwDCZT9R8");	// 无绑定
	//VF("get_public_key2", (public_key2 == "EOS7WSfPmLAeAwsFJ5J69E981myH9tA4NKhQtbPXKuniPbwvQMHZ7"));

	string public_key2 = EOSAPI::get_public_key("5Ki3MqDVhUaxVKQR2dYAA9WKZw34aPHpegzEyXpgajFxc1PjHXg");
	VF("get_public_key2", (public_key2 == "EOS7YWjcRZCaFdDzZWvknASE4QMzJKpNN8XZvisRFH758TPhbysdh"));	// btxontest444 的 owner

	string public_key3 = EOSAPI::get_public_key("5KV6Whwd9CW6SJw9iLqQjrYQozzAk9Dt4y8tGGB5tezzLq2Pdf7");
	VF("get_public_key3", (public_key3 == "EOS5JbLWFBkhFkEmA33ZkCfHFfdxbVyqK4bUaEsuaveLQAv984qZd"));	//曾经绑定到 yumenglei123

	string public_key4 = EOSAPI::get_public_key("5JA8sJQgprsyQAKKGRYVazym2T77zHGJnuY78eVkawt3JNNmkQF");
	VF("get_public_key4", (public_key4 == "EOS88F37stW2CRRHWXja1YXfGVC4mnBAwHwGGDYkvSjzdn6ygwRMd"));	// btxontest333 的 owner
}

static void test_get_key_account()
{
	BtxonAPI api;
	const CoinType& coinType = CT_EOS;
	vector<string> accounts;
	string public_key;

	// 模拟器 =================================================
	public_key = "EOS8VUVM6gPeGiekk64j6wGYPzRwhxoVvQHLSQjiWWq6yoYQvZFLz";
	if (api.getKeyAccounts(coinType, public_key, accounts) != 0)
	{
		printf("failed\n");
		return;
	}

	printf("\n%s\n", public_key.c_str());
	for (int i = 0; i < accounts.size(); i++)
		printf("%s\n", accounts[i].c_str());

	// =================================================
	public_key = "EOS88F37stW2CRRHWXja1YXfGVC4mnBAwHwGGDYkvSjzdn6ygwRMd";
	if (api.getKeyAccounts(coinType, public_key, accounts) != 0)
	{
		printf("failed\n");
		return;
	}

	printf("\n%s\n", public_key.c_str());
	for (int i = 0; i < accounts.size(); i++)
		printf("%s\n", accounts[i].c_str());
}

static void test_get_balance()
{
	char wallet_address[256];
	get_wallet_address(CT_EOS, 0, wallet_address);
	printf("EOS public key: %s\n", wallet_address);

	BtxonAPI api;
	vector<std::string> accounts;
	if (api.getKeyAccounts(CT_EOS, wallet_address, accounts) != 0)
	{
		printf("查询 EOS 账户失败\n");
		return;
	}

	if (accounts.size() == 0)
	{
		printf("此硬件没有绑定的 EOS 账户\n");
		return;
	}
	printf("EOS account: %s\n", accounts[0].c_str());

	u256 balance, froze;
	int ret = api.fetchBalanceV2(CT_EOS, accounts[0], true, balance, froze);
	if (ret == 0)
		printf("balance=%s, forze=%s\n", gCoin["EOS"].to_display(balance).toStdString().c_str(), gCoin["EOS"].to_display(froze).toStdString().c_str());
	else
		printf("fetchBalanceV2 error\n");
}

int EOSImport(QString& FWpubKey, QString& privateKey, const CoinType& coinType, string account)
{
	BtxonAPI api;
	QJsonValue accountInfo;

	//printf("Import\nFWpubKey=%s\nprivateKey=%s\naccount=%s\n", FWpubKey.toStdString().c_str(), privateKey.toStdString().c_str(), account.c_str());

	int retcode = api.getEOSAccount(coinType, account, accountInfo);
	if (retcode != 0)
	{
		printf("api.getEOSAccount: 获取账户信息失败，请重试\n");
		return -1;
	}
	if (!accountInfo.isObject())
	{
		printf("账户不存在，请重试\n");
		return -1;
	}

	std::string info;
	std::string binArgs;

	retcode = api.getEOSInfo(coinType, info);
	if (retcode != 0)
	{
		printf("api.getEOSInfo: 网络错误，请重试\n");
		return -1;
	}

	QJsonObject authObj;
	authObj.insert("permission", "active");
	authObj.insert("parent", "owner");
	authObj.insert("account", QString::fromStdString(account));

	QJsonObject accountObj = accountInfo.toObject();

	QJsonArray permissions = accountObj["permissions"].toArray();
	for (size_t itemidx = 0; itemidx < permissions.size(); itemidx++)
	{
		auto item = permissions.at(itemidx);
		if (item["perm_name"].toString() == "active" && item["parent"].toString() == "owner")
		{
			QJsonObject auth = item["required_auth"].toObject();
			QJsonArray keys = auth["keys"].toArray();
			int insertpos = 0;
			for (size_t keyidx = 0; keyidx < keys.size(); keyidx++)
			{
				auto keyItem = keys.at(keyidx);
				QString key = keyItem["key"].toString();
				if (key == FWpubKey) {
					printf("已经绑定过了");
					return 0;
				}
				if (key < FWpubKey) {
					insertpos++;
				}
			}
			QJsonObject key;
			key.insert("key", FWpubKey);
			key.insert("weight", auth["threshold"].toInt());
			keys.insert(insertpos, key);

			auth["keys"] = keys;
			authObj.insert("auth", auth);
			break;
		}
	}

	//printf("=======\n%s\n=======\n", QJsonDocument(authObj).toJson(QJsonDocument::Compact).toStdString().c_str());
	retcode = api.encodeEOSABI(coinType, "eosio", "updateauth", QJsonDocument(authObj).toJson(QJsonDocument::Compact).toStdString(), binArgs);
	if (retcode != 0)
	{
		printf("api.encodeEOSABI: 网络错误，请重试\n");
		return -1;
	}

	EOS_TX* eos_tx = nullptr;
	retcode = EOSAPI::make_unsign_tx(&eos_tx, true, "eosio", "updateauth", account, binArgs, info);
	if (retcode != 0)
	{
		printf("eos_TxCreate: 创建交易失败，请重试\n");
		return -1;
	}

	retcode = EOSAPI::sign_tx(eos_tx, privateKey.toStdString());
	if (retcode != 0)
	{
		printf("eos_TxSign: 签名失败，请检测输入的私钥\n");
		return -1;
	}

	Binary nulldata;
	string rawTransaction = EOSAPI::make_sign_tx(eos_tx, nulldata);
	if (retcode != 0)
	{
		printf("eos_TxPush: 签名失败，请检测输入的私钥\n");
		return -1;
	}

	u256 value;
	u256 frozeValue;
	retcode = api.sendSignedTransaction(coinType, account, account, 0, "eos", 0, 0, "updateauth", rawTransaction, value, frozeValue);
	if (retcode != 0)
	{
		printf("CoinApiAdapter::sendSignedTransaction: %s\n", QString::fromUtf8(api.error_msg.toStdString().c_str()).toStdString().c_str());
		return -1;
	}

	EOSAPI::release(&eos_tx);
	return 0;
}

int EOSDeImport(QString& FWpubKey, QString& privateKey, const CoinType& coinType, string account)
{
	BtxonAPI api;
	QJsonValue accountInfo;

	//printf("DeImport\nFWpubKey=%s\nprivateKey=%s\naccount=%s\n", FWpubKey.toStdString().c_str(), privateKey.toStdString().c_str(), account.c_str());

	int retcode = api.getEOSAccount(coinType, account, accountInfo);
	if (retcode != 0)
	{
		printf("api.getEOSAccount: 获取账户信息失败，请重试\n");
		return -1;
	}
	if (!accountInfo.isObject())
	{
		printf("账户不存在，请重试\n");
		return -1;
	}

	std::string info;
	std::string binArgs;

	retcode = api.getEOSInfo(coinType, info);
	if (retcode != 0) {
		printf("api.getEOSInfo: 网络错误，请重试\n");
	}

	QJsonObject authObj;
	authObj.insert("permission", "active");
	authObj.insert("parent", "owner");
	authObj.insert("account", QString::fromStdString(account));

	QJsonObject accountObj = accountInfo.toObject();

	QJsonArray permissions = accountObj["permissions"].toArray();
	for (size_t itemidx = 0; itemidx < permissions.size(); itemidx++)
	{
		auto item = permissions.at(itemidx);
		if (item["perm_name"].toString() == "active" && item["parent"].toString() == "owner")
		{
			QJsonObject auth = item["required_auth"].toObject();
			QJsonArray keys = auth["keys"].toArray();
			int insertpos = 0;
			for (size_t keyidx = 0; keyidx < keys.size(); keyidx++)
			{
				auto keyItem = keys.at(keyidx);
				QString key = keyItem["key"].toString();
				if (key == FWpubKey) {
					keys.erase(keys.begin() + keyidx);
				}
			}

			auth["keys"] = keys;
			authObj.insert("auth", auth);
			break;
		}
	}

	//printf("=======\n%s\n=======\n", QJsonDocument(authObj).toJson(QJsonDocument::Compact).toStdString().c_str());
	retcode = api.encodeEOSABI(coinType, "eosio", "updateauth", QJsonDocument(authObj).toJson(QJsonDocument::Compact).toStdString(), binArgs);
	if (retcode != 0) {
		printf("api.encodeEOSABI: 网络错误，请重试\n");
		return -1;
	}

	EOS_TX* eos_tx = nullptr;

	retcode = EOSAPI::make_unsign_tx(&eos_tx, true, "eosio", "updateauth", account, binArgs, info);
	if (retcode != 0) {
		printf("eos_TxCreate: 创建交易失败，请重试\n");
		return -1;
	}

	retcode = EOSAPI::sign_tx(eos_tx, privateKey.toStdString());
	if (retcode != 0) {
		printf("eos_TxSign: 签名失败，请检测输入的私钥\n");
		return -1;
	}

	Binary nulldata;
	string rawTransaction = EOSAPI::make_sign_tx(eos_tx, nulldata);
	if (retcode != 0)
	{
		printf("eos_TxPush: 签名失败，请检测输入的私钥\n");
		return -1;
	}

	u256 value;
	u256 frozeValue;
	retcode = api.sendSignedTransaction(coinType, account, account, 0, "eos", 0, 0, "updateauth", rawTransaction, value, frozeValue);
	if (retcode != 0)
	{
		printf("CoinApiAdapter::sendSignedTransaction: %s\n", api.error_msg.toStdString().c_str());
		return -1;
	}

	EOSAPI::release(&eos_tx);
	return 0;
}

void test_bind_unbind()
{
	char public_key[256] = "EOS8VUVM6gPeGiekk64j6wGYPzRwhxoVvQHLSQjiWWq6yoYQvZFLz";
	QString private_key("5Ki3MqDVhUaxVKQR2dYAA9WKZw34aPHpegzEyXpgajFxc1PjHXg"); // btxontest333 的 owner

	int ret;
	ret = EOSImport(QString(public_key), private_key, CT_EOS, "btxontest222");
	VF("EOSImport", ret==0);
	ret = EOSDeImport(QString(public_key), private_key, CT_EOS, "btxontest222");
	VF("EOSDeImport", ret==0);
}

void test_bind_fireware()
{
	BtxonAPI api;

	char public_key[256];
	get_wallet_address(CT_EOS, 0, public_key);
	printf("EOS public key: %s\n", public_key);

	QString private_key("5Ki3MqDVhUaxVKQR2dYAA9WKZw34aPHpegzEyXpgajFxc1PjHXg"); // btxontest333 的 owner
	string main_public_key = EOSAPI::get_public_key(private_key.toStdString());

	vector<std::string> accounts;
	if (api.getKeyAccounts(CT_EOS, main_public_key, accounts))
		return;

	for (size_t i = 0; i < accounts.size(); i++)
	{
		if (0 == EOSImport(QString(public_key), private_key, CT_EOS, accounts[i]))
		//if (0 == EOSDeImport(QString(public_key), private_key, CT_EOS, accounts[i]))
		{
			printf("绑定成功，请重新连接设备\n");
		}
	}
}

// 硬件签名的交易过程
static void test_firmware_sign()
{
	UserTransaction ut;
	ut.from_address = "btxontest222";
	ut.to_address = "yumenglei123";
	ut.pay = (uint64_t)gCoin["EOS"].from_display("0.011");
	ut.from_wallet_index = 0;
	ut.change_wallet_index = 0;

	CoinType coinType = gCoin["EOS"].type;
	BtxonAPI api;
	int ret;

	// 硬件签名 ==========================================
	Cos cos;
	SW sw;

	if (cos.find() <= 0)
		return;

	if (!cos.connect(0))
		return;

	if (!cos.open_channel())
		return;

	if (verify_pin(cos) != 0)
		return;

	u256 fee = ut.fee_count * ut.fee_price;

	string info;
	ret = api.getEOSInfo(coinType, info);
	if (ret != 0)
		return;

	QJsonObject jargs;
	jargs.insert("from", QString::fromStdString(ut.from_address));
	jargs.insert("memo", "");
	jargs.insert("quantity", QString("%1 %2").arg(gCoin["EOS"].to_display(ut.pay, false)).arg(gCoin["EOS"].symbol));
	jargs.insert("to", QString::fromStdString(ut.to_address));

	string binArgs;
	ret = api.encodeEOSABI(coinType, "eosio.token", "transfer", QJsonDocument(jargs).toJson(QJsonDocument::Compact).toStdString(), binArgs);
	if (ret != 0)
		return;

	EOS_TX* eos_tx = nullptr;
	ret = EOSAPI::make_unsign_tx(&eos_tx, false, "eosio.token", "transfer", ut.from_address, binArgs, info);
	if (ret != 0)
		return;

	Binary data = EOSAPI::get_data(eos_tx);
	if (data.size() == 0)
		return;

	int result_size;
	char result[4096];

	sw = cos.sign_transaction(coinType.major, coinType.minor, coinType.chain_id, ut.from_wallet_index, data.data(), data.size(), result, result_size);
	if (sw != SW_6D82_GET_INTERACTION_LATER)
	{
		printf("签名失败:%04x\n", sw);
		return;
	}

	char buffer[1024];
	printf("输入验证码:");
	scanf_s("%s", buffer, 1024);

	sw = cos.sign_transaction_end(buffer, result, result_size);
	if (sw != SW_9000_SUCCESS)
	{
		printf("验证失败\n");
		return;
	}

	Binary rdata(result, result_size);
	string rawTransaction = EOSAPI::make_sign_tx(eos_tx, rdata);
	if (rawTransaction.size() == 0)
		return;

	u256 balance, froze;
	ret = api.sendSignedTransaction(coinType, ut.from_address, ut.to_address, 0, "eos", ut.pay, fee, "transfer", rawTransaction, balance, froze);
	if (ret != 0)
	{
		printf("发送交易失败\n");
		return;
	}
}

// 软件签名的交易过程测试
static void test_sign()
{
	UserTransaction ut;
	ut.from_address = "btxontest333";
	ut.to_address = "yumenglei123";
	ut.pay = (uint64_t)gCoin["EOS"].from_display("0.011");
	ut.from_wallet_index = 0;
	ut.change_wallet_index = 0;

	string private_key("5Ki3MqDVhUaxVKQR2dYAA9WKZw34aPHpegzEyXpgajFxc1PjHXg");

	CoinType coinType = gCoin["EOS"].type;
	BtxonAPI api;
	int ret;

	// 软件签名 ==========================================
	u256 fee = ut.fee_count * ut.fee_price;

	string info;
	ret = api.getEOSInfo(coinType, info);
	if (ret != 0)
		return;

	QJsonObject jargs;
	jargs.insert("from", QString::fromStdString(ut.from_address));
	jargs.insert("memo", "");
	jargs.insert("quantity", QString("%1 %2").arg(gCoin["EOS"].to_display(ut.pay, false)).arg(gCoin["EOS"].symbol));
	jargs.insert("to", QString::fromStdString(ut.to_address));

	string binArgs;
	ret = api.encodeEOSABI(coinType, "eosio.token", "transfer", QJsonDocument(jargs).toJson(QJsonDocument::Compact).toStdString(), binArgs);
	if (ret != 0)
		return;

	EOS_TX* eos_tx = nullptr;
	ret = EOSAPI::make_unsign_tx(&eos_tx, false, "eosio.token", "transfer", ut.from_address, binArgs, info);
	if (ret != 0)
		return;

	ret = EOSAPI::sign_tx(eos_tx, private_key);
	if (ret != 0)
		return;

	Binary nulldata;
	string rawTransaction = EOSAPI::make_sign_tx(eos_tx, nulldata);
	if (rawTransaction.empty())
		return;

	u256 balance, froze;
	ret = api.sendSignedTransaction(coinType, ut.from_address, ut.to_address, 0, "eos", ut.pay, fee, "transfer", rawTransaction, balance, froze);
	if (ret != 0)
		return;

	EOSAPI::release(&eos_tx);
}

void EosTest()
{
	//test_validate_address();
	test_get_private_key();
	//test_get_public_key();
	//test_get_key_account();
	//test_get_balance();
	//test_bind_unbind();
	//test_bind_fireware();
	//test_firmware_sign();
	//test_sign();
}
