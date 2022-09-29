#include <BtcApi.h>

#include "Coin.h"
#include "HttpAPI\BtxonAPI.h"
#include "CosExt.h"

// 验证钱包地址是否有效
static void test_validate_address()
{
	{
		bool ret = BTCAPI::validate_address("1HT7xU2Ngenf7D4yocz2SAcnNLW7rK8d4E");
		VF("validate_address", ret);
	}

	{
		bool ret = BTCAPI::validate_address("mzGP6r6MUcwdocow6JnCxmxwrctKQqSL84");
		VF("validate_address", ret);
	}
}

static void test_get_private_key()
{
	{
		string private_key = BTCAPI::get_private_key(false, "37f63c464ed1e319103598012d13b5f48f4712fdd55766390eeb58f3812d71ef3da4d5eec187b80bc3896d95e3e7aaead526bedd999c3dd74ba0b137a9b194ae");
		VF("get_private_key", (private_key == "294fed288cf10b6cff57e855765759114b9603f9f235a916c939239e97b06d77"));
	}

	{
		string private_key = BTCAPI::get_private_key(true, "37f63c464ed1e319103598012d13b5f48f4712fdd55766390eeb58f3812d71ef3da4d5eec187b80bc3896d95e3e7aaead526bedd999c3dd74ba0b137a9b194ae");
		VF("get_private_key", (private_key == "9572e9246fd94cd9545e36a4bab9429477e77e1a21c0922e251fdbcd20021f8e"));
	}
}

static void test_get_public_key()
{
	{
		string public_key = BTCAPI::get_public_key("8ed1d17dabce1fccbbe5e9bf008b318334e5bcc78eb9e7c1ea850b7eb0ddb9c8");
		VF("get_public_key", (public_key == "0247140d2811498679fe9a0467a75ac7aa581476c102d27377bc0232635af8ad36"));
	}

	{
		string public_key = BTCAPI::get_public_key("8ed1d17dabce1fccbbe5e9bf008b318334e5bcc78eb9e7c1ea850b7eb0ddb9c801");
		VF("get_public_key", (public_key == "0247140d2811498679fe9a0467a75ac7aa581476c102d27377bc0232635af8ad36"));
	}

	{
		string public_key = BTCAPI::get_public_key("e50645d5eb93f456fd92502a891ba4fb199ba1e5c313b43e1eedca349905fd1701");	// 安康的
		VF("get_public_key", (public_key == "037eefdac5b0529fe7193bccaea2202ec8f2f3ad4856f213309d67276ef1392a1a"));
	}
}

static void test_get_address()
{
	{
		string address = BTCAPI::get_address(true, "0247140d2811498679fe9a0467a75ac7aa581476c102d27377bc0232635af8ad36");
		VF("get_address", (address == "mtqFYNDizo282Y29kjwXEf2dCkfdZZydbf"));
	}

	{
		string address = BTCAPI::get_address(false, "0247140d2811498679fe9a0467a75ac7aa581476c102d27377bc0232635af8ad36");
		VF("get_address", (address == "1EKJFK8kBmasFRYY3Ay9QjpJLm4vemJtC1"));
	}

	{
		string address3 = BTCAPI::get_address(true, "0447140d2811498679fe9a0467a75ac7aa581476c102d27377bc0232635af8ad36e87bb04f401be3b770a0f3e2267a6c3b14a3074f6b5ce4419f1fcdc1ca4b1cb6");
		VF("get_address", (address3 == "modCdv4bPiVHWRhJPLRxdfKuzjxz275cah"));
	}

	{
		string address = BTCAPI::get_address(false, "0447140d2811498679fe9a0467a75ac7aa581476c102d27377bc0232635af8ad36e87bb04f401be3b770a0f3e2267a6c3b14a3074f6b5ce4419f1fcdc1ca4b1cb6");
		VF("get_address", (address == "197FLrycah42jKDgfmTaok7b8kNHA7R2ih"));
	}
}

static void test_decode_script()
{
	string script = BTCAPI::decode_script("76a914d073c96316d066e5ea65c23dcd4ebaf6126ce9fb88ac");
	VF("decode_script", (script == "dup hash160 [d073c96316d066e5ea65c23dcd4ebaf6126ce9fb] equalverify checksig"));
}

