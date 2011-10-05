// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Automatically generated using gen_cpp.py.

#ifndef ATLAS_OBJECTS_ENTITY_ROOTENTITY_H
#define ATLAS_OBJECTS_ENTITY_ROOTENTITY_H

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

namespace Atlas { namespace Objects { namespace Entity { 

/** Starting point for entity hierarchy

Later in hierarchy tree objtype changes to 'object' when actual game objects are made.

*/

class RootEntityData;
typedef SmartPtr<RootEntityData> RootEntity;

static const int ROOT_ENTITY_NO = 2;

/// \brief Starting point for entity hierarchy.
///
/** Later in hierarchy tree objtype changes to 'object' when actual game objects are made.
 */
class RootEntityData : public RootData
{
protected:
    /// Construct a RootEntityData class definition.
    RootEntityData(RootEntityData *defaults = NULL) : 
        RootData((RootData*)defaults)
    {
        m_class_no = ROOT_ENTITY_NO;
    }
    /// Default destructor.
    virtual ~RootEntityData();

public:
    /// Copy this object.
    virtual RootEntityData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;

    /// Retrieve the attribute "name". Return non-zero if it does
    /// not exist.
    virtual int copyAttr(const std::string& name, Atlas::Message::Element & attr) const;
    /// Set the attribute "name" to the value given by"attr"
    virtual void setAttr(const std::string& name,
                         const Atlas::Message::Element& attr);
    /// Remove the attribute "name". This will not work for static attributes.
    virtual void removeAttr(const std::string& name);

    /// Send the contents of this object to a Bridge.
    virtual void sendContents(Atlas::Bridge & b) const;

    /// Write this object to an existing Element.
    virtual void addToMessage(Atlas::Message::MapType &) const;

    /// Set the "loc" attribute.
    inline void setLoc(const std::string& val);
    /// Set the "pos" attribute.
    inline void setPos(const std::vector<double>& val);
    /// Set the "pos" attribute AsList.
    inline void setPosAsList(const Atlas::Message::ListType& val);
    /// Set the "velocity" attribute.
    inline void setVelocity(const std::vector<double>& val);
    /// Set the "velocity" attribute AsList.
    inline void setVelocityAsList(const Atlas::Message::ListType& val);
    /// Set the "contains" attribute.
    inline void setContains(const std::list<std::string>& val);
    /// Set the "contains" attribute AsList.
    inline void setContainsAsList(const Atlas::Message::ListType& val);
    /// Set the "stamp_contains" attribute.
    inline void setStampContains(double val);

    /// Retrieve the "loc" attribute.
    inline const std::string& getLoc() const;
    /// Retrieve the "loc" attribute as a non-const reference.
    inline std::string& modifyLoc();
    /// Retrieve the "pos" attribute.
    inline const std::vector<double>& getPos() const;
    /// Retrieve the "pos" attribute as a non-const reference.
    inline std::vector<double>& modifyPos();
    /// Retrieve the "pos" attribute AsList.
    inline const Atlas::Message::ListType getPosAsList() const;
    /// Retrieve the "velocity" attribute.
    inline const std::vector<double>& getVelocity() const;
    /// Retrieve the "velocity" attribute as a non-const reference.
    inline std::vector<double>& modifyVelocity();
    /// Retrieve the "velocity" attribute AsList.
    inline const Atlas::Message::ListType getVelocityAsList() const;
    /// Retrieve the "contains" attribute.
    inline const std::list<std::string>& getContains() const;
    /// Retrieve the "contains" attribute as a non-const reference.
    inline std::list<std::string>& modifyContains();
    /// Retrieve the "contains" attribute AsList.
    inline const Atlas::Message::ListType getContainsAsList() const;
    /// Retrieve the "stamp_contains" attribute.
    inline double getStampContains() const;
    /// Retrieve the "stamp_contains" attribute as a non-const reference.
    inline double& modifyStampContains();

