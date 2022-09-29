#ifndef EOS_KEY_ENCODE_H
#define EOS_KEY_ENCODE_H

#include <TokenCommon.h>

class eos_key
{
public:
    explicit eos_key();
    explicit eos_key(const Binary& priKey);

    Binary get_private_key() const
    {
        return private_key;
    }

	Binary get_public_key() const
    {
        return public_key;
    }

    string get_wif_private_key() const
    {
        return wif_private_key;
    }

    string get_eos_public_key() const
    {
        return eos_public_key;
    }

    static Binary get_public_key_by_pri(const Binary &pri);
    static string get_eos_public_key_by_wif(const string &wif);
    static Binary get_private_key_by_wif(const string& wif);
    static Binary get_public_key_char(const string &eoskey);
    static Binary get_public_key_by_eos_pub(const string& pub);

private:
    void _init_with_binary(const Binary& priKey);

private:
    string wif_private_key;
    string eos_public_key;

	Binary private_key;
	Binary public_key;
};

#endif // EOS_KEY_ENCODE_H
