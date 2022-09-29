#include "typename.h"
#include "eosbytewriter.h"

// #include <QRegularExpression>

static const char char_map[] = ".12345abcdefghijklmnopqrstuvwxyz";

TypeName::TypeName()
{
    value = 0;
}

TypeName::TypeName(const std::string &name)
{
    value = name_string_to_long(name);
}

void TypeName::serialize(EOSByteWriter *writer) const
{
    if (writer) {
        writer->putLongLE(value);
    }
}

Json::Value TypeName::toJson() const
{
    return Json::Value(to_string());
}
// 
// void TypeName::fromJson(const QJsonValue &value)
// {
//     this->value = name_string_to_long(value.toString().toStdString());
// }

void TypeName::fromJson(const Json::Value& value)
{
    this->value = name_string_to_long(value.asCString());
}

std::string TypeName::to_string() const
{
    return long_name_to_string(value);
}

s64 TypeName::name_string_to_long(const std::string &name)
{
    if (name.empty()) {
        return 0L;
    }

    int len = (int)name.length();
    s64 value = 0;

    for (int i = 0; i <= MAX_NAME_IDX; ++i) {
        s64 c = 0;

        if (i < len && i <= MAX_NAME_IDX) {
            c = char_to_symbol(name.at(i));
        }

        if (i < MAX_NAME_IDX) {
            c &= 0x1f;
            c <<= 64 - 5 * (i + 1);
        } else {
            c &= 0x0f;
        }

        value |= c;
    }

    return value;
}

std::string TypeName::long_name_to_string(s64 val)
{
    std::vector<char> vec(MAX_NAME_IDX+1, '\0');
    s64 temp = val;
    for (int i = 0; i <= MAX_NAME_IDX; ++i) {
        char c = char_map[(int)(temp & (i == 0 ? 0x0f : 0x1f))];
        vec[MAX_NAME_IDX - i] = c;
        temp >>= (i == 0 ? 4 : 5);
    }

//     QString strRaw = QString::fromStdString(std::string(vec.begin(), vec.end()));
//     strRaw = strRaw.replace(QRegularExpression("[.]+$"), "");
//     return strRaw.toStdString();

    std::string strRaw = std::string(vec.begin(), vec.end());
    std::string::size_type pos = strRaw.find_last_not_of('.');
    if (pos < strRaw.length() - 1)
        strRaw.erase(pos + 1, strRaw.length() - pos - 1);

//    printf("long_name_to_string: %s\n", strRaw.c_str());
    return strRaw;
}

unsigned char TypeName::char_to_symbol(char c)
{
    if (c >= 'a' && c <= 'z') {
        return (unsigned char)((c - 'a') + 6);
    }

    if (c >= '1' && c <= '5') {
        return (unsigned char)((c - '1') + 1);
    }

    return (unsigned char)0;
}