// BTC 硬件签名的交易过程
static void test_firmware_sign()
{
	UserTransaction ut;
	ut.from_address = "mg9cmEEV7GB7NfsXPqc9yUvUjYH9EMUsuP";
	ut.to_address = "mjqGHq79osmPUsTHeREtUx3egEa7z3o7Yo";
	ut.change_address = "mg9cmEEV7GB7NfsXPqc9yUvUjYH9EMUsuP";

	CoinType coinType = gCoin["tBTC"].type;
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
	if (ret)
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

	ut.pay = gCoin["tBTC"].from_display("0.2");
	ut.fee_count = BTCAPI::get_tx_len(&ut);
	ut.fee_price = (u256)info.midFee;
	ut.from_wallet_index = 0;
	ut.change_wallet_index = 0;

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

	ret = BTCAPI::make_unsign_tx(&ut);

	int result_size;
	char result[4096];

	for (int i = 0; i < ut.input_count; i++)
	{
		Binary fdata = BTCAPI::firmware_prepare_data(coinType.is_testnet(), &ut, i);
		printf("firmware_data%d:\n", i);
		binout(fdata.data(), fdata.size());
		printf("firmware_size%d:%d\n", i, (int)fdata.size());

		sw = cos.sign_transaction(coinType.major, coinType.minor, coinType.chain_id, 0, fdata.data(), fdata.size(), result, result_size);
		if (sw == SW_9000_SUCCESS)
		{
			BTCAPI::firmware_process_result(&ut, i, result, result_size);
			continue;
		}
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

		BTCAPI::firmware_process_result(&ut, i, result, result_size);
	}

	BTCAPI::make_sign_tx(coinType.is_testnet(), &ut);
	//BTCAPI::dump_tx(true, ut->tx_str);
	printf("%s\n", ut.tx_str.c_str());
}

// BTC 软件签名的交易过程测试
static void test_sign()
{
	UserTransaction ut;
	ut.from_address = "mg9cmEEV7GB7NfsXPqc9yUvUjYH9EMUsuP";
	ut.to_address = "mjqGHq79osmPUsTHeREtUx3egEa7z3o7Yo";
	ut.change_address = "mg9cmEEV7GB7NfsXPqc9yUvUjYH9EMUsuP";

	CoinType coinType = gCoin["tBTC"].type;
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
	if (ret)
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

	ut.pay = gCoin["tBTC"].from_display("0.2");
	ut.fee_count = BTCAPI::get_tx_len(&ut);
	ut.fee_price = (u256)info.midFee;
	ut.from_wallet_index = 0;
	ut.change_wallet_index = 0;

	// 软件签名 ==========================================
	ret = BTCAPI::make_unsign_tx(&ut);
	if (ret != 0)
		return;
	BTCAPI::sign_tx(coinType.is_testnet(), &ut, "09cebf09bdb895dfe3820ba35812bbb5be2472f6291f1a965cd0144c8ae0a453");
	BTCAPI::make_sign_tx(coinType.is_testnet(), &ut);

	printf("%s\n", ut.tx_str.c_str());
}

class Wallet
{
public:
	string private_key;
	string public_key;
	string address;

	Wallet() {}
	Wallet(const string private_key)
	{
		this->private_key = private_key;
		this->public_key = BTCAPI::get_public_key(private_key);
		this->address = BTCAPI::get_address(true, this->public_key);
	}

	void set_address(const string address)
	{
		this->address = address;
	}
};

