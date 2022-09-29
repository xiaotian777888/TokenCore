
#include <stdint.h>
#include <vector>
#include <map>
#include <fstream>
#include <bitset>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "mnemonic-new.h"
#include "TokenCommon.h"
#include "crypto/hash.h"
#include "crypto/sha256.h"
#include "crypto/pbkdf2.h"
#include "crypto/utility_tools.h"

using namespace std;

map<uint16_t, string> g_position_to_words;

static size_t bits_per_word = 11;
static size_t entropy_bit_divisor = 32;
static size_t hmac_iterations = 2048;
static const char* passphrase_prefix = "mnemonic";

void load_words(const std::string& str_language,
                map<uint16_t, string>& position_to_word);

uint8_t bip39_shift(size_t bit) {
    return (1 << (8 - (bit % 8) - 1));
}

std::string mnemonic_new(const std::string& str_entropy,
                         const std::string& str_language /* = "en" */,
                         const std::string& str_prefixes /* = "standard" */)
{
    // seed在128到256位之间，且必须被32整除
    if (0 != str_entropy.size() % 8)
        return string("seed must be divede by 32");
    if (str_entropy.size() < 32 || str_entropy.size() > 64)
        return string("seed length is error");

    load_words(str_language, g_position_to_words);
	Binary bin_entropy = Binary::decode(str_entropy);
	Binary bin_checksum = sha256_hash(bin_entropy).left(bin_entropy.size() / 64);
	Binary bin_mnemonic = bin_entropy + bin_checksum;

    //string str_checksum = sha256_hash(seed).substr(0, seed.size() / 32);
    //string str_mnemonic = seed + str_checksum;
    //vector<unsigned char> vec_mnemonic;

    /*for (size_t i = 0; i < str_mnemonic.size() - 1;)
    {
        string str_byte = str_mnemonic.substr(i, 2);
        uint8_t u8 = string_to_uint8_t(str_byte);
        i += 2;
        vec_mnemonic.push_back(u8);
    }

    if (0 != str_mnemonic.size() % 2)
    {
        string str_last = str_mnemonic.substr(str_mnemonic.size() - 1) + "0";
        uint8_t u8 = string_to_uint8_t(str_last);
        vec_mnemonic.push_back(u8);
    }*/

    vector<string> vec_res;
    size_t bit = 0;

    for (size_t word = 0; word < bin_mnemonic.size() * 8 / 11; word++)
    {
        size_t position = 0;
        for (size_t loop = 0; loop < bits_per_word; loop++)
        {
            bit = (word * bits_per_word + loop);
            position <<= 1;

            size_t byte = bit / 8;

            if ((bin_mnemonic[byte] & bip39_shift(bit)) > 0)
                position++;
        }

        // BITCOIN_ASSERT(position < dictionary_size);
        // words.push_back(lexicon[position]);
        // map_mnemonic.insert(pair<uint16_t, string>(position, g_map_words[position]));
        vec_res.push_back(g_position_to_words[(uint16_t)position]);
    }

    string str_words;
    for (size_t i = 0; i < vec_res.size(); i++)
    {
        if (i != vec_res.size() - 1)
            str_words = str_words + vec_res[i] + " ";
        else
            str_words = str_words + vec_res[i];
    }

    return str_words;
}

std::string mnemonic_to_master_prikey(const std::string& str_mnemonic,
                             const std::string& str_language /* = "en" */,
                             const std::string& str_prefixes /* = "standard" */)
{
    /*const std::string salt(passphrase_prefix);
    vector<unsigned char> vec_mnemonic_, vec_salt;
    copy(str_mnemonic.begin(), str_mnemonic.end(), back_inserter(vec_mnemonic_));
    copy(salt.begin(), salt.end(), back_inserter(vec_salt));

    vector<unsigned char> hash(64);
    int result = pkcs5_pbkdf2(vec_mnemonic_.data(), vec_mnemonic_.size(),
        vec_salt.data(), vec_salt.size(), hash.data(), hash.size(), hmac_iterations);
    if (result != 0)
        return string("");

    string str_res;
    encode_base16(str_res, hash);
    return str_res;*/

	return std::string("");
}

void load_words(const std::string& str_language,
                map<uint16_t, string>& position_to_word)
{
    string str_filename;
    if ("zh_Hans" == str_language)
        str_filename = "wordlist/chinese_simplified.txt";
    else if ("zh_Hant" == str_language)
        str_filename = "wordlist/chinese_traditional.txt";
    else if ("en" == str_language)
        str_filename = "wordlist/english.txt";
    else if ("fr" == str_language)
        str_filename = "wordlist/french.txt";
    else if ("it" == str_language)
        str_filename = "wordlist/italian.txt";
    else if ("ja" == str_language)
        str_filename = "wordlist/japanese.txt";
    else if ("uk" == str_language)
        str_filename = "wordlist/korean.txt";
    else if ("cs" == str_language)
        str_filename = "wordlist/spanish.txt";
    else
        return;

    ifstream fin;
    fin.open(str_filename.c_str());
    uint16_t line_num = 0;
    string str_line;
    while (getline(fin, str_line))
    {
        if ("" != str_line)
        {
            position_to_word[line_num] = str_line;
            line_num++;
        }
    }
    fin.close();
}