    /// Is "loc" value default?
    inline bool isDefaultLoc() const;
    /// Is "pos" value default?
    inline bool isDefaultPos() const;
    /// Is "velocity" value default?
    inline bool isDefaultVelocity() const;
    /// Is "contains" value default?
    inline bool isDefaultContains() const;
    /// Is "stamp_contains" value default?
    inline bool isDefaultStampContains() const;

protected:
    /// Find the class which contains the attribute "name".
    virtual int getAttrClass(const std::string& name)const;
    /// Find the flag for the attribute "name".
    virtual int getAttrFlag(const std::string& name)const;
    /// Reference object for coordinates (location).
    std::string attr_loc;
    /// Position coordinates, usually world is 3D
    std::vector<double> attr_pos;
    /// Velocity object is moving, usually world is 3D
    std::vector<double> attr_velocity;
    /// List of objects that use this object as reference system (usually same as what this object contains).
    std::list<std::string> attr_contains;
    /// Last time any object that uses this as reference recursively has been modified
    double attr_stamp_contains;

    /// Send the "loc" attribute to an Atlas::Bridge.
    void sendLoc(Atlas::Bridge&) const;
    /// Send the "pos" attribute to an Atlas::Bridge.
    void sendPos(Atlas::Bridge&) const;
    /// Send the "velocity" attribute to an Atlas::Bridge.
    void sendVelocity(Atlas::Bridge&) const;
    /// Send the "contains" attribute to an Atlas::Bridge.
    void sendContains(Atlas::Bridge&) const;
    /// Send the "stamp_contains" attribute to an Atlas::Bridge.
    void sendStampContains(Atlas::Bridge&) const;

    virtual void iterate(int& current_class, std::string& attr) const;

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<RootEntityData> allocator;

private:
    virtual void free();

