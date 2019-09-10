// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Copyright 2011-2019 Erik Ogenvik.
// Automatically generated using gen_cpp.py. Don't edit directly.

#ifndef ATLAS_OBJECTS_OPERATION_ROOTOPERATION_H
#define ATLAS_OBJECTS_OPERATION_ROOTOPERATION_H

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#include <Atlas/Objects/Factories.h>

namespace Atlas { namespace Objects { namespace Operation { 

/** Base operation for all operators

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/

class RootOperationData;
typedef SmartPtr<RootOperationData> RootOperation;

static const int ROOT_OPERATION_NO = 9;

/// \brief Base operation for all operators.
///
/** This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.
 */
class RootOperationData : public RootData
{
protected:
    /// Construct a RootOperationData class definition.
    explicit RootOperationData(RootOperationData *defaults = nullptr) : 
        RootData((RootData*)defaults)
    {
        m_class_no = ROOT_OPERATION_NO;
    }
    /// Default destructor.
    ~RootOperationData() override = default;

public:
    // The parent type for this object's superclass
    static constexpr const char* super_parent = "root";
    // The default parent type for this object
    static constexpr const char* default_parent = "root_operation";
    // The default objtype for this object
    static constexpr const char* default_objtype = "op";
    /// Copy this object.
    RootOperationData * copy() const override;

    /// Is this instance of some class?
    bool instanceOf(int classNo) const override;

    /// Retrieve the attribute "name". Return non-zero if it does
    /// not exist.
    int copyAttr(const std::string& name, Atlas::Message::Element & attr) const override;
    /// Set the attribute "name" to the value given by"attr"
    void setAttr(const std::string& name,
                         Atlas::Message::Element attr, const Atlas::Objects::Factories* factories = nullptr) override;
    /// Remove the attribute "name". This will not work for static attributes.
    void removeAttr(const std::string& name) override;

    /// Send the contents of this object to a Bridge.
    void sendContents(Atlas::Bridge & b) const override;

    /// Write this object to an existing Element.
    void addToMessage(Atlas::Message::MapType &) const override;

    /// Set the "serialno" attribute.
    void setSerialno(long val);
    /// Set the "refno" attribute.
    void setRefno(long val);
    /// Set the "from" attribute.
    void setFrom(std::string val);
    /// Set the "to" attribute.
    void setTo(std::string val);
    /// Set the "seconds" attribute.
    void setSeconds(double val);
    /// Set the "future_seconds" attribute.
    void setFutureSeconds(double val);
    /// Set the "args" attribute.
    void setArgs(std::vector<Root> val);
    /// Set the "args" attribute AsList.
    void setArgsAsList(const Atlas::Message::ListType& val, const Atlas::Objects::Factories* factories);
    /// Set the first member of "args"
    template <class ObjectData>
    void setArgs1(const SmartPtr<ObjectData> & val);

    /// Retrieve the "serialno" attribute.
    long getSerialno() const;
    /// Retrieve the "serialno" attribute as a non-const reference.
    long& modifySerialno();
    /// Retrieve the "refno" attribute.
    long getRefno() const;
    /// Retrieve the "refno" attribute as a non-const reference.
    long& modifyRefno();
    /// Retrieve the "from" attribute.
    const std::string& getFrom() const;
    /// Retrieve the "from" attribute as a non-const reference.
    std::string& modifyFrom();
    /// Retrieve the "to" attribute.
    const std::string& getTo() const;
    /// Retrieve the "to" attribute as a non-const reference.
    std::string& modifyTo();
    /// Retrieve the "seconds" attribute.
    double getSeconds() const;
    /// Retrieve the "seconds" attribute as a non-const reference.
    double& modifySeconds();
    /// Retrieve the "future_seconds" attribute.
    double getFutureSeconds() const;
    /// Retrieve the "future_seconds" attribute as a non-const reference.
    double& modifyFutureSeconds();
    /// Retrieve the "args" attribute.
    const std::vector<Root>& getArgs() const;
    /// Retrieve the "args" attribute as a non-const reference.
    std::vector<Root>& modifyArgs();
    /// Retrieve the "args" attribute AsList.
    const Atlas::Message::ListType getArgsAsList() const;

