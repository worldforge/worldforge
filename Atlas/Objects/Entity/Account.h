// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#ifndef ATLAS_OBJECTS_ENTITY_ACCOUNT_H
#define ATLAS_OBJECTS_ENTITY_ACCOUNT_H

#include "AdminEntity.h"


namespace Atlas { namespace Objects { namespace Entity { 

/** Base class for accounts

Later in hierarchy tree objtype changes to 'object' when actual game objects are made.

*/
class Account : public AdminEntity
{
public:
    Account();
    virtual ~Account() { }

    static Account Instantiate();

    virtual bool HasAttr(const std::string& name)const;
    virtual Atlas::Message::Object GetAttr(const std::string& name)
            const throw (NoSuchAttrException);
    virtual void SetAttr(const std::string& name,
                         const Atlas::Message::Object& attr);
    virtual void RemoveAttr(const std::string& name);

    virtual void SendContents(Atlas::Bridge* b);

    inline void SetPassword(const std::string& val);

    inline const std::string& GetPassword() const;

protected:
    std::string attr_password;

    inline void SendPassword(Atlas::Bridge*) const;

};

//
// Inlined member functions follow.
//

void Account::SetPassword(const std::string& val)
{
    attr_password = val;
}

const std::string& Account::GetPassword() const
{
    return attr_password;
}

void Account::SendPassword(Atlas::Bridge* b) const
{
    b->MapItem("password", attr_password);
}


} } } // namespace Atlas::Objects::Entity

#endif // ATLAS_OBJECTS_ENTITY_ACCOUNT_H