    static void fillDefaultObjectInstance(RootEntityData& data, std::map<std::string, int>& attr_data);
};

//
// Attribute name strings follow.
//

extern const std::string LOC_ATTR;
extern const std::string POS_ATTR;
extern const std::string VELOCITY_ATTR;
extern const std::string CONTAINS_ATTR;
extern const std::string STAMP_CONTAINS_ATTR;

//
// Inlined member functions follow.
//

const int LOC_FLAG = 1 << 6;

void RootEntityData::setLoc(const std::string& val)
{
    attr_loc = val;
    m_attrFlags |= LOC_FLAG;
}

const int POS_FLAG = 1 << 7;

void RootEntityData::setPos(const std::vector<double>& val)
{
    attr_pos = val;
    m_attrFlags |= POS_FLAG;
}

void RootEntityData::setPosAsList(const Atlas::Message::ListType& val)
{
    m_attrFlags |= POS_FLAG;
    attr_pos.resize(0);
    for(Atlas::Message::ListType::const_iterator I = val.begin();
        I != val.end();
        I++)
    {
        if((*I).isNum()) {
            attr_pos.push_back((*I).asNum());
        }
    }
}

const int VELOCITY_FLAG = 1 << 8;

void RootEntityData::setVelocity(const std::vector<double>& val)
{
    attr_velocity = val;
    m_attrFlags |= VELOCITY_FLAG;
}

void RootEntityData::setVelocityAsList(const Atlas::Message::ListType& val)
{
    m_attrFlags |= VELOCITY_FLAG;
    attr_velocity.resize(0);
    for(Atlas::Message::ListType::const_iterator I = val.begin();
        I != val.end();
        I++)
    {
        if((*I).isNum()) {
            attr_velocity.push_back((*I).asNum());
        }
    }
}

const int CONTAINS_FLAG = 1 << 9;

void RootEntityData::setContains(const std::list<std::string>& val)
{
    attr_contains = val;
    m_attrFlags |= CONTAINS_FLAG;
}

void RootEntityData::setContainsAsList(const Atlas::Message::ListType& val)
{
    m_attrFlags |= CONTAINS_FLAG;
    attr_contains.resize(0);
    for(Atlas::Message::ListType::const_iterator I = val.begin();
        I != val.end();
        I++)
    {
        if((*I).isString()) {
            attr_contains.push_back((*I).asString());
        }
    }
}

const int STAMP_CONTAINS_FLAG = 1 << 10;

void RootEntityData::setStampContains(double val)
{
    attr_stamp_contains = val;
    m_attrFlags |= STAMP_CONTAINS_FLAG;
}

const std::string& RootEntityData::getLoc() const
{
    if(m_attrFlags & LOC_FLAG)
        return attr_loc;
    else
        return ((RootEntityData*)m_defaults)->attr_loc;
}

std::string& RootEntityData::modifyLoc()
{
    if(!(m_attrFlags & LOC_FLAG))
        setLoc(((RootEntityData*)m_defaults)->attr_loc);
    return attr_loc;
}

const std::vector<double>& RootEntityData::getPos() const
{
    if(m_attrFlags & POS_FLAG)
        return attr_pos;
    else
        return ((RootEntityData*)m_defaults)->attr_pos;
}

std::vector<double>& RootEntityData::modifyPos()
{
    if(!(m_attrFlags & POS_FLAG))
        setPos(((RootEntityData*)m_defaults)->attr_pos);
    return attr_pos;
}

const Atlas::Message::ListType RootEntityData::getPosAsList() const
{
    const std::vector<double>& lst_in = getPos();
    Atlas::Message::ListType lst_out;
    for(std::vector<double>::const_iterator I = lst_in.begin();
        I != lst_in.end();
        I++)
    {
        lst_out.push_back(*I);
    }
    return lst_out;
}

const std::vector<double>& RootEntityData::getVelocity() const
{
    if(m_attrFlags & VELOCITY_FLAG)
        return attr_velocity;
    else
        return ((RootEntityData*)m_defaults)->attr_velocity;
}

std::vector<double>& RootEntityData::modifyVelocity()
{
    if(!(m_attrFlags & VELOCITY_FLAG))
        setVelocity(((RootEntityData*)m_defaults)->attr_velocity);
    return attr_velocity;
}

const Atlas::Message::ListType RootEntityData::getVelocityAsList() const
{
    const std::vector<double>& lst_in = getVelocity();
    Atlas::Message::ListType lst_out;
    for(std::vector<double>::const_iterator I = lst_in.begin();
        I != lst_in.end();
        I++)
    {
        lst_out.push_back(*I);
    }
    return lst_out;
}

const std::list<std::string>& RootEntityData::getContains() const
{
    if(m_attrFlags & CONTAINS_FLAG)
        return attr_contains;
    else
        return ((RootEntityData*)m_defaults)->attr_contains;
}

std::list<std::string>& RootEntityData::modifyContains()
{
    if(!(m_attrFlags & CONTAINS_FLAG))
        setContains(((RootEntityData*)m_defaults)->attr_contains);
    return attr_contains;
}

const Atlas::Message::ListType RootEntityData::getContainsAsList() const
{
    const std::list<std::string>& lst_in = getContains();
    Atlas::Message::ListType lst_out;
    for(std::list<std::string>::const_iterator I = lst_in.begin();
        I != lst_in.end();
        I++)
    {
        lst_out.push_back(std::string(*I));
    }
    return lst_out;
}

double RootEntityData::getStampContains() const
{
    if(m_attrFlags & STAMP_CONTAINS_FLAG)
        return attr_stamp_contains;
    else
        return ((RootEntityData*)m_defaults)->attr_stamp_contains;
}

double& RootEntityData::modifyStampContains()
{
    if(!(m_attrFlags & STAMP_CONTAINS_FLAG))
        setStampContains(((RootEntityData*)m_defaults)->attr_stamp_contains);
    return attr_stamp_contains;
}

bool RootEntityData::isDefaultLoc() const
{
    return (m_attrFlags & LOC_FLAG) == 0;
}

bool RootEntityData::isDefaultPos() const
{
    return (m_attrFlags & POS_FLAG) == 0;
}

bool RootEntityData::isDefaultVelocity() const
{
    return (m_attrFlags & VELOCITY_FLAG) == 0;
}

bool RootEntityData::isDefaultContains() const
{
    return (m_attrFlags & CONTAINS_FLAG) == 0;
}

bool RootEntityData::isDefaultStampContains() const
{
    return (m_attrFlags & STAMP_CONTAINS_FLAG) == 0;
}


} } } // namespace Atlas::Objects::Entity

#endif // ATLAS_OBJECTS_ENTITY_ROOTENTITY_H