    /// Is "serialno" value default?
    bool isDefaultSerialno() const;
    /// Is "refno" value default?
    bool isDefaultRefno() const;
    /// Is "from" value default?
    bool isDefaultFrom() const;
    /// Is "to" value default?
    bool isDefaultTo() const;
    /// Is "seconds" value default?
    bool isDefaultSeconds() const;
    /// Is "future_seconds" value default?
    bool isDefaultFutureSeconds() const;
    /// Is "args" value default?
    bool isDefaultArgs() const;

protected:
    /// Find the class which contains the attribute "name".
    int getAttrClass(const std::string& name)const override;
    /// Find the flag for the attribute "name".
    bool getAttrFlag(const std::string& name, uint32_t& flag)const override;
    /// Serial number.
    long attr_serialno;
    /// Reference to serial number.
    long attr_refno;
    /// Source of message/operation.
    std::string attr_from;
    /// Target of message/operation.
    std::string attr_to;
    /// Time in seconds
    double attr_seconds;
    /// Time in seconds to add current time
    double attr_future_seconds;
    /// List of arguments this operation has
    std::vector<Root> attr_args;

    /// Send the "serialno" attribute to an Atlas::Bridge.
    void sendSerialno(Atlas::Bridge&) const;
    /// Send the "refno" attribute to an Atlas::Bridge.
    void sendRefno(Atlas::Bridge&) const;
    /// Send the "from" attribute to an Atlas::Bridge.
    void sendFrom(Atlas::Bridge&) const;
    /// Send the "to" attribute to an Atlas::Bridge.
    void sendTo(Atlas::Bridge&) const;
    /// Send the "seconds" attribute to an Atlas::Bridge.
    void sendSeconds(Atlas::Bridge&) const;
    /// Send the "future_seconds" attribute to an Atlas::Bridge.
    void sendFutureSeconds(Atlas::Bridge&) const;
    /// Send the "args" attribute to an Atlas::Bridge.
    void sendArgs(Atlas::Bridge&) const;

    void iterate(int& current_class, std::string& attr) const override;

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<RootOperationData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    void reset() override;
    void free() override;

private:

    static void fillDefaultObjectInstance(RootOperationData& data, std::map<std::string, uint32_t>& attr_data);
};

//
// Attribute name strings follow.
//

extern const std::string SERIALNO_ATTR;
extern const std::string REFNO_ATTR;
extern const std::string FROM_ATTR;
extern const std::string TO_ATTR;
extern const std::string SECONDS_ATTR;
extern const std::string FUTURE_SECONDS_ATTR;
extern const std::string ARGS_ATTR;

//
// Inlined member functions follow.
//

const uint32_t SERIALNO_FLAG = 1u << 14u;

inline void RootOperationData::setSerialno(long val)
{
    attr_serialno = val;
    m_attrFlags |= SERIALNO_FLAG;
}

const uint32_t REFNO_FLAG = 1u << 15u;

inline void RootOperationData::setRefno(long val)
{
    attr_refno = val;
    m_attrFlags |= REFNO_FLAG;
}

const uint32_t FROM_FLAG = 1u << 16u;

inline void RootOperationData::setFrom(std::string val)
{
    attr_from = std::move(val);
    m_attrFlags |= FROM_FLAG;
}

const uint32_t TO_FLAG = 1u << 17u;

inline void RootOperationData::setTo(std::string val)
{
    attr_to = std::move(val);
    m_attrFlags |= TO_FLAG;
}

const uint32_t SECONDS_FLAG = 1u << 18u;

inline void RootOperationData::setSeconds(double val)
{
    attr_seconds = val;
    m_attrFlags |= SECONDS_FLAG;
}

const uint32_t FUTURE_SECONDS_FLAG = 1u << 19u;

inline void RootOperationData::setFutureSeconds(double val)
{
    attr_future_seconds = val;
    m_attrFlags |= FUTURE_SECONDS_FLAG;
}

const uint32_t ARGS_FLAG = 1u << 20u;

inline void RootOperationData::setArgs(std::vector<Root> val)
{
    attr_args = std::move(val);
    m_attrFlags |= ARGS_FLAG;
}

inline void RootOperationData::setArgsAsList(const Atlas::Message::ListType& val, const Factories* factories)
{
    if (!factories) {
        throw Exception("You must pass in a valid Factories instance when setting 'args'.");
    }
    m_attrFlags |= ARGS_FLAG;
    attr_args = factories->parseListOfObjects(val);
}

template <class ObjectData>
inline void RootOperationData::setArgs1(const SmartPtr<ObjectData>& val)
{
    m_attrFlags |= ARGS_FLAG;
    if(attr_args.size()!=1) attr_args.resize(1);
    attr_args[0] = val;
}

inline long RootOperationData::getSerialno() const
{
    if(m_attrFlags & SERIALNO_FLAG)
        return attr_serialno;
    else
        return ((RootOperationData*)m_defaults)->attr_serialno;
}

inline long& RootOperationData::modifySerialno()
{
    if(!(m_attrFlags & SERIALNO_FLAG))
        setSerialno(((RootOperationData*)m_defaults)->attr_serialno);
    return attr_serialno;
}

inline long RootOperationData::getRefno() const
{
    if(m_attrFlags & REFNO_FLAG)
        return attr_refno;
    else
        return ((RootOperationData*)m_defaults)->attr_refno;
}

inline long& RootOperationData::modifyRefno()
{
    if(!(m_attrFlags & REFNO_FLAG))
        setRefno(((RootOperationData*)m_defaults)->attr_refno);
    return attr_refno;
}

inline const std::string& RootOperationData::getFrom() const
{
    if(m_attrFlags & FROM_FLAG)
        return attr_from;
    else
        return ((RootOperationData*)m_defaults)->attr_from;
}

inline std::string& RootOperationData::modifyFrom()
{
    if(!(m_attrFlags & FROM_FLAG))
        setFrom(((RootOperationData*)m_defaults)->attr_from);
    return attr_from;
}

inline const std::string& RootOperationData::getTo() const
{
    if(m_attrFlags & TO_FLAG)
        return attr_to;
    else
        return ((RootOperationData*)m_defaults)->attr_to;
}

inline std::string& RootOperationData::modifyTo()
{
    if(!(m_attrFlags & TO_FLAG))
        setTo(((RootOperationData*)m_defaults)->attr_to);
    return attr_to;
}

inline double RootOperationData::getSeconds() const
{
    if(m_attrFlags & SECONDS_FLAG)
        return attr_seconds;
    else
        return ((RootOperationData*)m_defaults)->attr_seconds;
}

inline double& RootOperationData::modifySeconds()
{
    if(!(m_attrFlags & SECONDS_FLAG))
        setSeconds(((RootOperationData*)m_defaults)->attr_seconds);
    return attr_seconds;
}

inline double RootOperationData::getFutureSeconds() const
{
    if(m_attrFlags & FUTURE_SECONDS_FLAG)
        return attr_future_seconds;
    else
        return ((RootOperationData*)m_defaults)->attr_future_seconds;
}

inline double& RootOperationData::modifyFutureSeconds()
{
    if(!(m_attrFlags & FUTURE_SECONDS_FLAG))
        setFutureSeconds(((RootOperationData*)m_defaults)->attr_future_seconds);
    return attr_future_seconds;
}

inline const std::vector<Root>& RootOperationData::getArgs() const
{
    if(m_attrFlags & ARGS_FLAG)
        return attr_args;
    else
        return ((RootOperationData*)m_defaults)->attr_args;
}

inline std::vector<Root>& RootOperationData::modifyArgs()
{
    if(!(m_attrFlags & ARGS_FLAG))
        setArgs(((RootOperationData*)m_defaults)->attr_args);
    return attr_args;
}

inline const Atlas::Message::ListType RootOperationData::getArgsAsList() const
{
    const std::vector<Root>& args_in = getArgs();
    Atlas::Message::ListType args_out;
    for (const auto& entry : args_in) {
        args_out.push_back(Atlas::Message::MapType());
        entry->addToMessage(args_out.back().Map());
    }
    return args_out;
}

inline bool RootOperationData::isDefaultSerialno() const
{
    return (m_attrFlags & SERIALNO_FLAG) == 0;
}

inline bool RootOperationData::isDefaultRefno() const
{
    return (m_attrFlags & REFNO_FLAG) == 0;
}

inline bool RootOperationData::isDefaultFrom() const
{
    return (m_attrFlags & FROM_FLAG) == 0;
}

inline bool RootOperationData::isDefaultTo() const
{
    return (m_attrFlags & TO_FLAG) == 0;
}

inline bool RootOperationData::isDefaultSeconds() const
{
    return (m_attrFlags & SECONDS_FLAG) == 0;
}

inline bool RootOperationData::isDefaultFutureSeconds() const
{
    return (m_attrFlags & FUTURE_SECONDS_FLAG) == 0;
}

inline bool RootOperationData::isDefaultArgs() const
{
    return (m_attrFlags & ARGS_FLAG) == 0;
}


} } } // namespace Atlas::Objects::Operation

#endif // ATLAS_OBJECTS_OPERATION_ROOTOPERATION_H
