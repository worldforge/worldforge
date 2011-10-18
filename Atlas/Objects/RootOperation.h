// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Stefanus Du Toit and Aloril.
// Copyright 2001-2005 Alistair Riddoch.
// Automatically generated using gen_cpp.py.

#ifndef ATLAS_OBJECTS_OPERATION_ROOTOPERATION_H
#define ATLAS_OBJECTS_OPERATION_ROOTOPERATION_H

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#include <Atlas/Objects/objectFactory.h>

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
    RootOperationData(RootOperationData *defaults = NULL) : 
        RootData((RootData*)defaults)
    {
        m_class_no = ROOT_OPERATION_NO;
    }
    /// Default destructor.
    virtual ~RootOperationData();

public:
    /// Copy this object.
    virtual RootOperationData * copy() const;

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

    /// Set the "serialno" attribute.
    inline void setSerialno(long val);
    /// Set the "refno" attribute.
    inline void setRefno(long val);
    /// Set the "from" attribute.
    inline void setFrom(const std::string& val);
    /// Set the "to" attribute.
    inline void setTo(const std::string& val);
    /// Set the "seconds" attribute.
    inline void setSeconds(double val);
    /// Set the "future_seconds" attribute.
    inline void setFutureSeconds(double val);
    /// Set the "args" attribute.
    inline void setArgs(const std::vector<Root>& val);
    /// Set the "args" attribute AsList.
    inline void setArgsAsList(const Atlas::Message::ListType& val);
    /// Set the first member of "args"
    template <class ObjectData>
    inline void setArgs1(const SmartPtr<ObjectData> & val);

    /// Retrieve the "serialno" attribute.
    inline long getSerialno() const;
    /// Retrieve the "serialno" attribute as a non-const reference.
    inline long& modifySerialno();
    /// Retrieve the "refno" attribute.
    inline long getRefno() const;
    /// Retrieve the "refno" attribute as a non-const reference.
    inline long& modifyRefno();
    /// Retrieve the "from" attribute.
    inline const std::string& getFrom() const;
    /// Retrieve the "from" attribute as a non-const reference.
    inline std::string& modifyFrom();
    /// Retrieve the "to" attribute.
    inline const std::string& getTo() const;
    /// Retrieve the "to" attribute as a non-const reference.
    inline std::string& modifyTo();
    /// Retrieve the "seconds" attribute.
    inline double getSeconds() const;
    /// Retrieve the "seconds" attribute as a non-const reference.
    inline double& modifySeconds();
    /// Retrieve the "future_seconds" attribute.
    inline double getFutureSeconds() const;
    /// Retrieve the "future_seconds" attribute as a non-const reference.
    inline double& modifyFutureSeconds();
    /// Retrieve the "args" attribute.
    inline const std::vector<Root>& getArgs() const;
    /// Retrieve the "args" attribute as a non-const reference.
    inline std::vector<Root>& modifyArgs();
    /// Retrieve the "args" attribute AsList.
    inline const Atlas::Message::ListType getArgsAsList() const;

    /// Is "serialno" value default?
    inline bool isDefaultSerialno() const;
    /// Is "refno" value default?
    inline bool isDefaultRefno() const;
    /// Is "from" value default?
    inline bool isDefaultFrom() const;
    /// Is "to" value default?
    inline bool isDefaultTo() const;
    /// Is "seconds" value default?
    inline bool isDefaultSeconds() const;
    /// Is "future_seconds" value default?
    inline bool isDefaultFutureSeconds() const;
    /// Is "args" value default?
    inline bool isDefaultArgs() const;

protected:
    /// Find the class which contains the attribute "name".
    virtual int getAttrClass(const std::string& name)const;
    /// Find the flag for the attribute "name".
    virtual int getAttrFlag(const std::string& name)const;
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

    virtual void iterate(int& current_class, std::string& attr) const;

public:
    template <typename>
    friend class ::Atlas::Objects::Allocator;
    static Allocator<RootOperationData> allocator;

protected:
    ///Resets the object as it's returned to the pool.
    virtual void reset();

private:
    virtual void free();

    static void fillDefaultObjectInstance(RootOperationData& data, std::map<std::string, int>& attr_data);
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

const int SERIALNO_FLAG = 1 << 14;

void RootOperationData::setSerialno(long val)
{
    attr_serialno = val;
    m_attrFlags |= SERIALNO_FLAG;
}

const int REFNO_FLAG = 1 << 15;

void RootOperationData::setRefno(long val)
{
    attr_refno = val;
    m_attrFlags |= REFNO_FLAG;
}

const int FROM_FLAG = 1 << 16;

void RootOperationData::setFrom(const std::string& val)
{
    attr_from = val;
    m_attrFlags |= FROM_FLAG;
}

const int TO_FLAG = 1 << 17;

void RootOperationData::setTo(const std::string& val)
{
    attr_to = val;
    m_attrFlags |= TO_FLAG;
}

const int SECONDS_FLAG = 1 << 18;

void RootOperationData::setSeconds(double val)
{
    attr_seconds = val;
    m_attrFlags |= SECONDS_FLAG;
}

const int FUTURE_SECONDS_FLAG = 1 << 19;

void RootOperationData::setFutureSeconds(double val)
{
    attr_future_seconds = val;
    m_attrFlags |= FUTURE_SECONDS_FLAG;
}

