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
    RootOperation();
    virtual ~RootOperation() { }

    static RootOperation Instantiate();

    virtual bool HasAttr(const std::string& name)const;
    virtual Atlas::Message::Object GetAttr(const std::string& name)
            const throw (NoSuchAttrException);
    virtual void SetAttr(const std::string& name,
                         const Atlas::Message::Object& attr);
    virtual void RemoveAttr(const std::string& name);

    inline void SetSerialno(int val);
    inline void SetRefno(int val);
    inline void SetFrom(const std::string& val);
    inline void SetTo(const std::string& val);
    inline void SetSeconds(double val);
    inline void SetFutureSeconds(double val);
    inline void SetTimeString(const std::string& val);
    inline void SetArgs(const Atlas::Message::Object::ListType& val);

    inline int GetSerialno() const;
    inline int GetRefno() const;
    inline const std::string& GetFrom() const;
    inline const std::string& GetTo() const;
    inline double GetSeconds() const;
    inline double GetFutureSeconds() const;
    inline const std::string& GetTimeString() const;
    inline const Atlas::Message::Object::ListType& GetArgs() const;

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

int RootOperation::GetRefno() const
{
    return attr_refno;
}

const std::string& RootOperation::GetFrom() const
{
    return attr_from;
}

const std::string& RootOperation::GetTo() const
{
    return attr_to;
}

double RootOperation::GetSeconds() const
{
    return attr_seconds;
}

double RootOperation::GetFutureSeconds() const
{
    return attr_future_seconds;
}

const std::string& RootOperation::GetTimeString() const
{
    return attr_time_string;
}

const Atlas::Message::Object::ListType& RootOperation::GetArgs() const
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
