#include <stdio.h>
#include <EthApi.h>

#include "Coin.h"
#include "HttpAPI\BtxonAPI.h"
#include "CosExt.h"

using namespace std;

// 验证钱包地址是否有效
static void test_validate_address()
{
	bool ret = ETHAPI::validate_address("0xAB8582E91e2387E2CA67542B26E67FdD245e0f0b");
	VF("validate_address", ret);
}

static void test_get_private_key()
{
	string private_key = ETHAPI::get_private_key("37f63c464ed1e319103598012d13b5f48f4712fdd55766390eeb58f3812d71ef3da4d5eec187b80bc3896d95e3e7aaead526bedd999c3dd74ba0b137a9b194ae");
	VF("get_private_key", (private_key == "1a53fb52f459cf07130069a5fef054ccd7917e19879eff8afb723c5b3e97661d"));
}

static void test_get_public_key()
{
	string public_key = ETHAPI::get_public_key("9a3e738288cec8ec030d51338b02956951c294b725920082a441c0ba26f8a4b7");
	VF("get_public_key", (public_key == "03dc139242c971935076cd95ff25d7561c52c03943460b7b8b9a116762b711dead"));
}

static void test_get_address()
{
	string address = ETHAPI::get_address("03dc139242c971935076cd95ff25d7561c52c03943460b7b8b9a116762b711dead");
	VF("get_address", (address == "0xcFeFbd41025Fe3EAc18b532F3cA645e8B25Fd3a3"));
}

static void test_get_balance()
{
	char wallet_address[256];
	get_wallet_address(CT_ETH, 0, wallet_address);
	printf("ETH address: %s\n", wallet_address);

	BtxonAPI api;
	u256 balance, froze;
	int ret = api.fetchBalanceV2(CT_ETH, wallet_address, true, balance, froze);
	if (ret == 0)
		printf("balance=%s, forze=%s\n", gCoin["ETH"].to_display(balance).toStdString().c_str(), gCoin["ETH"].to_display(froze).toStdString().c_str());
	else
		printf("fetchBalanceV2 error\n");

	strcpy_s(wallet_address, 256, "0xAB8582E91e2387E2CA67542B26E67FdD245e0f0b");
	printf("ETH address: %s\n", wallet_address);
	ret = api.fetchBalanceV2(CT_ETH, wallet_address, true, balance, froze);
	if (ret == 0)
		printf("balance=%s, forze=%s\n", gCoin["ETH"].to_display(balance).toStdString().c_str(), gCoin["ETH"].to_display(froze).toStdString().c_str());
	else
		printf("fetchBalanceV2 error\n");
}

// 硬件签名的交易过程
static void test_firmware_sign()
{
	UserTransaction ut;
	ut.from_address = "0xcFeFbd41025Fe3EAc18b532F3cA645e8B25Fd3a3";		// 模拟器
	ut.to_address = "0xAB8582E91e2387E2CA67542B26E67FdD245e0f0b";		// 硬件
	ut.pay = (uint64_t)gCoin["ETH"].from_display("0.0013");
	ut.from_wallet_index = 0;
	ut.change_wallet_index = 0;

	CoinType coinType = gCoin["ETH"].type;
	BtxonAPI api;
	int ret;

	// 取合约地址
	ut.contract_address = gCoin["ETH"].contract_address.toStdString().c_str();

	// 取交易费
	FeeInfo info;
	ret = api.fetchFee(coinType, info);
	if (ret)
	{
		printf("取交易费失败\n");
		return;
	}
	ut.fee_count = info.param;
	ut.fee_price = info.minFee;

	// 取nonce
	ret = api.getNonce(coinType, ut.from_address, ut.nonce);
	if (ret)
	{
		printf("取Nonce失败\n");
		return;
	}

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

	EthereumSignTx eth_tx;
	ret = ETHAPI::make_unsign_tx(eth_tx, coinType.minor, ut.from_address.c_str(), ut.to_address.c_str(), (uint64_t)ut.nonce, coinType.chain_id, ut.contract_address.c_str(), ut.pay, (uint64_t)ut.fee_count, (uint64_t)ut.fee_price);
	if (ret != 0)
		return;

	Binary custom_data;
	unsigned char firmware_data[4096];
	int firmware_size;
	ETHAPI::firmware_prepare_data(eth_tx, firmware_data, firmware_size, custom_data);

	int result_size;
	char result[4096];
	sw = cos.sign_transaction(coinType.major, coinType.minor, coinType.chain_id, ut.from_wallet_index, firmware_data, firmware_size, result, result_size);
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

	EthereumTxRequest txReq;
	ETHAPI::firmware_process_result(eth_tx, result, result_size, txReq);
	string str_tx = ETHAPI::make_sign_tx(eth_tx, txReq);

	u256 balance, froze;
	ret = api.sendSignedTransaction(coinType, ut.from_address, ut.to_address, ut.nonce, "eth", ut.pay, fee, "transfer", str_tx.c_str(), balance, froze);
	{
		printf("发送交易失败\n");
		return;
	}
}

// 软件签名的交易过程
static void test_sign()
{
	UserTransaction ut;
	ut.from_address = "0xcFeFbd41025Fe3EAc18b532F3cA645e8B25Fd3a3";		// 模拟器
	ut.to_address = "0xAB8582E91e2387E2CA67542B26E67FdD245e0f0b";		// 硬件
	ut.pay = (uint64_t)gCoin["ETH"].from_display("0.0013");
	ut.from_wallet_index = 0;
	ut.change_wallet_index = 0;

	CoinType coinType = gCoin["ETH"].type;
	BtxonAPI api;
	int ret;

	// 取合约地址
	ut.contract_address = gCoin["ETH"].contract_address.toStdString().c_str();

	// 取交易费
	FeeInfo info;
	ret = api.fetchFee(coinType, info);
	if (ret)
	{
		printf("取交易费失败\n");
		return;
	}
	ut.fee_count = info.param;
	ut.fee_price = info.minFee;

	// 取nonce
	ret = api.getNonce(coinType, ut.from_address, ut.nonce);
	if (ret)
	{
		printf("取Nonce失败\n");
		return;
	}

	u256 fee = ut.fee_count * ut.fee_price;

	// 软件签名 ==========================================
	EthereumSignTx eth_tx;
	ret = ETHAPI::make_unsign_tx(eth_tx, coinType.minor, ut.from_address.c_str(), ut.to_address.c_str(), (uint64_t)ut.nonce, coinType.chain_id, ut.contract_address.c_str(), ut.pay, (uint64_t)ut.fee_count, (uint64_t)ut.fee_price);
	if (ret != 0)
		return;

	EthereumTxRequest txReq;
	ETHAPI::sign_tx(eth_tx, "9a3e738288cec8ec030d51338b02956951c294b725920082a441c0ba26f8a4b7", txReq);
	string str_tx = ETHAPI::make_sign_tx(eth_tx, txReq);

	u256 balance, froze;
	ret = api.sendSignedTransaction(coinType, ut.from_address, ut.to_address, ut.nonce, "eth", ut.pay, fee, "transfer", str_tx.c_str(), balance, froze);
	if (ret != 0)
		return;
}


void EthTest()
{
	//test_validate_address();
	test_get_private_key();
	//test_get_public_key();
	//test_get_address();
	//test_get_balance();
	//test_firmware_sign();
	//test_sign();
}
