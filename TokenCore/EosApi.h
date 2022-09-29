//
// Interface for EOS Coin
//

#ifndef __EOSAPI_H__
#define __EOSAPI_H__

#include "TokenCommon.h"

#ifdef BX_OS_UNIX_LIKE
#   include <sys/types.h>
#   include <string.h>
#   include <cstdlib>
#   include <stdio.h>
#   include <stdarg.h>
#   include <unistd.h>
#   ifndef ULONG
#       define ULONG unsigned int
#       define DWORD unsigned int
#       define UINT unsigned int
#       define UINT8 unsigned char
#       define USHORT unsigned short
#       define BYTE unsigned char
#   endif
#   define BXAPI
#endif // BX_OS_UNIX_LIKE

#define _in
#define _out
#define _inout

#define BXRET_OK           0    // success
#define BXRET_INTERNAL    -1    // internal error
#define BXRET_BAD_PARAM   -2    // invalid parameter
#define BXRET_BUFFER_TOO_SMALL -3  // buffer too small

typedef void *EOS_TX;

namespace EOSAPI {

bool validate_account(string account);
string get_public_key(const string &private_key);        // ��˽Կ��ȡ��Կ
string get_private_key(const string& seed);

int make_unsign_tx(
        _out EOS_TX **tx,
        _in const bool owner,
        _in const string &code,
        _in const string &action,
        _in const string &account,
        _in const string &bin_args,
        _in const string &chain_info
);
int make_unsign_tx_from_json(
        _out EOS_TX** tx,
        _in const char* chain_id,
        _in const char* json_raw
);

int sign_tx(_in EOS_TX *tx, _in const string private_key);

int add_action(
        _in EOS_TX *tx,
        _in const bool owner,
        _in const string &code,
        _in const string &action,
        _in const string &account,
        _in const string &binargs
);

void release(_in EOS_TX **tx);

Binary get_data(_in const EOS_TX *tx);

string make_sign_tx(_in const EOS_TX *tx, _in Binary &sign_data);


//=============================================================
// C Interface.
//=============================================================

/*
1. normal
{
  "owner": false,
  "code": "eosio.token",
  "action: "transfer",
  "account: "apexluna3333",
  "bin_args": "30c61866ead8553520841066ead855350c3000000000000004454f5300000000117465737420454f53207472616e73666572",
  "chain_info": "..."
}

2. from json
{
  "chain_id": ".....",
  "raw_json": {json object}
}
*/
int TxCreate(
        _in const char *json_param,
        _out EOS_TX **tx
);

/*
{
  "owner": false,
  "code": "eosio.token",
  "action: "transfer",
  "account: "apexluna3333",
  "bin_args": "30c61866ead8553520841066ead855350c3000000000000004454f5300000000117465737420454f53207472616e73666572",
  "chain_info": "..."
}
*/
int TxAddAction(
        _in EOS_TX *tx,
        _in const char *json_param
);

/*
{
  "key": "11eadf3ae12028ddabc765b16cab6e622080dacf69e0f1eae81ba8ceba5fff15"
}
*/
int TxSign(
        _in EOS_TX *tx,
        _in const char *json_param
);

int TxGetTransacton(
        _in const EOS_TX *tx,
        _in const char *json_param,
        _out char **result
);

/*
�������Ӽ���HD˽Կ
{
  "seed": "3423423421231424"
}
*/
int TxGetPriKey(
        _in const char *json_param,
        _out char **result
);

int TxGetPubKey(
        _in const char *wifPriKey,
        _out char **result
);

int TxLongToName(
        _in long val,
        _out char **result
);

string sign_eos_message(
        _in string str_msg_json,
        _in string str_prikey
//        _out char **result
);

} // end of namespace.

#endif // __BDAPI_H__
