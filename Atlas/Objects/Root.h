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
    /// Construct a Root class definition.
    Root();
    /// Construct a Root instance.
    Root(const string& id);
    /// Default destructor.
    virtual ~Root();

    /// Create a new instance of Root.
    static Root Instantiate();

    /// Check whether the attribute "name" exists.
    virtual bool HasAttr(const std::string& name) const;
    /// Retrieve the attribute "name". Throws NoSuchAttrException if it does
    /// not exist.
    virtual Atlas::Message::Object GetAttr(const std::string& name)
        const throw (NoSuchAttrException);
    /// Set the attribute "name" to the value given by "attr".
    virtual void SetAttr(const std::string& name,
                         const Atlas::Message::Object& attr);
    /// Remove the attribute "name". This will not work for static attributes.
    virtual void RemoveAttr(const std::string& name);

    /// Set the "parents" attribute.
    inline void SetParents(const Atlas::Message::Object::ListType& val);
    /// Set the "id" attribute.
    inline void SetId(const std::string& val);
    /// Set the "objtype" attribute.
    inline void SetObjtype(const std::string& val);
    /// Set the "name" attribute.
    inline void SetName(const std::string& val);

    /// Retrieve the "parents" attribute.
    inline const Atlas::Message::Object::ListType& GetParents() const;
    /// Retrieve the "id" attribute.
    inline const std::string& GetId() const;
    /// Retrieve the "objtype" attribute.
    inline const std::string& GetObjtype() const;
    /// Retrieve the "name" attribute.
    inline const std::string& GetName() const;
    
    /// Convert this object to a Message::Object.
    virtual Atlas::Message::Object AsObject() const;

    /// Send the contents of this object to a Bridge.
    virtual void SendContents(Atlas::Bridge* b) const;

protected:
    std::map<std::string, Atlas::Message::Object> attributes;
    Atlas::Message::Object::ListType attr_parents;
    std::string attr_id;
    std::string attr_objtype;
    std::string attr_name;

    inline void SendParents(Atlas::Bridge*) const;
    inline void SendId(Atlas::Bridge*) const;
    inline void SendObjtype(Atlas::Bridge*) const;
    inline void SendName(Atlas::Bridge*) const;
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

void Root::SendParents(Atlas::Bridge* b) const
{
    Atlas::Message::Encoder e(b);
    e.MapItem("parents", attr_parents);
}

void Root::SendId(Atlas::Bridge* b) const
{
    b->MapItem("id", attr_id);
}

void Root::SendObjtype(Atlas::Bridge* b) const
{
    b->MapItem("objtype", attr_objtype);
}

void Root::SendName(Atlas::Bridge* b) const
{
    b->MapItem("name", attr_name);
}


} }

#endif
