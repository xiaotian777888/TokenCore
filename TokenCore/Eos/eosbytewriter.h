#ifndef EOSBYTEWRITER_H
#define EOSBYTEWRITER_H

#include <string>
#include <vector>
#include <string.h>

#include "chainbase.h"

// class QJsonObject;
// class QJsonArray;

class EOSByteWriter
{
public:
    EOSByteWriter();
    EOSByteWriter(int capacity);
    EOSByteWriter(const Binary& buf);
    ~EOSByteWriter();

    void put(unsigned char b);
    void putShortLE(short value);
    void putIntLE(int value);
    void putLongLE(s64 value);
    void putBytes(const unsigned char *value, int len);
    void putBytes(const unsigned char *value, int offset, int len);
    void putString(const string& str);
    void putVariableUInt(s64 val);

	Binary toBytes();
    int length();

private:
	Binary buf;
};

// gcc do not support template which is a member, use this.
template<typename T>
void SerializeCollection(const vector<T>& list, EOSByteWriter *writer)
{
    if (writer) {
        writer->putVariableUInt(list.size());
        for (auto item : list) {
            item.serialize(writer);
        }
    }
}

#endif // EOSBYTEWRITER_H