void GetWalletBalance()
{
	vector<Wallet> wallet =
	{
		Wallet("07d92015577b1ff298a7f6530c1e733f75b6c717d0e14e9f9dc04c0f9cd3a8a1"),
		Wallet("6f37c7b514f7deac821fd33c591a89c28830d11929f282ca0ecb92ae335aae5b"),
		Wallet("2260d0236ce4bf2836aeea1fda679ac811e3c09e9e47e038d61034b1f491e75d"),
		Wallet("98d01965d5e1221a38db0912bf041d0c900a5fc002054d0b7dda210522e99c90"),	//张扬的硬件
	};

	/*
	我的钱包:
	wallet address: mjqGHq79osmPUsTHeREtUx3egEa7z3o7Yo 0.360979
	wallet address: mj7n7pgZppGFgDaoRHYLVuj4XAHUySCgqw 0.068870
	wallet address: mkyCHRRDAYWtbsUkPqaNP1jUUFQYdYJwHy 0.110000
	wallet address: mzGP6r6MUcwdocow6JnCxmxwrctKQqSL84 0.464730		(模拟器钱包)
	*/
	// 16Lk8o4uwCXYVnhancmbDSCAuVTQpE238x

	BtxonAPI api;
	u256 balance, forze;
	for (int i = 0; i < wallet.size(); i++)
	{
		api.fetchBalance(gCoin["tBTC"].type, wallet[i].address, balance, forze);
		printf("wallet address: %s (%s, %s)\n", wallet[i].address.c_str(), gCoin["tBTC"].to_display(balance).toStdString().c_str(), gCoin["BTC"].to_display(forze).toStdString().c_str());
	}

	Wallet fw_wallet;
	fw_wallet.set_address("muMCrdMHhtW21wUKfqBg8zWPSJV6eG9tjT");
	api.fetchBalance(gCoin["tBTC"].type, fw_wallet.address, balance, forze);
	printf("wallet address: %s (%s, %s)\n", fw_wallet.address.c_str(), gCoin["tBTC"].to_display(balance).toStdString().c_str(), gCoin["BTC"].to_display(forze).toStdString().c_str());

	// 15KJzn2AzrL8hkyfvrGWf2qKpEyR5U8u3Z
}

// USDT 软件签名的交易过程测试
void test_usdt_sign()
{
	UserTransaction ut;
	ut.from_address = "1DBPJJoRSarbjTeNmAUxwmRPYP6vEQgztD";
	ut.to_address = "16Lk8o4uwCXYVnhancmbDSCAuVTQpE238x";
	ut.change_address = "1DBPJJoRSarbjTeNmAUxwmRPYP6vEQgztD";
	ut.pay = (uint64_t)gCoin["USDT"].from_display("5.0");
	ut.from_wallet_index = 0;
	ut.change_wallet_index = 0;

	CoinType coinType = gCoin["USDT"].type;
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

	ut.pay = 546;	// 这个仅仅是用于计算交易长度时使用(BTC)
	ut.fee_count = USDTAPI::get_tx_len(&ut);
	ut.fee_price = (u256)info.midFee;
	ut.pay = (uint64_t)gCoin["USDT"].from_display("5.0");	// 恢复真正的交易值

	// 软件签名 ==========================================
	ret = USDTAPI::make_unsign_tx(&ut);
	if (ret != 0)
		return;
	BTCAPI::sign_tx(false, &ut, "8cde367ccbe92cc7c05dde05603ca416935919eb0276ce0eb307870942d92e00");
	BTCAPI::make_sign_tx(false, &ut);

	printf("%s\n", ut.tx_str.c_str());

	u256 value = USDTAPI::get_usdt_from_tx(false, ut.tx_str);
	printf("%s\n", value.str().c_str());
}

