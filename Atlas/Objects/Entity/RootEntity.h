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
    RootEntity();
    virtual ~RootEntity() { }

    static RootEntity Instantiate();

    virtual bool HasAttr(const std::string& name)const;
    virtual Atlas::Message::Object GetAttr(const std::string& name)
            const throw (NoSuchAttrException);
    virtual void SetAttr(const std::string& name,
                         const Atlas::Message::Object& attr);
    virtual void RemoveAttr(const std::string& name);

    inline void SetLoc(const std::string& val);
    inline void SetPos(const Atlas::Message::Object::ListType& val);
    inline void SetVelocity(const Atlas::Message::Object::ListType& val);
    inline void SetContains(const Atlas::Message::Object::ListType& val);
    inline void SetStampContains(double val);

    inline const std::string& GetLoc() const;
    inline const Atlas::Message::Object::ListType& GetPos() const;
    inline const Atlas::Message::Object::ListType& GetVelocity() const;
    inline const Atlas::Message::Object::ListType& GetContains() const;
    inline double GetStampContains() const;

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

const Atlas::Message::Object::ListType& RootEntity::GetPos() const
{
    return attr_pos;
}

const Atlas::Message::Object::ListType& RootEntity::GetVelocity() const
{
    return attr_velocity;
}

const Atlas::Message::Object::ListType& RootEntity::GetContains() const
{
    return attr_contains;
}

double RootEntity::GetStampContains() const
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
