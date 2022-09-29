#include "Coin.h"

Coin _COIN_UNKNOW("UNKNOWN", QT_TRANSLATE_NOOP_UTF8("Coin", "不存在"), "", 0, CT_UNKNOW);

map<QString, Coin> gCoin =
{
	{ "BTC", { "BTC", QT_TRANSLATE_NOOP_UTF8("Coin", "比特币"), "", 8, CT_BTC } },
	{ "tBTC", { "BTC", QT_TRANSLATE_NOOP_UTF8("Coin", "比特币-测试网"), "", 8, CT_BTC_TEST } },
	{ "USDT", { "USDT", QT_TRANSLATE_NOOP_UTF8("Coin", "泰达币"), "", 8, CT_USDT } },
	{ "tUSDT", { "USDT", QT_TRANSLATE_NOOP_UTF8("Coin", "泰达币-测试网"), "", 8, CT_USDT_TEST } },
	{ "ETH", { "ETH", QT_TRANSLATE_NOOP_UTF8("Coin", "以太币"), "", 18, CT_ETH } },
	{ "tETH", { "ETH", QT_TRANSLATE_NOOP_UTF8("Coin", "以太币-测试网3"), "", 18, CT_ETH_TEST3 } },
	{ "ETH-AE", { "AE", QT_TRANSLATE_NOOP_UTF8("Coin", "Aeternity"), "0x5ca9a71b1d01849c0a95490cc00559717fcf0d1d", 18, CT_ETH_AE } },
	{ "ETH-BAT", { "BAT", QT_TRANSLATE_NOOP_UTF8("Coin", "注意力币"), "0x0d8775f648430679a709e98d2b0cb6250d2887ef", 18, CT_ETH_BAT } },
	{ "ETH-BTM", { "BTM", QT_TRANSLATE_NOOP_UTF8("Coin", "比原币"), "0xcb97e65f07da24d46bcdd078ebebd7c6e6e3d750", 8, CT_ETH_BTM } },
	{ "ETH-HT", { "HT", QT_TRANSLATE_NOOP_UTF8("Coin", "火币"), "0x6f259637dcd74c767781e37bc6133cd6a68aa161", 18, CT_ETH_HT } },
	{ "ETH-OMG", { "OMG", QT_TRANSLATE_NOOP_UTF8("Coin", "OmiseGo"), "0xd26114cd6EE289AccF82350c8d8487fedB8A0C07", 18, CT_ETH_OMG } },
	{ "ETH-NAS", { "NAS", QT_TRANSLATE_NOOP_UTF8("Coin", "星云链"), "0x5d65D971895Edc438f465c17DB6992698a52318D", 18, CT_ETH_NAS } },
	{ "ETH-ELF", { "ELF", QT_TRANSLATE_NOOP_UTF8("Coin", "aelf"), "0xbf2179859fc6d5bee9bf9158632dc51678a4100e", 18, CT_ETH_ELF } },
	{ "ETH-ZIL", { "ZIL", QT_TRANSLATE_NOOP_UTF8("Coin", "Zilliqa"), "0x05f4a42e251f2d52b8ed15e9fedaacfcef1fad27", 12, CT_ETH_ZIL } },
	{ "ETH-BLZ", { "BLZ", QT_TRANSLATE_NOOP_UTF8("Coin", "Bluzelle"), "0x5732046a883704404f284ce41ffadd5b007fd668", 18, CT_ETH_BLZ } },
	{ "ETH-VEN", { "VEN", QT_TRANSLATE_NOOP_UTF8("Coin", "唯链"), "0xd850942ef8811f2a866692a623011bde52a462c1", 18, CT_ETH_VEN } },
	{ "ETH-THETA", { "THETA", QT_TRANSLATE_NOOP_UTF8("Coin", "Theta token"), "0x3883f5e181fccaf8410fa61e12b59bad963fb645", 18, CT_ETH_THETA } },
	{ "ETH-ZRX", { "ZRX", QT_TRANSLATE_NOOP_UTF8("Coin", "0x协议"), "0xe41d2489571d322189246dafa5ebde1f4699f498", 18, CT_ETH_ZRX } },
	{ "ETH-BNB", { "BNB", QT_TRANSLATE_NOOP_UTF8("Coin", "币安币"), "0xB8c77482e45F1F44dE1745F52C74426C631bDD52", 18, CT_ETH_BNB } },
	{ "ETH-ICX", { "ICX", QT_TRANSLATE_NOOP_UTF8("Coin", "ICON"), "0xb5a5f22694352c15b00323844ad545abb2b11028", 18, CT_ETH_ICX } },
	{ "ETH-WAX", { "WAX", QT_TRANSLATE_NOOP_UTF8("Coin", "WAX Token"), "0x39bb259f66e1c59d5abef88375979b4d20d98022", 8, CT_ETH_WAX } },
	{ "ETH-POLY", { "POLY", QT_TRANSLATE_NOOP_UTF8("Coin", "Polymath"), "0x9992ec3cf6a55b00978cddf2b27bc6882d88d1ec", 18, CT_ETH_POLY } },
	{ "ETH-LBA", { "LBA", QT_TRANSLATE_NOOP_UTF8("Coin", "Libra Credit"), "0xfe5f141bf94fe84bc28ded0ab966c16b17490657", 18, CT_ETH_LBA } },
	{ "ETH-KNC", { "KNC", QT_TRANSLATE_NOOP_UTF8("Coin", "开博网络"), "0xdd974d5c2e2928dea5f71b9825b8b646686bd200", 18, CT_ETH_KNC } },
	{ "ETH-RUFF", { "RUFF", QT_TRANSLATE_NOOP_UTF8("Coin", "RUFF"), "0xf278c1ca969095ffddded020290cf8b5c424ace2", 18, CT_ETH_RUFF } },
	{ "ETH-CMT", { "CMT", QT_TRANSLATE_NOOP_UTF8("Coin", "草莓糖"), "0xf85feea2fdd81d51177f6b8f35f0e6734ce45f5f", 18, CT_ETH_CMT } },
	{ "ETH-WTC", { "WTC", QT_TRANSLATE_NOOP_UTF8("Coin", "沃尔顿链"), "0xb7cb1c96db6b22b0d3d9536e0108d062bd488f74", 18, CT_ETH_WTC } },
	{ "ETH-QSP", { "QSP", QT_TRANSLATE_NOOP_UTF8("Coin", "Quantstamp"), "0x99ea4db9ee77acd40b119bd1dc4e33e1c070b80d", 18, CT_ETH_QSP } },
	{ "ETH-SNT", { "SNT", QT_TRANSLATE_NOOP_UTF8("Coin", "StatusNetwork"), "0x744d70fdbe2ba4cf95131626614a1763df805b9e", 18, CT_ETH_SNT } },
	{ "ETH-RRC", { "RRC", QT_TRANSLATE_NOOP_UTF8("Coin", "RRChain"), "0xb6259685685235c1ef4b8529e7105f00bd42b9f8", 2, CT_ETH_RRC } },
	{ "ETH-GUSD", { "GUSD", QT_TRANSLATE_NOOP_UTF8("Coin", "Gemini Dollar"), "0x056fd409e1d7a124bd7017459dfea2f387b6d5cd", 2, CT_ETH_GUSD } },
	{ "ETH-PLY", { "PLY", QT_TRANSLATE_NOOP_UTF8("Coin", "PlayCoin"), "0x59be937f05cf2c406b61c42c6c82a093fa54edfe", 9, CT_ETH_PLY } },
	{ "ETH-LRC", { "LRC", QT_TRANSLATE_NOOP_UTF8("Coin", "Loopring"), "0xef68e7c694f40c8202821edf525de3782458639f", 18, CT_ETH_LRC } },
	{ "ETH-POWR", { "POWR", QT_TRANSLATE_NOOP_UTF8("Coin", "PowerLedger"), "0x595832f8fc6bf59c85c527fec3740a1b7a361269", 6, CT_ETH_POWR } },
	{ "ETH-RNT", { "RNT", QT_TRANSLATE_NOOP_UTF8("Coin", "OneRoot"), "0xff603f43946a3a28df5e6a73172555d8c8b02386", 18, CT_ETH_RNT } },
	{ "ETH-STORM", { "STORM", QT_TRANSLATE_NOOP_UTF8("Coin", "Storm"), "0xd0a4b8946cb52f0661273bfbc6fd0e0c75fc6433", 18, CT_ETH_STORM } },
	{ "ETH-OST", { "OST", QT_TRANSLATE_NOOP_UTF8("Coin", "Simple Token"), "0x2c4e8f2d746113d0696ce89b35f0d8bf88e0aeca", 18, CT_ETH_OST } },
	{ "ETH-DELC", { "DELC", QT_TRANSLATE_NOOP_UTF8("Coin", "RelationPerson"), "0x51c1a88a007411ca7af351218ea6448ed8f381c1", 18, CT_ETH_DELC } },
	{ "ETH-GGC", { "GGC", QT_TRANSLATE_NOOP_UTF8("Coin", "GlobalGoldCash"), "0x66a9250A5075F3B4d5B4b2E500Fa57888A5e10b7", 18, CT_ETH_GGC } },
	{ "ETH-GGT", { "GGT", QT_TRANSLATE_NOOP_UTF8("Coin", "GlobalGoldToken"), "0x1d72E76e38C815B9F91661c340949E8673e897b3", 18, CT_ETH_GGT } },
	{ "EOS", { "EOS", QT_TRANSLATE_NOOP_UTF8("Coin", "EOS币"), "", 4, CT_EOS } },
	{ "tEOS", { "EOS", QT_TRANSLATE_NOOP_UTF8("Coin", "EOS币-测试网"), "", 4, CT_EOS_TEST } },
	{ "BHP", { "BHP", QT_TRANSLATE_NOOP_UTF8("Coin", "算力币"), "", 8, CT_BHP } },
	{ "tBHP", { "BHP", QT_TRANSLATE_NOOP_UTF8("Coin", "算力币-测试网"), "", 8, CT_BHP_TEST } },
	{ "TRX", { "TRX", QT_TRANSLATE_NOOP_UTF8("Coin", "波场币"), "", 6, CT_TRX } },
	{ "tTRX", { "TRX", QT_TRANSLATE_NOOP_UTF8("Coin", "波场币-测试网"), "", 6, CT_TRX_TEST } },
};

Coin& GetCoinByType(const CoinType& type)
{
	for (auto& it : gCoin)
	{
		if (it.second.type == type)
			return it.second;
	}
	return _COIN_UNKNOW;
}

QString GetCoinIDByType(const CoinType& type)
{
	for (auto& it : gCoin)
	{
		if (it.second.type == type)
			return it.first;
	}
	return QString("");
}
