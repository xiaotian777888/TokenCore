#include "transactionheader.h"
#include "eosbytewriter.h"
#include "eosapi_utils.h"

#include <time.h>

// #include <QDateTime>
// #include <QVariant>

TransactionHeader::TransactionHeader()
{
    ref_block_num = 0U;
    ref_block_prefix = 0UL;
    max_net_usage_words = 0UL;
    max_cpu_usage_ms = 0;
    delay_seconds = 0UL;
    is_fromJson = false;
}

void TransactionHeader::serialize(EOSByteWriter *writer) const
{
   if (writer) {
//        tm tm_;
//        int year, month, day, hour, minute, second, msecond;
//        // 2018-08-16T11:04:01.000
//        sscanf_s(expiration.c_str(), "%d-%d-%dT%d:%d:%d.%d", &year, &month, &day, &hour, &minute, &second, &msecond);
//        tm_.tm_year = year - 1900;
//        tm_.tm_mon = month - 1;
//        tm_.tm_mday = day;
//        tm_.tm_hour = hour;
//        tm_.tm_min = minute;
//        tm_.tm_sec = second;
//        tm_.tm_isdst = 0;
// 
//        time_t t_ = mktime(&tm_);
// //        tm gmt;
// //        gmtime_s(&gmt, &t_);
// // 
// //        t_ = mktime(&gmt);
// 
// 
// 
// //        QDateTime date = QDateTime::fromString(QString::fromStdString(expiration), Qt::ISODate);
// //        writer->putIntLE((int)(date.toMSecsSinceEpoch() / 1000 + date.offsetFromUtc() + EXPIRATION_SEC));
// 
//       t_ += EXPIRATION_SEC;
// //       t_ += EXPIRATION_SEC + 8 * 60 * 60;
//       printf("serial date time: %ld\n", (long)t_);

//       time_t t_ = expiration + EXPIRATION_SEC;
       unsigned long t_ = 0;
       if(is_fromJson)
           t_ = expiration + 8*60*60;
       else
           t_ = expiration + EXPIRATION_SEC + 8*60*60;
       printf("serial date time: %ld\n", t_);

       writer->putIntLE((int)t_);

       writer->putShortLE((short)ref_block_num & 0xFFFF);
       writer->putIntLE((int)(ref_block_prefix & 0xFFFFFFFF));
       writer->putVariableUInt(max_net_usage_words);
       writer->putVariableUInt(max_cpu_usage_ms);
       writer->putVariableUInt(delay_seconds);
   }
}

// QJsonValue TransactionHeader::toJson() const
// {
//     QJsonObject obj;
//     QDateTime date = QDateTime::fromString(QString::fromStdString(expiration), Qt::ISODate);
//     date = date.addSecs(EXPIRATION_SEC+ date.offsetFromUtc());
// 
//     obj.insert("expiration", QJsonValue(date.toString(Qt::ISODate)));
//     obj.insert("ref_block_num", QJsonValue((int)ref_block_num));
//     obj.insert("ref_block_prefix", QJsonValue::fromVariant(QVariant((uint)ref_block_prefix)));
//     obj.insert("max_net_usage_words", QJsonValue((int)max_net_usage_words));
//     obj.insert("max_cpu_usage_ms", QJsonValue((int)max_cpu_usage_ms));
//     obj.insert("delay_sec", QJsonValue((int)delay_seconds));
// 
//     return QJsonValue(obj);
// }
Json::Value TransactionHeader::toJson() const
{
//    QJsonObject obj;
//    QDateTime date = QDateTime::fromString(QString::fromStdString(expiration), Qt::ISODate);
//    date = date.addSecs(EXPIRATION_SEC+ date.offsetFromUtc());

//    obj.insert("expiration", QJsonValue(date.toString(Qt::ISODate)));
//     obj.insert("ref_block_num", QJsonValue((int)ref_block_num));
//     obj.insert("ref_block_prefix", QJsonValue::fromVariant(QVariant((uint)ref_block_prefix)));
//     obj.insert("max_net_usage_words", QJsonValue((int)max_net_usage_words));
//     obj.insert("max_cpu_usage_ms", QJsonValue((int)max_cpu_usage_ms));
//     obj.insert("delay_sec", QJsonValue((int)delay_seconds));
//    return QJsonValue(obj);

    time_t t_ = expiration + EXPIRATION_SEC;
    //t_ += EXPIRATION_SEC;
    tm tm_;
#ifdef WIN32
    localtime_s(&tm_, &t_);
#else
    localtime_r(&t_, &tm_);
#endif //WIN32
    char szTime[128] = { 0 };
    sprintf_s(szTime, 127, "%04d-%02d-%02dT%02d:%02d:%02d", tm_.tm_year + 1900, tm_.tm_mon + 1, tm_.tm_mday, tm_.tm_hour, tm_.tm_min, tm_.tm_sec);
    //printf("toJson: %s\n", szTime);

    Json::Value obj;
    obj["expiration"] = szTime;
    obj["ref_block_num"] = ref_block_num;
    obj["ref_block_prefix"] = (unsigned int)ref_block_prefix;
    obj["max_net_usage_words"] = (unsigned int)max_net_usage_words;
    obj["max_cpu_usage_ms"] = max_cpu_usage_ms;
    obj["delay_sec"] = (unsigned int)delay_seconds;

    return obj;
}