const int ARGS_FLAG = 1 << 20;

void RootOperationData::setArgs(const std::vector<Root>& val)
{
    attr_args = val;
    m_attrFlags |= ARGS_FLAG;
}

void RootOperationData::setArgsAsList(const Atlas::Message::ListType& val)
{
    m_attrFlags |= ARGS_FLAG;
    attr_args.resize(0);
    for(Message::ListType::const_iterator I = val.begin();
        I != val.end();
        I++)
    {
        if (I->isMap()) {
            attr_args.push_back(Factories::instance()->createObject(I->asMap()));
        }
    }
}

template <class ObjectData>
void RootOperationData::setArgs1(const SmartPtr<ObjectData>& val)
{
    m_attrFlags |= ARGS_FLAG;
    if(attr_args.size()!=1) attr_args.resize(1);
    attr_args[0] = val;
}

long RootOperationData::getSerialno() const
{
    if(m_attrFlags & SERIALNO_FLAG)
        return attr_serialno;
    else
        return ((RootOperationData*)m_defaults)->attr_serialno;
}

long& RootOperationData::modifySerialno()
{
    if(!(m_attrFlags & SERIALNO_FLAG))
        setSerialno(((RootOperationData*)m_defaults)->attr_serialno);
    return attr_serialno;
}

long RootOperationData::getRefno() const
{
    if(m_attrFlags & REFNO_FLAG)
        return attr_refno;
    else
        return ((RootOperationData*)m_defaults)->attr_refno;
}

long& RootOperationData::modifyRefno()
{
    if(!(m_attrFlags & REFNO_FLAG))
        setRefno(((RootOperationData*)m_defaults)->attr_refno);
    return attr_refno;
}

const std::string& RootOperationData::getFrom() const
{
    if(m_attrFlags & FROM_FLAG)
        return attr_from;
    else
        return ((RootOperationData*)m_defaults)->attr_from;
}

std::string& RootOperationData::modifyFrom()
{
    if(!(m_attrFlags & FROM_FLAG))
        setFrom(((RootOperationData*)m_defaults)->attr_from);
    return attr_from;
}

const std::string& RootOperationData::getTo() const
{
    if(m_attrFlags & TO_FLAG)
        return attr_to;
    else
        return ((RootOperationData*)m_defaults)->attr_to;
}

std::string& RootOperationData::modifyTo()
{
    if(!(m_attrFlags & TO_FLAG))
        setTo(((RootOperationData*)m_defaults)->attr_to);
    return attr_to;
}

double RootOperationData::getSeconds() const
{
    if(m_attrFlags & SECONDS_FLAG)
        return attr_seconds;
    else
        return ((RootOperationData*)m_defaults)->attr_seconds;
}

double& RootOperationData::modifySeconds()
{
    if(!(m_attrFlags & SECONDS_FLAG))
        setSeconds(((RootOperationData*)m_defaults)->attr_seconds);
    return attr_seconds;
}

double RootOperationData::getFutureSeconds() const
{
    if(m_attrFlags & FUTURE_SECONDS_FLAG)
        return attr_future_seconds;
    else
        return ((RootOperationData*)m_defaults)->attr_future_seconds;
}

double& RootOperationData::modifyFutureSeconds()
{
    if(!(m_attrFlags & FUTURE_SECONDS_FLAG))
        setFutureSeconds(((RootOperationData*)m_defaults)->attr_future_seconds);
    return attr_future_seconds;
}

const std::vector<Root>& RootOperationData::getArgs() const
{
    if(m_attrFlags & ARGS_FLAG)
        return attr_args;
    else
        return ((RootOperationData*)m_defaults)->attr_args;
}

std::vector<Root>& RootOperationData::modifyArgs()
{
    if(!(m_attrFlags & ARGS_FLAG))
        setArgs(((RootOperationData*)m_defaults)->attr_args);
    return attr_args;
}

const Atlas::Message::ListType RootOperationData::getArgsAsList() const
{
    const std::vector<Root>& args_in = getArgs();
    Atlas::Message::ListType args_out;
    for(std::vector<Root>::const_iterator I = args_in.begin();
        I != args_in.end();
        I++)
    {
        args_out.push_back(Atlas::Message::MapType());
        (*I)->addToMessage(args_out.back().asMap());
    }
    return args_out;
}

bool RootOperationData::isDefaultSerialno() const
{
    return (m_attrFlags & SERIALNO_FLAG) == 0;
}

bool RootOperationData::isDefaultRefno() const
{
    return (m_attrFlags & REFNO_FLAG) == 0;
}

bool RootOperationData::isDefaultFrom() const
{
    return (m_attrFlags & FROM_FLAG) == 0;
}

bool RootOperationData::isDefaultTo() const
{
    return (m_attrFlags & TO_FLAG) == 0;
}

bool RootOperationData::isDefaultSeconds() const
{
    return (m_attrFlags & SECONDS_FLAG) == 0;
}

bool RootOperationData::isDefaultFutureSeconds() const
{
    return (m_attrFlags & FUTURE_SECONDS_FLAG) == 0;
}

bool RootOperationData::isDefaultArgs() const
{
    return (m_attrFlags & ARGS_FLAG) == 0;
}


} } } // namespace Atlas::Objects::Operation

#endif // ATLAS_OBJECTS_OPERATION_ROOTOPERATION_H
