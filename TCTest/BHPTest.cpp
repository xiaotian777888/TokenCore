#include <iostream>
#include <BhpApi.h>

#include "Coin.h"
#include "HttpAPI\BtxonAPI.h"
#include "CosExt.h"

using namespace std;
using namespace BHPAPI;

static void test_get_private_key()
{
	{
		string private_key = BHPAPI::get_private_key("37f63c464ed1e319103598012d13b5f48f4712fdd55766390eeb58f3812d71ef3da4d5eec187b80bc3896d95e3e7aaead526bedd999c3dd74ba0b137a9b194ae");
		VF("get_private_key", (private_key == "94f628597b09c9664887657afcdf3366fab9d4b4824ac1380b4514b3e680c19c"));
	}
}

static void test_get_public_key()
{
	{
		string public_key = get_public_key("fdd7596639575967305867a31cb219a27e4b73cfc22894bca2c62a9c4dc7a2c6");
		VF("get_public_key", (public_key == "03d7d62e2a60e63479e0eec7cea7f3f13faf10e4ad283a1cff38ce1f5a42cc2e1b"));
	}

	{
		string public_key = get_public_key("68a1639fa801be966c6da92570f7a24caa26618061b4a3248b03247d7fef418a");
		VF("get_public_key", (public_key == "03e7bf8c9dedbc5041815ce822941fcbf0482793f656421443bde01e800614448d"));
	}
}

static void test_get_address()
{
	{
		string address = get_address("03d7d62e2a60e63479e0eec7cea7f3f13faf10e4ad283a1cff38ce1f5a42cc2e1b");
		VF("get_address", (address == "ASwD7MMtoE2t35H4ker5tBzAhp5dfBLsyh"));
	}

	{
		string address = get_address("03e7bf8c9dedbc5041815ce822941fcbf0482793f656421443bde01e800614448d");
		VF("get_address", (address == "AQNaVMJHdEX6qKvVkoh5Gfu2DbrQqU561S"));
	}
}

// 硬件签名的交易过程
static void test_firmware_sign()
{
	UserTransaction ut;
	ut.from_address = "APTqisEseDZ8fGWM6txcwg3ZpiYDHvBD7n";
	ut.to_address = "AQNaVMJHdEX6qKvVkoh5Gfu2DbrQqU561S";		// string str_pubkey_hash_1 = "8d8dfca8e949d5548e3193b88b59d682e3426b1c";	
	ut.change_address = "APTqisEseDZ8fGWM6txcwg3ZpiYDHvBD7n";
	ut.pay = (uint64_t)gCoin["BHP"].from_display("0.0012");
	ut.from_wallet_index = 0;
	ut.change_wallet_index = 0;

	CoinType coinType = gCoin["BHP"].type;
	BtxonAPI api;
	int ret;

	// 取余额
	u256 balance, froze;
	ret = api.fetchBalanceV2(coinType, ut.from_address, true, balance, froze);
	if (ret)
	{
		printf("取余额失败\n");
		return;
	}
	if (balance < ut.pay)
	{
		printf("余额不足\n");
		return;
	}

	// 取 UTXO
	ret = api.getUTXO(coinType, ut.from_address, ut.utxo_list);
	if (ret || ut.utxo_list.size() == 0)
	{
		printf("取UTXO失败\n");
		return;
	}

	// 取交易费
	FeeInfo info;
	ret = api.fetchFee(coinType, info);
	if (ret)
	{
		printf("取交易费失败\n");
		return;
	}

	// 必须在取得 utxo 后才能去算交易费
	int len = get_tx_len(&ut);
	ut.fee_count = (len + 511) / 512;
	if (ut.fee_count > 5)
		ut.fee_count = 5;
	ut.fee_price = 10000;

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

	ret = BHPAPI::make_unsign_tx(&ut);
	if (ret != 0)
		return;

	int result_size;
	char result[4096];

	Binary fdata = BHPAPI::firmware_prepare_data(&ut);
	sw = cos.sign_transaction(coinType.major, coinType.minor, coinType.chain_id, 0, fdata.data(), fdata.size(), result, result_size);
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

	BHPAPI::firmware_process_result(&ut, result, result_size);
	BHPAPI::make_sign_tx(&ut);

	printf("%s\n", ut.tx_str.c_str());
}

// 软件签名的交易过程测试
static void test_sign()
{
	UserTransaction ut;
	ut.from_address = "ASwD7MMtoE2t35H4ker5tBzAhp5dfBLsyh";
	ut.to_address = "AQNaVMJHdEX6qKvVkoh5Gfu2DbrQqU561S";		// string str_pubkey_hash_1 = "8d8dfca8e949d5548e3193b88b59d682e3426b1c";	
	ut.change_address = "ASwD7MMtoE2t35H4ker5tBzAhp5dfBLsyh";
	ut.pay = (uint64_t)gCoin["BHP"].from_display("0.0012");

	CoinType coinType = gCoin["BHP"].type;
	BtxonAPI api;
	int ret;

	// 取余额
	u256 balance, froze;
	ret = api.fetchBalanceV2(coinType, ut.from_address, true, balance, froze);
	if (ret)
	{
		printf("取余额失败\n");
		return;
	}
	if (balance < ut.pay)
	{
		printf("余额不足\n");
		return;
	}

	// 取 UTXO
	ret = api.getUTXO(coinType, ut.from_address, ut.utxo_list);
	if (ret || ut.utxo_list.size() == 0)
	{
		printf("取UTXO失败\n");
		return;
	}

	// 取交易费
	FeeInfo info;
	ret = api.fetchFee(coinType, info);
	if (ret)
	{
		printf("取交易费失败\n");
		return;
	}

	// 必须在取得 utxo 后才能去算交易费
	int len = get_tx_len(&ut);
	ut.fee_count = (len + 511) / 512;
	if (ut.fee_count > 5)
		ut.fee_count = 5;
	ut.fee_price = 10000;

	// 软件签名 ==========================================
	ret = make_unsign_tx(&ut);
	if (ret != 0)
		return;
	sign_tx(&ut, "fdd7596639575967305867a31cb219a27e4b73cfc22894bca2c62a9c4dc7a2c6");
	make_sign_tx(&ut);

	printf("%s\n", ut.tx_str.c_str());
}

