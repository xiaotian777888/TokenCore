#pragma once
#include "global.h"
#include <CosBase.h>

typedef unsigned short SW;

int get_input();
void binout(unsigned char* data, int len);
int set_pin(Cos& cos);
int verify_pin(Cos& cos);

int get_wallet_address(const CoinType& coin_type, unsigned long index, char* wallet_address);
int dump_wallet_address(const CoinType& coin_type);
