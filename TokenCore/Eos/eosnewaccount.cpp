#include "eosnewaccount.h"
#include "eosbytewriter.h"

EOSNewAccount::EOSNewAccount()
{

}

EOSNewAccount::EOSNewAccount(const string &creator, const string &newName, const TypeAuthority &owner, const TypeAuthority &active, const TypeAuthority &recovery)
{
    this->creator = TypeName(creator);
    this->newName = TypeName(newName);
    this->owner = owner;
    this->active = active;
    this->recovery = recovery;
}

EOSNewAccount::EOSNewAccount(const string &creator, const string &newName, const string &owner, const string &active, const string &recovery)
{
    this->creator = TypeName(creator);
    this->newName = TypeName(newName);
    this->owner = TypeAuthority(1, TypeKeyPermissionWeight(owner));
    this->active = TypeAuthority(1, TypeKeyPermissionWeight(active));
    this->recovery = TypeAuthority(1, TypeAccountPermissionWeight(recovery));
}

void EOSNewAccount::serialize(EOSByteWriter *writer) const
{
    if (writer) {
        creator.serialize(writer);
        newName.serialize(writer);
        owner.serialize(writer);
        active.serialize(writer);
        //recovery.serialize(writer);
    }
}

string EOSNewAccount::getCreatorName()
{
    return creator.to_string();
}

string EOSNewAccount::getActionName() const
{
    return "newaccount";
}

Binary EOSNewAccount::data()
{
    EOSByteWriter writer;
    serialize(&writer);

    return writer.toBytes();
}

string EOSNewAccount::dataAsHex()
{
    return Binary::encode(data());
}
