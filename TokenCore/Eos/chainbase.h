#ifndef CHAINBASE_H
#define CHAINBASE_H

#ifndef s64
#ifndef _WIN64
//#define LONG long long
#   define s64 long long
#else
//#LONG long
#   define s64 __int64
#endif
#endif

// #include <QJsonDocument>
// #include <QJsonObject>
// #include <QJsonArray>
// #include <QJsonValue>

#include <string>
#include <json/json.h>
#include "eosapi_utils.h"

class EOSByteWriter;

static const std::string EOS_SYSTEM_ACCOUNT = "eosio";

class ISerializer
{
public:
    virtual void serialize(EOSByteWriter* writer) const = 0;
};

class IFormater
{
public:
//     virtual QJsonValue toJson() const = 0;
//     virtual void fromJson(const QJsonValue& value) = 0;
    virtual Json::Value toJson() const = 0;
};

#endif // CHAINBASE_H
