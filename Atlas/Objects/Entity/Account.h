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
                /// Construct a Account class definition.
    Account();
                /// Default destructor.
    virtual ~Account() { }

                /// Create a new instance of Account.
    static Account Instantiate();

                /// Check whether the attribute "name" exists.
    virtual bool HasAttr(const std::string& name)const;
                /// Retrieve the attribute "name". Throws NoSuchAttrException if it does
                /// not exist.
    virtual Atlas::Message::Object GetAttr(const std::string& name)
            const throw (NoSuchAttrException);
                /// Set the attribute "name" to the value given by"attr"
    virtual void SetAttr(const std::string& name,
                         const Atlas::Message::Object& attr);
                /// Remove the attribute "name". This will not work for static attributes.
    virtual void RemoveAttr(const std::string& name);

                /// Send the contents of this object to a Bridge.
    virtual void SendContents(Atlas::Bridge* b);

                /// Convert this object to a Message::Object.
    virtual Atlas::Message::Object AsObject() const;

                /// Set the "password" attribute.
    inline void SetPassword(const std::string& val);

                /// Retrieve the "password" attribute.
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
