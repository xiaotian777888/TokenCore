#ifndef ELECTRUM_H
#define ELECTRUM_H

#include <string>

// str_language options: 'en', 'es', 'fr', 'it', 'ja', 'cs', 'ru', 'uk', 'zh_Hans', 'zh_Hant' and 'any', defaults to 'en'.
// str_prefixes options: "standard", "witness", "dual", "any"
// ���ӵ����Ǵ�
std::string mnemonic_new(const std::string& str_entropy,
                         const std::string& str_language = "en",
                         const std::string& str_prefixes = "standard");

// ���Ǵʵ���˽Կ
std::string mnemonic_to_master_prikey(const std::string& str_mnemonic,
                             const std::string& str_language = "en",
                             const std::string& str_prefixes = "standard");

#endif // ELECTRUM_H
