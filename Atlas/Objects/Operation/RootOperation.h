// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000 Stefanus Du Toit.
// Automatically generated using gen_cc.py.

#ifndef ATLAS_OBJECTS_OPERATION_ROOTOPERATION_H
#define ATLAS_OBJECTS_OPERATION_ROOTOPERATION_H

#include "../Root.h"


namespace Atlas { namespace Objects { namespace Operation { 

/** Base operation for all operators

This is base operation for all other
    operations and defines basic attributes. You can use this as
    starting point for browsing whole operation hiearchy. refno refers
    to operation this is reply for. In examples all attributes that
    are just as examples (and thus world specific) are started with 'e_'.

*/
class RootOperation : public Root
{
public:
    /// Construct a RootOperation class definition.
    RootOperation();
    /// Default destructor.
    virtual ~RootOperation() { }

    /// Create a new instance of RootOperation.
    static RootOperation Instantiate();

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

    /// Set the "serialno" attribute.
    inline void SetSerialno(int val);
    /// Set the "refno" attribute.
    inline void SetRefno(int val);
    /// Set the "from" attribute.
    inline void SetFrom(const std::string& val);
    /// Set the "to" attribute.
    inline void SetTo(const std::string& val);
    /// Set the "seconds" attribute.
    inline void SetSeconds(double val);
    /// Set the "future_seconds" attribute.
    inline void SetFutureSeconds(double val);
    /// Set the "time_string" attribute.
    inline void SetTimeString(const std::string& val);
    /// Set the "args" attribute.
    inline void SetArgs(const Atlas::Message::Object::ListType& val);

    /// Retrieve the "serialno" attribute.
    inline int GetSerialno() const;
    /// Retrieve the "serialno" attribute as a non-const reference.
    inline int& GetSerialno();
    /// Retrieve the "refno" attribute.
    inline int GetRefno() const;
    /// Retrieve the "refno" attribute as a non-const reference.
    inline int& GetRefno();
    /// Retrieve the "from" attribute.
    inline const std::string& GetFrom() const;
    /// Retrieve the "from" attribute as a non-const reference.
    inline std::string& GetFrom();
    /// Retrieve the "to" attribute.
    inline const std::string& GetTo() const;
    /// Retrieve the "to" attribute as a non-const reference.
    inline std::string& GetTo();
    /// Retrieve the "seconds" attribute.
    inline double GetSeconds() const;
    /// Retrieve the "seconds" attribute as a non-const reference.
    inline double& GetSeconds();
    /// Retrieve the "future_seconds" attribute.
    inline double GetFutureSeconds() const;
    /// Retrieve the "future_seconds" attribute as a non-const reference.
    inline double& GetFutureSeconds();
    /// Retrieve the "time_string" attribute.
    inline const std::string& GetTimeString() const;
    /// Retrieve the "time_string" attribute as a non-const reference.
    inline std::string& GetTimeString();
    /// Retrieve the "args" attribute.
    inline const Atlas::Message::Object::ListType& GetArgs() const;
    /// Retrieve the "args" attribute as a non-const reference.
    inline Atlas::Message::Object::ListType& GetArgs();

protected:
    int attr_serialno;
    int attr_refno;
    std::string attr_from;
    std::string attr_to;
    double attr_seconds;
    double attr_future_seconds;
    std::string attr_time_string;
    Atlas::Message::Object::ListType attr_args;

    inline void SendSerialno(Atlas::Bridge*) const;
    inline void SendRefno(Atlas::Bridge*) const;
    inline void SendFrom(Atlas::Bridge*) const;
    inline void SendTo(Atlas::Bridge*) const;
    inline void SendSeconds(Atlas::Bridge*) const;
    inline void SendFutureSeconds(Atlas::Bridge*) const;
    inline void SendTimeString(Atlas::Bridge*) const;
    inline void SendArgs(Atlas::Bridge*) const;

};

//
// Inlined member functions follow.
//

void RootOperation::SetSerialno(int val)
{
    attr_serialno = val;
}

void RootOperation::SetRefno(int val)
{
    attr_refno = val;
}

void RootOperation::SetFrom(const std::string& val)
{
    attr_from = val;
}

void RootOperation::SetTo(const std::string& val)
{
    attr_to = val;
}

void RootOperation::SetSeconds(double val)
{
    attr_seconds = val;
}

void RootOperation::SetFutureSeconds(double val)
{
    attr_future_seconds = val;
}

void RootOperation::SetTimeString(const std::string& val)
{
    attr_time_string = val;
}

void RootOperation::SetArgs(const Atlas::Message::Object::ListType& val)
{
    attr_args = val;
}

int RootOperation::GetSerialno() const
{
    return attr_serialno;
}

int& RootOperation::GetSerialno()
{
    return attr_serialno;
}

int RootOperation::GetRefno() const
{
    return attr_refno;
}

int& RootOperation::GetRefno()
{
    return attr_refno;
}

const std::string& RootOperation::GetFrom() const
{
    return attr_from;
}

std::string& RootOperation::GetFrom()
{
    return attr_from;
}

const std::string& RootOperation::GetTo() const
{
    return attr_to;
}

std::string& RootOperation::GetTo()
{
    return attr_to;
}

double RootOperation::GetSeconds() const
{
    return attr_seconds;
}

double& RootOperation::GetSeconds()
{
    return attr_seconds;
}

double RootOperation::GetFutureSeconds() const
{
    return attr_future_seconds;
}

double& RootOperation::GetFutureSeconds()
{
    return attr_future_seconds;
}

const std::string& RootOperation::GetTimeString() const
{
    return attr_time_string;
}

std::string& RootOperation::GetTimeString()
{
    return attr_time_string;
}

const Atlas::Message::Object::ListType& RootOperation::GetArgs() const
{
    return attr_args;
}

Atlas::Message::Object::ListType& RootOperation::GetArgs()
{
    return attr_args;
}

void RootOperation::SendSerialno(Atlas::Bridge* b) const
{
    b->MapItem("serialno", attr_serialno);
}

void RootOperation::SendRefno(Atlas::Bridge* b) const
{
    b->MapItem("refno", attr_refno);
}

void RootOperation::SendFrom(Atlas::Bridge* b) const
{
    b->MapItem("from", attr_from);
}

void RootOperation::SendTo(Atlas::Bridge* b) const
{
    b->MapItem("to", attr_to);
}

void RootOperation::SendSeconds(Atlas::Bridge* b) const
{
    b->MapItem("seconds", attr_seconds);
}

void RootOperation::SendFutureSeconds(Atlas::Bridge* b) const
{
    b->MapItem("future_seconds", attr_future_seconds);
}

void RootOperation::SendTimeString(Atlas::Bridge* b) const
{
    b->MapItem("time_string", attr_time_string);
}

void RootOperation::SendArgs(Atlas::Bridge* b) const
{
    Atlas::Message::Encoder e(b);
    e.MapItem("args", attr_args);
}


} } } // namespace Atlas::Objects::Operation

#endif // ATLAS_OBJECTS_OPERATION_ROOTOPERATION_H
