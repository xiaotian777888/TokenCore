#include <QtCore/QCoreApplication>

#include "TokenCommon.h"

QString language("zh-CN");

extern void U256Test();
extern void CosTest();
extern void BtxonAPITest();
extern void BTCTest();
extern void BHPTest();
extern void EosTest();
extern void EthTest();
extern void TronTest();
extern void BumoTest();

// 0xAB8582E91e2387E2CA67542B26E67FdD245e0f0b ETH µØÖ·

static void test_seed()
{
	printf("seed 128: %s\n", gen_seed(128).c_str());
	printf("seed 256: %s\n", gen_seed(256).c_str());
}

static void test_bip44_get_private_key()
{
	string btc = bip44_get_private_key("89f31b00fa6996fc18201cb6adb39791a44c15792aaf34a88c11a26f46cc12c3919011f5ec0d3287e30f7b95294f809014a9daafd97b57933292cd845285673d", "m/44'/0'/0'/0/0");
	VF("bip44_get_private_key1", btc == "747d9255d3fba7c99d31afe4976049d7a6881a53c2329c47d2c0a3ed87e2d214");

	string eos = bip44_get_private_key("89f31b00fa6996fc18201cb6adb39791a44c15792aaf34a88c11a26f46cc12c3919011f5ec0d3287e30f7b95294f809014a9daafd97b57933292cd845285673d", "m/44'/194'/0'/0/x");
	VF("bip44_get_private_key2", eos == "7cf14e8495241ec3a83f10a16e6c0e20b236677a62605b7fe7067884751d16ab");

	string a1 = bip44_get_private_key("a262d6fb6122ecf45be09c50492b31f92e9beb7d9a845987a02cefda57a15f9c467a17872029a9e92299b5cbdf306e3a0ee620245cbd508959b6cb7ca637bd55", "m/44'/0'/0'/0/0");
	VF("bip44_get_private_key2", a1 == "7cf14e8495241ec3a83f10a16e6c0e20b236677a62605b7fe7067884751d16ab");
}

static void test_entropy_to_mnemonic()
{
	{
		string mnemonic = entropy_to_mnemonic("00000000000000000000000000000000");
		VF("entropy_to_mnemonic", mnemonic == "abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon about");
	}

	{
		string mnemonic = entropy_to_mnemonic("7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f");
		VF("entropy_to_mnemonic", mnemonic == "legal winner thank year wave sausage worth useful legal winner thank yellow");
	}

	{
		string mnemonic = entropy_to_mnemonic("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
		VF("entropy_to_mnemonic", mnemonic == "zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo vote");
	}

	{
		string mnemonic = entropy_to_mnemonic("f585c11aec520db57dd353c69554b21a89b20fb0650966fa0a9d6f74fd989d8f");
		VF("entropy_to_mnemonic", mnemonic == "void come effort suffer camp survey warrior heavy shoot primary clutch crush open amazing screen patrol group space point ten exist slush involve unfold");
	}
}

static void test_mnemonic_to_seed()
{
	{
		string mnemonic = entropy_to_mnemonic("f585c11aec520db57dd353c69554b21a89b20fb0650966fa0a9d6f74fd989d8f");
		string seed = mnemonic_to_seed(mnemonic, "");
		VF("mnemonic_to_seed", seed == "b873212f885ccffbf4692afcb84bc2e55886de2dfa07d90f5c3c239abc31c0a6ce047e30fd8bf6a281e71389aa82d73df74c7bbfb3b06b4639a5cee775cccd3c");
	}

	{
		string mnemonic = entropy_to_mnemonic("f585c11aec520db57dd353c69554b21a89b20fb0650966fa0a9d6f74fd989d8f");
		string seed = mnemonic_to_seed(mnemonic, "TREZOR");
		VF("mnemonic_to_seed", seed == "01f5bced59dec48e362f2c45b5de68b9fd6c92c6634f44d6d40aab69056506f0e35524a518034ddc1192e1dacd32c1ed3eaa3c3b131c88ed8e7e54c49a5d0998");
	}

	{
		string mnemonic = entropy_to_mnemonic("00000000000000000000000000000000");
		string seed = mnemonic_to_seed(mnemonic, "TREZOR");
		VF("mnemonic_to_seed", seed == "c55257c360c07c72029aebc1b53c05ed0362ada38ead3e3e9efa3708e53495531f09a6987599d18264c1e1c92f2cf141630c7a3c4ab7c81b2f001698e7463b04");
	}

	{
		// ÕÅÑïµÄ±Ò¶Ü
		string seed = mnemonic_to_seed("blue submit hurt base spray learn permit two absurd brown large extend awkward cool hair resist quarter fever brave sight palm argue adapt slush", "");
		VF("mnemonic_to_seed", seed == "a23af240b23010918472b3a7918899950f22e313168665839e2381618972cf572e36fbda9f4fbc7260193d522abb06a6612d879f756567c44d02c7687d2259e1");
	}
}

static void check_mnemonic()
{
	string mnemonic = entropy_to_mnemonic("f585c11aec520db57dd353c69554b21a89b20fb0650966fa0a9d6f74fd989d8f");
	VF("check_mnemonic", check_mnemonic(mnemonic));
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	//test_seed();
	//test_bip44_get_private_key();
	//test_entropy_to_mnemonic();
	//test_mnemonic_to_seed();
	//check_mnemonic();

	//U256Test();
	//CosTest();
	//BtxonAPITest();
	BTCTest();
	//BHPTest();
	//EthTest();
	//EosTest();
	//TronTest();
	//BumoTest();

	return 0;
}