// sp
static void sp_test_sign()
{
	string seed = mnemonic_to_seed("manual shoot jelly view scrub head also price cliff upset honey farm daring among route cheese evidence caution joy lock asset occur catalog high", "");
	//string seed = mnemonic_to_seed("blue submit hurt base spray learn permit two absurd brown large extend awkward cool hair resist quarter fever brave sight palm argue adapt slush", "");
	string private_key = BTCAPI::get_private_key(false, seed);
	string public_key = BTCAPI::get_public_key(private_key);
	string address = BTCAPI::get_address(false, public_key);	// 1K2VpuurN1seRUvCUHj6DLGiFWASJifZ7y

	UserTransaction ut;
	ut.from_address = address;
	ut.to_address = "19uvdbPW1A2hDaNysFRB6xJDCEG5opAJg6";
	ut.change_address = address;
	//ut.pay = 15591;
	ut.pay = 10000;

	CoinType coinType = gCoin["BTC"].type;
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
	if (ret)
	{
		printf("取UTXO失败\n");
		return;
	}

	/*
	printf("points\n{\n");
	for (int i = 0; i < ut.utxo_list.size(); i++)
	{
		printf("\tpoint\n");
		printf("\t{\n");
		printf("\t\thash %s\n", ut.utxo_list[i].hash.c_str());
		printf("\t\tindex %u\n", ut.utxo_list[i].index);
		printf("\t\tvalue %I64u\n", ut.utxo_list[i].value);
		printf("\t\tscript %s\n", ut.utxo_list[i].script.c_str());
		printf("\t}\n");
	}
	printf("}\n");
	*/

	// 取交易费
	FeeInfo info;
	ret = api.fetchFee(coinType, info);
	if (ret)
	{
		printf("取交易费失败\n");
		return;
	}

	ut.fee_count = BTCAPI::get_tx_len(&ut);
	ut.fee_price = (u256)info.midFee;
	ut.from_wallet_index = 0;
	ut.change_wallet_index = 0;

	//ut.pay = balance - (ut.fee_count*ut.fee_price);

	// 软件签名 ==========================================
	ret = BTCAPI::make_unsign_tx(&ut);
	if (ret != 0)
		return;
	BTCAPI::sign_tx(coinType.is_testnet(), &ut, private_key);
	BTCAPI::make_sign_tx(coinType.is_testnet(), &ut);

	printf("%s\n", ut.tx_str.c_str());
}