// void TransactionHeader::fromJson(const QJsonValue &value)
// {
//     QJsonObject obj = value.toObject();
//     if (obj.isEmpty()) {
//         return;
//     }
// 
//     expiration = obj.value("expiration").toString().toStdString();
//     ref_block_num = obj.value("ref_block_num").toInt();
//     ref_block_prefix = obj.value("ref_block_prefix").toVariant().toUInt();
//     max_net_usage_words = obj.value("max_net_usage_words").toInt();
//     max_cpu_usage_ms = obj.value("max_cpu_usage_ms").toInt();
//     delay_seconds = obj.value("delay_sec").toInt();
// }

void TransactionHeader::fromJson(const Json::Value& value)
{
//     QJsonObject obj = value.toObject();
//     if (obj.isEmpty()) {
//         return;
//     }

    is_fromJson = true;
    std::string _expiration = value["expiration"].asString();
    setExpiration(_expiration);
    //expiration = obj.value("expiration").toString().toStdString();

    ref_block_num = value["ref_block_num"].asInt();
    ref_block_prefix = value["ref_block_prefix"].asUInt();
    max_net_usage_words = value["max_net_usage_words"].asInt();
    max_cpu_usage_ms = value["max_cpu_usage_ms"].asInt();
    delay_seconds = value["delay_sec"].asInt();
}

void TransactionHeader::setExpiration(const std::string &expiration)
{
    //this->expiration = expiration;

    tm tm_;
    int year, month, day, hour, minute, second, msecond;
    // e.g.: 2018-08-16T11:04:01.000
    sscanf(expiration.c_str(), "%d-%d-%dT%d:%d:%d.%d", &year, &month, &day, &hour, &minute, &second, &msecond);
    tm_.tm_year = year - 1900;
    tm_.tm_mon = month - 1;
    tm_.tm_mday = day;
    tm_.tm_hour = hour;
    tm_.tm_min = minute;
    tm_.tm_sec = second;
    tm_.tm_isdst = 0;

    //time_t t_ = mktime(&tm_);
    this->expiration = (u32)mktime(&tm_);
}

void TransactionHeader::setReferenceBlock(const std::string &ref)
{
//     QString ref_block = QString::fromStdString(ref);
//     ref_block_num = ref_block.left(8).toInt(nullptr, 16);
//     ref_block_prefix = Utils::convertEndian(ref_block.mid(16, 8).toUInt(nullptr, 16));

    std::string str_block_num;
    str_block_num.assign(ref, 4, 4);

    std::string str_block_prefix;
    str_block_prefix.assign(ref, 16, 8);

    bx_hex2bin(str_block_num.c_str(), (unsigned char*)&ref_block_num, 2);
    bx_memrevert((unsigned char*)&ref_block_num, sizeof(ref_block_num));

    bx_hex2bin(str_block_prefix.c_str(), (unsigned char*)&ref_block_prefix, 4);
//     bx_hex2bin(str_block_prefix.c_str(), (unsigned char*)&ref_block_prefix + 4, 4);
//     bx_memrevert((unsigned char*)&ref_block_prefix, 8);

//    printf("ref: %s\n", ref.c_str());
//    printf("str_block_num: %s\n", str_block_num.c_str());
//    printf("ref_block_num: 0x%04x (%d)\n", ref_block_num, ref_block_num);
//    printf("str_block_prefix: %s\n", str_block_num.c_str());
//    printf("ref_block_prefix: 0x%08x (%d)\n", ref_block_prefix, ref_block_prefix);
}

void TransactionHeader::setReferenceBlock(u32 _ref_block_num, u32 _ref_block_prefix) {
    ref_block_num = (u16)(_ref_block_num & 0xffff);
    ref_block_prefix = (u32)(_ref_block_prefix & 0xffffffff);
}


void TransactionHeader::setNetUsageWords(s64 net_usage)
{
    max_net_usage_words = (unsigned long)net_usage;
}

void TransactionHeader::setKcpuUsage(s64 kcpu)
{
    max_cpu_usage_ms = (unsigned char)kcpu;
}
