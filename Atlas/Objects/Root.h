// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_OBJECTS_ROOT_H
#define ATLAS_OBJECTS_ROOT_H

#include <map>
#include <string>
#include "../Message/Encoder.h"
#include "../Message/Object.h"
#include "../Bridge.h"

namespace Atlas { namespace Objects {

/** An exception indicating the requested attribute does not exist.
 *
 * This is thrown by Root::GetAttr() [and derivatives thereof!]
 */
class NoSuchAttrException
{
public:
    NoSuchAttrException(const std::string& name) : name(name) {}
    std::string name;
};

/** All objects inherit from this


  
*/
class Root {
public:
    Root();
    Root(const string& id);
    virtual ~Root();

    static Root Instantiate();

    virtual bool HasAttr(const std::string& name) const;
    virtual Atlas::Message::Object GetAttr(const std::string& name)
        const throw (NoSuchAttrException);
    virtual void SetAttr(const std::string& name,
                         const Atlas::Message::Object& attr);
    virtual void RemoveAttr(const std::string& name);

    inline void SetParents(const Atlas::Message::Object::ListType& val);
    inline void SetId(const std::string& val);
    inline void SetObjtype(const std::string& val);
    inline void SetName(const std::string& val);

    inline const Atlas::Message::Object::ListType& GetParents() const;
    inline const std::string& GetId() const;
    inline const std::string& GetObjtype() const;
    inline const std::string& GetName() const;
    
    virtual Atlas::Message::Object AsObject() const;

    virtual void SendContents(Atlas::Bridge* b) const;

protected:
    std::map<std::string, Atlas::Message::Object> attributes;
    Atlas::Message::Object::ListType attr_parents;
    std::string attr_id;
    std::string attr_objtype;
    std::string attr_name;

    inline void SendParents(Bridge*) const;
    inline void SendId(Bridge*) const;
    inline void SendObjtype(Bridge*) const;
    inline void SendName(Bridge*) const;
};

// 
// Inlined member functions follow.
//

void Root::SetParents(const Atlas::Message::Object::ListType& val)
{
    attr_parents = val;
}

void Root::SetId(const std::string& val)
{
    attr_id = val;
}

void Root::SetObjtype(const std::string& val)
{
    attr_objtype = val;
}

void Root::SetName(const std::string& val)
{
    attr_name = val;
}

const Atlas::Message::Object::ListType& Root::GetParents() const
{
    return attr_parents;
}

const std::string& Root::GetId() const
{
    return attr_id;
}

const std::string& Root::GetObjtype() const
{
    return attr_objtype;
}

const std::string& Root::GetName() const
{
    return attr_name;
}

void Root::SendParents(Bridge* b) const
{
    Atlas::Message::Encoder e(b);
    e.MapItem("parents", attr_parents);
}

void Root::SendId(Bridge* b) const
{
    b->MapItem("parents", attr_id);
}

void Root::SendObjtype(Bridge* b) const
{
    b->MapItem("parents", attr_objtype);
}

void Root::SendName(Bridge* b) const
{
    b->MapItem("parents", attr_name);
}


} }

#endif
