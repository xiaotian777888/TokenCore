#include "action.h"
#include "eosbytewriter.h"

// #include <QStringList>

Action::Action()
{

}

Action::Action(const TypeName &account, const TypeName &action, const TypePermissionLevel &auth, const string &data)
{
    this->account = account;
    this->account = action;
    this->authorization.push_back(auth);
    this->data = data;
}

Action::Action(const TypeName &account, const TypeName &action)
{
    this->account = account;
    this->action = action;
}

void Action::serialize(EOSByteWriter *writer) const
{
    if (writer) {
        account.serialize(writer);
        action.serialize(writer);

        SerializeCollection<TypePermissionLevel>(authorization, writer);
        //writer->putCollection(authorization);

        Binary bytes = Binary::decode(data);

        if (!bytes.empty()) {
            writer->putVariableUInt(bytes.size());
            writer->putBytes(bytes.data(), (int)bytes.size());
        } else {
            writer->putVariableUInt(0);
        }
    }
}

// QJsonValue Action::toJson() const
// {
//     QJsonObject obj;
//     obj.insert("account", account.toJson());
//     obj.insert("name", action.toJson());
//     obj.insert("data", QJsonValue(QString::fromStdString(data)));
// 
//     QJsonArray authArr;
//     for (int i = 0; i < (int)authorization.size(); ++i) {
//         authArr.append(authorization.at(i).toJson());
//     }
// 
//     obj.insert("authorization", authArr);
// 
//     return QJsonValue(obj);
// }

Json::Value Action::toJson() const
{
    Json::Value obj;
    obj["account"] = account.toJson();
    obj["name"] = action.toJson();
    obj["data"] = Json::Value(data);

    Json::Value authArr(Json::arrayValue);
    for (int i = 0; i < (int)authorization.size(); ++i) {
        authArr.append(authorization.at(i).toJson());
    }

    obj["authorization"] = authArr;

    return obj;
}


// 
// void Action::fromJson(const QJsonValue &value)
// {
//     QJsonObject obj = value.toObject();
//     if (obj.isEmpty()) {
//         return;
//     }
// 
//     data = obj.value("data").toString().toStdString();
//     account.fromJson(obj.value("account"));
//     action.fromJson(obj.value("name"));
// 
//     QJsonArray authArr = obj.value("authorization").toArray();
//     if (!authArr.isEmpty()) {
//         for (int i = 0; i < authArr.size(); ++i) {
//             TypePermissionLevel permission;
//             permission.fromJson(authArr.at(i));
//             authorization.push_back(permission);
//         }
//     }
// 
// }

void Action::fromJson(const Json::Value& value)
{
//     QJsonObject obj = value.toObject();
//     if (obj.isEmpty()) {
//         return;
//     }

    data = value["data"].asString();
    account.fromJson(value["account"]);
    action.fromJson(value["name"]);

    Json::Value authArr = value["authorization"];
    if (authArr.isArray() && authArr.size() > 0) {
        for (size_t i = 0; i < authArr.size(); ++i) {
            TypePermissionLevel permission;
            permission.fromJson(authArr[(int)i]);
            authorization.push_back(permission);
        }
    }

}

void Action::setAccount(const TypeName &account)
{
    this->account = account;
}

void Action::setAction(const TypeName &action)
{
    this->action = action;
}

void Action::addAuthorization(const TypePermissionLevel &auth)
{
    authorization.push_back(auth);
}

void Action::setAuthorization(const vector<TypePermissionLevel> &authorization)
{
    this->authorization = authorization;
}

void Action::setAuthorization(const vector<string> &accountWithPermissionLvl)
{
    if (accountWithPermissionLvl.empty()) {
        return;
    }

    for (const auto s: accountWithPermissionLvl) {
//         QString str = QString::fromStdString(s);
//         QStringList split = str.split("@");
//         authorization.push_back(TypePermissionLevel(split.at(0).toStdString(), split.at(1).toStdString()));

        string::size_type pos = s.find('@');
        if (pos == string::npos)
            return;

        string s1, s2;
        s1.assign(s, 0, pos);
        s2.assign(s, pos + 1, s.length() - pos - 1);
        authorization.push_back(TypePermissionLevel(s1, s2));
    }
}

void Action::setData(const string &data)
{
    this->data = data;
}