void BTCTest()
{
	//test_validate_address();
	//test_get_private_key();
	//test_get_public_key();
	//test_get_address();
	//test_decode_script();
	//test_firmware_sign();
	//test_sign();

	//BTCAPI::dump_tx(false, "0200000005ad65867ffc89aa123f6f949569206e9feaa06f959b39ac28031f636d802a5a0f010000006b483045022100edaf6269014e5ebb048363076b251856bfc320896b389a4fdcfca19969d7b41302207606de5b2da1bbb2969e776483def616ae925e907bcfa1c34ccad81c35dc381e0121034f7d7dbe6383ab7392bd084bc2230a5d71a356d259b63a8211fde7963a6ac54afeffffff86c2bdb1ce6c42b84d161121df052f1fa8879effab70b4e0546b2fdc2894329b9c0000006b483045022100fe1dba1f0cb003d704dd705e0f5fb5d665178bef77083b67abd21feae23642c902205b44a702b67423de6971af3f6e4d9b9856fee67191221439aaf83ac445fbe27e012102250059deda6357b3a6a36e85fdc75b5f8ba626e04c01e54521aa9e8f9f8f67e2feffffff86c2bdb1ce6c42b84d161121df052f1fa8879effab70b4e0546b2fdc2894329bb10000006b483045022100ab4f70ba1ee1ba45337b6d8fe274c75ce58a5d395defd87e7a5ec108f297efcb02204fc3ab48fca57212b442d4a8ee5e45577e5fd7a6d5f82411d009d0db4fba38f2012102e6cd1608a55a5b856a3f5a7009322f2355ceb88c619f59f9792c9843d417efe3feffffff72f7c2a66de0a0e46f848fcbcf81b5be354d1e5f1324ce39bacc20d61e72286e000000006a47304402201e43add83c61ec51b84158628f2f9a2ba9ad143b006bee1285ac7735ed80a5ad0220255b328144c282e83a2c1286ff5f0e08082edc1e33cad6db02b7194f50b2167601210203090a0b9e917ce758bad053aaa0966a6b97fcb7ed02a41b6a40458f9017e648feffffff5bef1f3f09a81ac96bbb1b407ec1d1ca9658025ea99a9e6337ee9ace83fb5830030000006b483045022100cebbfda56777133a51a6152f2a3caba9bdcadbf874ed91810d6fdd927d608eb802201356065851aae6a7aeb51489a4280cd338dfb6dca637f1106b56fece02efd7eb012103b450cd8f896031f75c026772855aeecbc1c3b7d46380046551cbbd9a89a84f39feffffff026ec20c00000000001976a914e1d1d2c42f4b46d85ac3aacf4fda5986fd84b67088ac806532010000000017a914beac82207c8d64e734afeca04ae9977bcfbf5c43877c510800");
	//BTCAPI::dump_tx(false, "010000000132c7dbe543cb6b222a6988ab962064566a27c1077cd281936f7e61373fc5bb5e000000006b48304502210097fc2a813d23bcf56fe2a489d3163b94bfc029edf016856cc72a04208525b46d0220338265898aaaf8e391b3c612885306293aeb2bfe85d82658a34ac47f6bdfc85d012102ae3aabcdefdfd8c990a5d1cad524d0d91257a427eb78112cf7a266484699c30affffffff03409c0000000000001976a91485984864a7e26c3c4a1888373115494a56673b5d88ac0000000000000000166a146f6d6e69000000000000001f000000000098968022020000000000001976a91480d052caee0cbc838f340a5c16c1c708fb0c2ce688ac00000000");
	//BTCAPI::dump_tx(false, "0100000001887e205b6a3d96fa115424bdcc70a739d87078c0212da9ba91440ce9954a0b660000000000ffffffff02c0e1e400000000001976a91478ce9278e94d6de90a22b23fc475883dfbc422d588ac50c30000000000001976a91425322a09dcc10ae461c5ca617818aca8bfc63b3b88ac00000000");

	//BTCAPI::dump_tx(false, "010000000132c7dbe543cb6b222a6988ab962064566a27c1077cd281936f7e61373fc5bb5e000000006b48304502210097fc2a813d23bcf56fe2a489d3163b94bfc029edf016856cc72a04208525b46d0220338265898aaaf8e391b3c612885306293aeb2bfe85d82658a34ac47f6bdfc85d012102ae3aabcdefdfd8c990a5d1cad524d0d91257a427eb78112cf7a266484699c30affffffff03409c0000000000001976a91485984864a7e26c3c4a1888373115494a56673b5d88ac0000000000000000166a146f6d6e69000000000000001f000000000098968022020000000000001976a91480d052caee0cbc838f340a5c16c1c708fb0c2ce688ac00000000");
	//BTCAPI::dump_tx(true, "0100000002f0470a1d58e0e361ac6b73c47a048f81231b4d0d6426152a98814411c3036f54000000006b483045022100da3521feff575301e574fc57f33b089bc4eec838b15cad985b74d6418da8cfc50220408211a0bc4ad2c86fb8dceaba61958eabcc11964272daa46eb23bf72af06731012102d8dce330e3db29d953a579b4bddf0cf76fe264ad7e5a6454fe66fa2a6d31997bffffffffa4a3510644f3b605247c3c23b7ab94068f7db3a70ce9d2a8de8d8837d62da9bd000000006a47304402207480c18626de64a12950a8a818541cf9b39aeded1f99b3329a9f2e7969de7b7302205786c971697d9cf6e165b7d1ab9ea89c57177c9296ae82d98e724c9e941c9fb2012102d8dce330e3db29d953a579b4bddf0cf76fe264ad7e5a6454fe66fa2a6d31997bffffffff02405dc600000000001976a914f711e972a2a9c44e9d03a9c3f650c1ab74fe5ef988ace8c66900000000001976a9148efbde144197c532d5d8bf694375a7b9dcdf704188ac00000000");
	//BTCAPI::dump_tx(true, "0100000001eab24fa00eba0a2a9fd7247485c7880de28b6b59d577b29022af4a011c728f6b010000006a47304402204ae1e0bce8bf142d454ec611f7b1db3d5c18b6ef7ec4886c116b12ef3842349402202d203f5c5b79b86200b639f98a86c55b17c1e1d712bbc59ddf0d8989b30cb68c01210259ac45c55394f2b30daa6db957984f643739a31bf739f736e524cbd35aec4c26ffffffff02002d3101000000001976a9142f57034b6ee54315a481f13e232a1033d42ca7ed88ac00dd8e00000000001976a91406ef16276bdae552ffb5a175a02351e336a1926288ac00000000");

	//GetWalletBalance();
	//test_usdt_sign();

	sp_test_sign();
}