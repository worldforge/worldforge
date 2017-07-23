// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Automatically generated using gen_cpp.py.

#ifndef ATLAS_OBJECTS_ROOT_H
#define ATLAS_OBJECTS_ROOT_H

#include <Atlas/Objects/BaseObject.h>

#include <Atlas/Message/Element.h>

namespace Atlas { namespace Objects { 

template <class T> class SmartPtr;

/** All objects inherit from this.

You can browse all definitions starting from here and 
descending into childrens.

*/

class RootData;
typedef SmartPtr<RootData> Root;

static const int ROOT_NO = 1;

/// \brief All objects inherit from this..
///
/** You can browse all definitions starting from here and 
descending into childrens.
 */
class RootData : public BaseObjectData
{
protected:
    /// Construct a RootData class definition.
    RootData(RootData *defaults = NULL) : 
        BaseObjectData((BaseObjectData*)defaults)
    {
        m_class_no = ROOT_NO;
    }
    /// Default destructor.
    virtual ~RootData();

public:
    /// Copy this object.
    virtual RootData * copy() const;

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

    /// Set the "id" attribute.
    void setId(const std::string& val);
    /// Set the "parent" attribute.
    void setParent(const std::string& val);
    /// Set the "stamp" attribute.
    void setStamp(double val);
    /// Set the "objtype" attribute.
    void setObjtype(const std::string& val);
    /// Set the "name" attribute.
    void setName(const std::string& val);

    /// Retrieve the "id" attribute.
    const std::string& getId() const;
    /// Retrieve the "id" attribute as a non-const reference.
    std::string& modifyId();
    /// Retrieve the "parent" attribute.
    const std::string& getParent() const;
    /// Retrieve the "parent" attribute as a non-const reference.
    std::string& modifyParent();
    /// Retrieve the "stamp" attribute.
    double getStamp() const;
    /// Retrieve the "stamp" attribute as a non-const reference.
    double& modifyStamp();
    /// Retrieve the "objtype" attribute.
    const std::string& getObjtype() const;
    /// Retrieve the "objtype" attribute as a non-const reference.
    std::string& modifyObjtype();
    /// Retrieve the "name" attribute.
    const std::string& getName() const;
    /// Retrieve the "name" attribute as a non-const reference.
    std::string& modifyName();

    /// Is "id" value default?
    bool isDefaultId() const;
    /// Is "parent" value default?
    bool isDefaultParent() const;
    /// Is "stamp" value default?
    bool isDefaultStamp() const;
    /// Is "objtype" value default?
    bool isDefaultObjtype() const;
    /// Is "name" value default?
    bool isDefaultName() const;

protected:
    /// Find the class which contains the attribute "name".
    virtual int getAttrClass(const std::string& name)const;
    /// Find the flag for the attribute "name".
    virtual int getAttrFlag(const std::string& name)const;
    /// Id of object
    std::string attr_id;
    /// The object this inherits attributes from.
    std::string attr_parent;
    /// Last time this object was modified.
    double attr_stamp;
    /// What kind of object this is.
    std::string attr_objtype;
    /// Name of object.
    std::string attr_name;

    /// Send the "id" attribute to an Atlas::Bridge.
    void sendId(Atlas::Bridge&) const;
    /// Send the "parent" attribute to an Atlas::Bridge.
    void sendParent(Atlas::Bridge&) const;
    /// Send the "stamp" attribute to an Atlas::Bridge.
    void sendStamp(Atlas::Bridge&) const;
    /// Send the "objtype" attribute to an Atlas::Bridge.
    void sendObjtype(Atlas::Bridge&) const;
    /// Send the "name" attribute to an Atlas::Bridge.
    void sendName(Atlas::Bridge&) const;

    virtual void iterate(int& current_class, std::string& attr) const;

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<RootData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    virtual void reset();

private:
    virtual void free();

