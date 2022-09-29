#include "HttpAPI\BtxonAPI.h"

// 这是测试环境的接口
// http://test.btxon.com/explorer/

int GetUtxo(const CoinType& coinType, char* address)
{
	BtxonAPI api;
	vector<Utxo> utxo_list;
	int ret = api.getUTXO(coinType, address, utxo_list);
	if (ret == 0)
	{
		printf("points\n{\n");
		for (int i = 0; i < utxo_list.size(); i++)
		{
			printf("\tpoint\n");
			printf("\t{\n");
			printf("\t\thash %s\n", utxo_list[i].hash.c_str());
			printf("\t\tindex %u\n", utxo_list[i].index);
			printf("\t\tvalue %I64u\n", utxo_list[i].value);
			printf("\t\tscript %s\n", utxo_list[i].script.c_str());
			printf("\t}\n");
		}
		printf("}\n");
	}
	return ret;
}

void TestGetTxList()
{
	BtxonAPI api;
	// 1GCH1cifoJ7bbYV2ApGEGzrLV2jULa3N8v
	string wallet_address("muMCrdMHhtW21wUKfqBg8zWPSJV6eG9tjT");

	vector<TransactionNet> transaction_list;
	int ret = api.getTxList(CT_BTC_TEST, wallet_address, transaction_list);
	if (ret == 0)
	{
		printf("{\n");
		for (int i = 0; i < transaction_list.size(); i++)
		{
			printf("\t{\n");
			printf("\t\ttransaction_hash=%s\n", transaction_list[i].hash.c_str());
			printf("\t\tfrom_address=%s\n", transaction_list[i].from_address.c_str());
			printf("\t\tto_address=%s\n", transaction_list[i].to_address.c_str());
			printf("\t\tvalue=%s\n", transaction_list[i].value.str().c_str());
			printf("\t\tfee=%s\n", transaction_list[i].fee.str().c_str());
			printf("\t\tscript_pub_key=%s\n", transaction_list[i].script.c_str());
			printf("\t\tspent_index=%d\n", transaction_list[i].spent_index);
			printf("\t\tstatus=%d\n", transaction_list[i].status);
			printf("\t\tcreated_at=%d\n", transaction_list[i].created);
			printf("\t}\n");
		}
		printf("}\n");
	}
}

void TestGetScript()
{
	BtxonAPI api;
	string script;
	int ret = api.getScript("dc827b257071cc1e42a9dd967345ad62afad1d442089217f6572bfad2ae5f02a", script);
	if (ret == 0)
	{
		printf("%s\n", script.c_str());
	}
}

void BtxonAPITest()
{
	GetUtxo(CT_BTC_TEST, "mg9cmEEV7GB7NfsXPqc9yUvUjYH9EMUsuP");
	TestGetTxList();
	TestGetScript();

	BtxonAPI api;
	double price = 0;
	int ret = api.getPrice(CT_USDT, CurrencyType::CNY, price);
}