void BHPTest()
{
	//test_get_private_key();
	//test_get_public_key();
	//test_get_address();

	//test_sign();
	//BHPAPI::dump_tx("80000001d946e9a15af33940f1390bbbffb7931a750829ae21a550b0eec94a9ede86f11800000254a80a4c72f6157a7af0a753fc4ac4af6b159a17634dd57fecf319feab6ff713c0d40100000000005e513e0d707c63e6ca422ae5c8eb843b0ba681ed54a80a4c72f6157a7af0a753fc4ac4af6b159a17634dd57fecf319feab6ff71370460d00000000007a6d5fc9483180cd05c87a3b2940f90a282945f20141404782adf6060c449fe2145fca2be633c08453ae3227729cb5b53df43b74f48c8a00eb37ef83b000e7bc9b396209a6f9f7d830a2fc4fbef62a7fdb7ea0ca90191b232103d7d62e2a60e63479e0eec7cea7f3f13faf10e4ad283a1cff38ce1f5a42cc2e1bac");
	test_firmware_sign();
	//BHPAPI::dump_tx("80000001d946e9a15af33940f1390bbbffb7931a750829ae21a550b0eec94a9ede86f11800000254a80a4c72f6157a7af0a753fc4ac4af6b159a17634dd57fecf319feab6ff713c0d40100000000005e513e0d707c63e6ca422ae5c8eb843b0ba681ed54a80a4c72f6157a7af0a753fc4ac4af6b159a17634dd57fecf319feab6ff71370460d00000000007a6d5fc9483180cd05c87a3b2940f90a282945f20141406ffaf546991fc3ea40375f8346759c73f34f17e5c5a8246b445218d535e6cfe44bcb5026e7aaaa6cb34c3a33876f0980c3a74dbb98315a5b6d2073c47d188ea1232102b54b7e1df98d25d48b59721abbacf892af19cdf01e045f21f836950fb34b1bc9ac");
	

#if 0
	// 模拟器钱包地址: APTqisEseDZ8fGWM6txcwg3ZpiYDHvBD7n
	char wallet_address[512];
	get_wallet_address(gCoin["BHP"].type, 0, wallet_address);
	printf("%s\n", wallet_address);
#endif

#if 0
	// 张扬的币盾(模拟器)
	string private_key = BHPAPI::get_private_key("a23af240b23010918472b3a7918899950f22e313168665839e2381618972cf572e36fbda9f4fbc7260193d522abb06a6612d879f756567c44d02c7687d2259e1");
	// public_key = 02b54b7e1df98d25d48b59721abbacf892af19cdf01e045f21f836950fb34b1bc9
	string public_key = BHPAPI::get_public_key(private_key);
	string address = BHPAPI::get_address(public_key);
	VF("validate_address", BHPAPI::validate_address(address));
#endif

	//string hash = BHPAPI::tx_hash("80000003364007f6bccf1168b1284ada0086b8f80b371a45c4588c3ab1c50ff691c52a840000364007f6bccf1168b1284ada0086b8f80b371a45c4588c3ab1c50ff691c52a840100fc5b8a6f2b52a650c0d9d1b39a568c98c136f00756b1d8f90b9aa3ac10c7944e00000254a80a4c72f6157a7af0a753fc4ac4af6b159a17634dd57fecf319feab6ff71300205fa012000000dd8ae706baa7acb84b894ba60a44eb68149ffcc654a80a4c72f6157a7af0a753fc4ac4af6b159a17634dd57fecf319feab6ff713fff37911620000008d8dfca8e949d5548e3193b88b59d682e3426b1c0141405d9c54c80f65df47d46289e25b8901c07068b1bd78529e06ce7b5b939511aff136eb461ec91dc4d2643d80a3bfa60e2c6574336753fb47ca60335430b6e99592232103d1998c163fc5137bb2ccc8062968db728081857ecf9d5c489c93935ab1aeece9ac");
	//BHPAPI::dump_tx("80000002d49333d271fd52b9ba3dd9ace5113249c1e6681640cebf8f2aa73a9acc29b09400006b1c9762e491b2a6291023a23e22317cdc4bdc626b2f4f0e10e3d120faf6edb701000254a80a4c72f6157a7af0a753fc4ac4af6b159a17634dd57fecf319feab6ff71300ca9a3b000000006da3cd7869e449546177c5fdac7fa02edd4f9f2054a80a4c72f6157a7af0a753fc4ac4af6b159a17634dd57fecf319feab6ff7130053ae1610000000dd8ae706baa7acb84b894ba60a44eb68149ffcc60141403cdfc5632c777bf6feabcb86c0024f2b2792caa9a397d53168faefb082cacc57108d6673241eade5d8a6d479d903cbef7611fbe0022984f75dcdafaad623545a2321032cc74af88291b8d7ad36812fd78152618a18c8914dad3662f8a0a0cb5aeea26eac");
}
