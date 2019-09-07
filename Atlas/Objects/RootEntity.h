// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011-2019 Erik Ogenvik.
// Automatically generated using gen_cpp.py. Don't edit directly.

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
    explicit RootEntityData(RootEntityData *defaults = nullptr) : 
        RootData((RootData*)defaults)
    {
        m_class_no = ROOT_ENTITY_NO;
    }
    /// Default destructor.
    ~RootEntityData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "root";
    // The default parent type for this object
    static constexpr const char* default_parent = "root_entity";
    // The default objtype for this object
    static constexpr const char* default_objtype = "class";
    /// Copy this object.
    RootEntityData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;

    /// Retrieve the attribute "name". Return non-zero if it does
    /// not exist.
    int copyAttr(const std::string& name, Atlas::Message::Element & attr) const override;
    /// Set the attribute "name" to the value given by"attr"
    void setAttr(const std::string& name,
                         Atlas::Message::Element attr) override;
    /// Remove the attribute "name". This will not work for static attributes.
    void removeAttr(const std::string& name) override;

    /// Send the contents of this object to a Bridge.
    void sendContents(Atlas::Bridge & b) const override;

    /// Write this object to an existing Element.
    void addToMessage(Atlas::Message::MapType &) const override;

    /// Set the "loc" attribute.
    void setLoc(std::string val);
    /// Set the "pos" attribute.
    void setPos(std::vector<double> val);
    /// Set the "pos" attribute AsList.
    void setPosAsList(const Atlas::Message::ListType& val);
    /// Set the "velocity" attribute.
    void setVelocity(std::vector<double> val);
    /// Set the "velocity" attribute AsList.
    void setVelocityAsList(const Atlas::Message::ListType& val);
    /// Set the "contains" attribute.
    void setContains(std::list<std::string> val);
    /// Set the "contains" attribute AsList.
    void setContainsAsList(const Atlas::Message::ListType& val);
    /// Set the "stamp_contains" attribute.
    void setStampContains(double val);

    /// Retrieve the "loc" attribute.
    const std::string& getLoc() const;
    /// Retrieve the "loc" attribute as a non-const reference.
    std::string& modifyLoc();
    /// Retrieve the "pos" attribute.
    const std::vector<double>& getPos() const;
    /// Retrieve the "pos" attribute as a non-const reference.
    std::vector<double>& modifyPos();
    /// Retrieve the "pos" attribute AsList.
    Atlas::Message::ListType getPosAsList() const;
    /// Retrieve the "velocity" attribute.
    const std::vector<double>& getVelocity() const;
    /// Retrieve the "velocity" attribute as a non-const reference.
    std::vector<double>& modifyVelocity();
    /// Retrieve the "velocity" attribute AsList.
    Atlas::Message::ListType getVelocityAsList() const;
    /// Retrieve the "contains" attribute.
    const std::list<std::string>& getContains() const;
    /// Retrieve the "contains" attribute as a non-const reference.
    std::list<std::string>& modifyContains();
    /// Retrieve the "contains" attribute AsList.
    Atlas::Message::ListType getContainsAsList() const;
    /// Retrieve the "stamp_contains" attribute.
    double getStampContains() const;
    /// Retrieve the "stamp_contains" attribute as a non-const reference.
    double& modifyStampContains();

    /// Is "loc" value default?
    bool isDefaultLoc() const;
    /// Is "pos" value default?
    bool isDefaultPos() const;
    /// Is "velocity" value default?
    bool isDefaultVelocity() const;
    /// Is "contains" value default?
    bool isDefaultContains() const;
    /// Is "stamp_contains" value default?
    bool isDefaultStampContains() const;

protected:
    /// Find the class which contains the attribute "name".
    int getAttrClass(const std::string& name)const override;
    /// Find the flag for the attribute "name".
    bool getAttrFlag(const std::string& name, uint32_t& flag)const override;
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

    void iterate(int& current_class, std::string& attr) const override;

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<RootEntityData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(RootEntityData& data, std::map<std::string, uint32_t>& attr_data);
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

const uint32_t LOC_FLAG = 1u << 6u;

inline void RootEntityData::setLoc(std::string val)
{
    attr_loc = std::move(val);
    m_attrFlags |= LOC_FLAG;
}

const uint32_t POS_FLAG = 1u << 7u;

inline void RootEntityData::setPos(std::vector<double> val)
{
    attr_pos = std::move(val);
    m_attrFlags |= POS_FLAG;
}

inline void RootEntityData::setPosAsList(const Atlas::Message::ListType& val)
{
    m_attrFlags |= POS_FLAG;
    attr_pos.resize(0);
    for (const auto& entry : val) {
        if(entry.isNum()) {
            attr_pos.push_back(entry.asNum());
        }
    }
}

const uint32_t VELOCITY_FLAG = 1u << 8u;

inline void RootEntityData::setVelocity(std::vector<double> val)
{
    attr_velocity = std::move(val);
    m_attrFlags |= VELOCITY_FLAG;
}