    static void fillDefaultObjectInstance(RootData& data, std::map<std::string, int>& attr_data);
};

//
// Attribute name strings follow.
//

extern const std::string ID_ATTR;
extern const std::string PARENT_ATTR;
extern const std::string STAMP_ATTR;
extern const std::string OBJTYPE_ATTR;
extern const std::string NAME_ATTR;

//
// Inlined member functions follow.
//

const int ID_FLAG = 1 << 1;

inline void RootData::setId(const std::string& val)
{
    attr_id = val;
    m_attrFlags |= ID_FLAG;
}

const int PARENT_FLAG = 1 << 2;

inline void RootData::setParent(const std::string& val)
{
    attr_parent = val;
    m_attrFlags |= PARENT_FLAG;
}

const int STAMP_FLAG = 1 << 3;

inline void RootData::setStamp(double val)
{
    attr_stamp = val;
    m_attrFlags |= STAMP_FLAG;
}

const int OBJTYPE_FLAG = 1 << 4;

inline void RootData::setObjtype(const std::string& val)
{
    attr_objtype = val;
    m_attrFlags |= OBJTYPE_FLAG;
}

const int NAME_FLAG = 1 << 5;

inline void RootData::setName(const std::string& val)
{
    attr_name = val;
    m_attrFlags |= NAME_FLAG;
}

inline const std::string& RootData::getId() const
{
    if(m_attrFlags & ID_FLAG)
        return attr_id;
    else
        return ((RootData*)m_defaults)->attr_id;
}

inline std::string& RootData::modifyId()
{
    if(!(m_attrFlags & ID_FLAG))
        setId(((RootData*)m_defaults)->attr_id);
    return attr_id;
}

inline const std::string& RootData::getParent() const
{
    if(m_attrFlags & PARENT_FLAG)
        return attr_parent;
    else
        return ((RootData*)m_defaults)->attr_parent;
}

inline std::string& RootData::modifyParent()
{
    if(!(m_attrFlags & PARENT_FLAG))
        setParent(((RootData*)m_defaults)->attr_parent);
    return attr_parent;
}

inline double RootData::getStamp() const
{
    if(m_attrFlags & STAMP_FLAG)
        return attr_stamp;
    else
        return ((RootData*)m_defaults)->attr_stamp;
}

inline double& RootData::modifyStamp()
{
    if(!(m_attrFlags & STAMP_FLAG))
        setStamp(((RootData*)m_defaults)->attr_stamp);
    return attr_stamp;
}

inline const std::string& RootData::getObjtype() const
{
    if(m_attrFlags & OBJTYPE_FLAG)
        return attr_objtype;
    else
        return ((RootData*)m_defaults)->attr_objtype;
}

inline std::string& RootData::modifyObjtype()
{
    if(!(m_attrFlags & OBJTYPE_FLAG))
        setObjtype(((RootData*)m_defaults)->attr_objtype);
    return attr_objtype;
}

inline const std::string& RootData::getName() const
{
    if(m_attrFlags & NAME_FLAG)
        return attr_name;
    else
        return ((RootData*)m_defaults)->attr_name;
}

inline std::string& RootData::modifyName()
{
    if(!(m_attrFlags & NAME_FLAG))
        setName(((RootData*)m_defaults)->attr_name);
    return attr_name;
}

inline bool RootData::isDefaultId() const
{
    return (m_attrFlags & ID_FLAG) == 0;
}

inline bool RootData::isDefaultParent() const
{
    return (m_attrFlags & PARENT_FLAG) == 0;
}

inline bool RootData::isDefaultStamp() const
{
    return (m_attrFlags & STAMP_FLAG) == 0;
}

inline bool RootData::isDefaultObjtype() const
{
    return (m_attrFlags & OBJTYPE_FLAG) == 0;
}

inline bool RootData::isDefaultName() const
{
    return (m_attrFlags & NAME_FLAG) == 0;
}


} } // namespace Atlas::Objects

#endif // ATLAS_OBJECTS_ROOT_H
