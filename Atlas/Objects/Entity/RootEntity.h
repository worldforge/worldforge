// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#ifndef ATLAS_OBJECTS_ENTITY_ROOTENTITY_H
#define ATLAS_OBJECTS_ENTITY_ROOTENTITY_H

#include "../Root.h"


namespace Atlas { namespace Objects { namespace Entity { 

/** Starting point for entity hierarchy

Later in hierarchy tree objtype changes to 'object' when actual game objects are made.

*/
class RootEntity : public Root
{
public:
    /// Construct a RootEntity class definition.
    RootEntity();
    /// Default destructor.
    virtual ~RootEntity() { }

    /// Create a new instance of RootEntity.
    static RootEntity Instantiate();

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
    virtual void SendContents(Atlas::Bridge* b) const;

    /// Convert this object to a Message::Object.
    virtual Atlas::Message::Object AsObject() const;

    /// Set the "loc" attribute.
    inline void SetLoc(const std::string& val);
    /// Set the "pos" attribute.
    inline void SetPos(const Atlas::Message::Object::ListType& val);
    /// Set the "velocity" attribute.
    inline void SetVelocity(const Atlas::Message::Object::ListType& val);
    /// Set the "contains" attribute.
    inline void SetContains(const Atlas::Message::Object::ListType& val);
    /// Set the "stamp_contains" attribute.
    inline void SetStampContains(double val);

    /// Retrieve the "loc" attribute.
    inline const std::string& GetLoc() const;
    /// Retrieve the "loc" attribute as a non-const reference.
    inline std::string& GetLoc();
    /// Retrieve the "pos" attribute.
    inline const Atlas::Message::Object::ListType& GetPos() const;
    /// Retrieve the "pos" attribute as a non-const reference.
    inline Atlas::Message::Object::ListType& GetPos();
    /// Retrieve the "velocity" attribute.
    inline const Atlas::Message::Object::ListType& GetVelocity() const;
    /// Retrieve the "velocity" attribute as a non-const reference.
    inline Atlas::Message::Object::ListType& GetVelocity();
    /// Retrieve the "contains" attribute.
    inline const Atlas::Message::Object::ListType& GetContains() const;
    /// Retrieve the "contains" attribute as a non-const reference.
    inline Atlas::Message::Object::ListType& GetContains();
    /// Retrieve the "stamp_contains" attribute.
    inline double GetStampContains() const;
    /// Retrieve the "stamp_contains" attribute as a non-const reference.
    inline double& GetStampContains();

protected:
    std::string attr_loc;
    Atlas::Message::Object::ListType attr_pos;
    Atlas::Message::Object::ListType attr_velocity;
    Atlas::Message::Object::ListType attr_contains;
    double attr_stamp_contains;

    inline void SendLoc(Atlas::Bridge*) const;
    inline void SendPos(Atlas::Bridge*) const;
    inline void SendVelocity(Atlas::Bridge*) const;
    inline void SendContains(Atlas::Bridge*) const;
    inline void SendStampContains(Atlas::Bridge*) const;

};

//
// Inlined member functions follow.
//

void RootEntity::SetLoc(const std::string& val)
{
    attr_loc = val;
}

void RootEntity::SetPos(const Atlas::Message::Object::ListType& val)
{
    attr_pos = val;
}

void RootEntity::SetVelocity(const Atlas::Message::Object::ListType& val)
{
    attr_velocity = val;
}

void RootEntity::SetContains(const Atlas::Message::Object::ListType& val)
{
    attr_contains = val;
}

void RootEntity::SetStampContains(double val)
{
    attr_stamp_contains = val;
}

const std::string& RootEntity::GetLoc() const
{
    return attr_loc;
}

std::string& RootEntity::GetLoc()
{
    return attr_loc;
}

const Atlas::Message::Object::ListType& RootEntity::GetPos() const
{
    return attr_pos;
}

Atlas::Message::Object::ListType& RootEntity::GetPos()
{
    return attr_pos;
}

const Atlas::Message::Object::ListType& RootEntity::GetVelocity() const
{
    return attr_velocity;
}

Atlas::Message::Object::ListType& RootEntity::GetVelocity()
{
    return attr_velocity;
}

const Atlas::Message::Object::ListType& RootEntity::GetContains() const
{
    return attr_contains;
}

Atlas::Message::Object::ListType& RootEntity::GetContains()
{
    return attr_contains;
}

double RootEntity::GetStampContains() const
{
    return attr_stamp_contains;
}

double& RootEntity::GetStampContains()
{
    return attr_stamp_contains;
}

void RootEntity::SendLoc(Atlas::Bridge* b) const
{
    b->MapItem("loc", attr_loc);
}

void RootEntity::SendPos(Atlas::Bridge* b) const
{
    Atlas::Message::Encoder e(b);
    e.MapItem("pos", attr_pos);
}

void RootEntity::SendVelocity(Atlas::Bridge* b) const
{
    Atlas::Message::Encoder e(b);
    e.MapItem("velocity", attr_velocity);
}

void RootEntity::SendContains(Atlas::Bridge* b) const
{
    Atlas::Message::Encoder e(b);
    e.MapItem("contains", attr_contains);
}

void RootEntity::SendStampContains(Atlas::Bridge* b) const
{
    b->MapItem("stamp_contains", attr_stamp_contains);
}


} } } // namespace Atlas::Objects::Entity

#endif // ATLAS_OBJECTS_ENTITY_ROOTENTITY_H