inline void RootEntityData::setVelocityAsList(const Atlas::Message::ListType& val)
{
    m_attrFlags |= VELOCITY_FLAG;
    attr_velocity.resize(0);
    for (const auto& entry : val) {
        if(entry.isNum()) {
            attr_velocity.push_back(entry.asNum());
        }
    }
}

const uint32_t CONTAINS_FLAG = 1u << 9u;

inline void RootEntityData::setContains(std::list<std::string> val)
{
    attr_contains = std::move(val);
    m_attrFlags |= CONTAINS_FLAG;
}

inline void RootEntityData::setContainsAsList(const Atlas::Message::ListType& val)
{
    m_attrFlags |= CONTAINS_FLAG;
    attr_contains.resize(0);
    for (const auto& entry : val) {
        if(entry.isString()) {
            attr_contains.push_back(entry.asString());
        }
    }
}

const uint32_t STAMP_CONTAINS_FLAG = 1u << 10u;

inline void RootEntityData::setStampContains(double val)
{
    attr_stamp_contains = val;
    m_attrFlags |= STAMP_CONTAINS_FLAG;
}

inline const std::string& RootEntityData::getLoc() const
{
    if(m_attrFlags & LOC_FLAG)
        return attr_loc;
    else
        return ((RootEntityData*)m_defaults)->attr_loc;
}

inline std::string& RootEntityData::modifyLoc()
{
    if(!(m_attrFlags & LOC_FLAG))
        setLoc(((RootEntityData*)m_defaults)->attr_loc);
    return attr_loc;
}

inline const std::vector<double>& RootEntityData::getPos() const
{
    if(m_attrFlags & POS_FLAG)
        return attr_pos;
    else
        return ((RootEntityData*)m_defaults)->attr_pos;
}

inline std::vector<double>& RootEntityData::modifyPos()
{
    if(!(m_attrFlags & POS_FLAG))
        setPos(((RootEntityData*)m_defaults)->attr_pos);
    return attr_pos;
}

inline Atlas::Message::ListType RootEntityData::getPosAsList() const
{
    const std::vector<double>& lst_in = getPos();
    Atlas::Message::ListType lst_out;
    for (const auto& entry : lst_in) {
        lst_out.push_back(entry);
    }
    return lst_out;
}

inline const std::vector<double>& RootEntityData::getVelocity() const
{
    if(m_attrFlags & VELOCITY_FLAG)
        return attr_velocity;
    else
        return ((RootEntityData*)m_defaults)->attr_velocity;
}

inline std::vector<double>& RootEntityData::modifyVelocity()
{
    if(!(m_attrFlags & VELOCITY_FLAG))
        setVelocity(((RootEntityData*)m_defaults)->attr_velocity);
    return attr_velocity;
}

inline Atlas::Message::ListType RootEntityData::getVelocityAsList() const
{
    const std::vector<double>& lst_in = getVelocity();
    Atlas::Message::ListType lst_out;
    for (const auto& entry : lst_in) {
        lst_out.push_back(entry);
    }
    return lst_out;
}

inline const std::list<std::string>& RootEntityData::getContains() const
{
    if(m_attrFlags & CONTAINS_FLAG)
        return attr_contains;
    else
        return ((RootEntityData*)m_defaults)->attr_contains;
}

inline std::list<std::string>& RootEntityData::modifyContains()
{
    if(!(m_attrFlags & CONTAINS_FLAG))
        setContains(((RootEntityData*)m_defaults)->attr_contains);
    return attr_contains;
}

inline Atlas::Message::ListType RootEntityData::getContainsAsList() const
{
    const std::list<std::string>& lst_in = getContains();
    Atlas::Message::ListType lst_out;
    for (const auto& entry : lst_in) {
        lst_out.push_back(std::string(entry));
    }
    return lst_out;
}

inline double RootEntityData::getStampContains() const
{
    if(m_attrFlags & STAMP_CONTAINS_FLAG)
        return attr_stamp_contains;
    else
        return ((RootEntityData*)m_defaults)->attr_stamp_contains;
}

inline double& RootEntityData::modifyStampContains()
{
    if(!(m_attrFlags & STAMP_CONTAINS_FLAG))
        setStampContains(((RootEntityData*)m_defaults)->attr_stamp_contains);
    return attr_stamp_contains;
}

inline bool RootEntityData::isDefaultLoc() const
{
    return (m_attrFlags & LOC_FLAG) == 0;
}

inline bool RootEntityData::isDefaultPos() const
{
    return (m_attrFlags & POS_FLAG) == 0;
}

inline bool RootEntityData::isDefaultVelocity() const
{
    return (m_attrFlags & VELOCITY_FLAG) == 0;
}

inline bool RootEntityData::isDefaultContains() const
{
    return (m_attrFlags & CONTAINS_FLAG) == 0;
}

inline bool RootEntityData::isDefaultStampContains() const
{
    return (m_attrFlags & STAMP_CONTAINS_FLAG) == 0;
}


} } } // namespace Atlas::Objects::Entity

#endif // ATLAS_OBJECTS_ENTITY_ROOTENTITY_H
