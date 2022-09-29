#ifndef TRANSACTIONHEADER_H
#define TRANSACTIONHEADER_H

#include "chainbase.h"
#include <string>

class TransactionHeader : public ISerializer, public IFormater
{
public:
    TransactionHeader();
    virtual ~TransactionHeader() {}

    virtual void serialize(EOSByteWriter* writer) const;
//     virtual QJsonValue toJson() const;
//     virtual void fromJson(const QJsonValue& value);
    virtual Json::Value toJson() const;
    virtual void fromJson(const Json::Value& value);

    void setExpiration(const std::string& expiration);
    void setReferenceBlock(const std::string& ref);
    void setNetUsageWords(s64 net_usage);
    void setKcpuUsage(s64 kcpu);

    void setReferenceBlock(u32 _ref_block_num, u32 _ref_block_prefix);

private:
    //std::string expiration;
//     time_t expiration;
//     int ref_block_num;
//     s64 ref_block_prefix;
//     s64 max_net_usage_words;
//     s64 max_cpu_usage_ms;
//     s64 delay_seconds;
    u32 expiration;
    u16 ref_block_num;
    u32 ref_block_prefix;
    u32 max_net_usage_words;
    u8  max_cpu_usage_ms;
    u32 delay_seconds;

    bool is_fromJson;
    
private:
    const static int EXPIRATION_SEC = 3000;
};

#endif // TRANSACTIONHEADER_H
