#ifndef BHPBASE_H
#define BHPBASE_H

#include "TokenCommon.h"

namespace BHPAPI
{
	string get_private_key(const string& seed);
	string get_public_key(const string& private_key);
	string get_address(const string& public_key);
	bool validate_address(const string& str_address);
	int make_unsign_tx(UserTransaction* ut);
	void sign_tx(UserTransaction* ut, const string& private_key);
	void make_sign_tx(UserTransaction* ut);
	string tx_hash(const string& unsign_tx);

	Binary firmware_prepare_data(UserTransaction* ut);
	void firmware_process_result(UserTransaction* ut, char* result, int result_size);
	int get_tx_len(UserTransaction* ut);
	void dump_tx(const string& tx_str);
}

#endif // BHPBASE_H
