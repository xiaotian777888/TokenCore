#ifndef EOSNEWACCOUNT_H
#define EOSNEWACCOUNT_H

#include "typename.h"
#include "typeauthority.h"

class EOSNewAccount : public ISerializer
{
public:
    EOSNewAccount();
    EOSNewAccount(const string& creator, const string& newName, const TypeAuthority& owner, const TypeAuthority& active, const TypeAuthority& recovery);
    EOSNewAccount(const string& creator, const string& newName, const string& owner, const string& active, const string& recovery);

    virtual void serialize(EOSByteWriter *writer) const;

    string getCreatorName();
    string getActionName() const;

	Binary data();
	string dataAsHex();

private:
    TypeName creator;
    TypeName newName;
    TypeAuthority owner;
    TypeAuthority active;
    TypeAuthority recovery;
};

#endif // EOSNEWACCOUNT_H
