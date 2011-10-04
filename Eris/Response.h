#ifndef ERIS_RESPONSE_H
#define ERIS_RESPONSE_H

#include <Atlas/Objects/ObjectsFwd.h>
#include <map>

namespace Eris
{

class ResponseBase
{
public:
    virtual ~ResponseBase();
    
    /**
    Process a response. Return true if the operation was handled, and false
    if it should be processed by the router system as normal.
    */
    virtual bool responseReceived(const Atlas::Objects::Operation::RootOperation& op) = 0;
};

class NullResponse : public ResponseBase
{
public:
    virtual bool responseReceived(const Atlas::Objects::Operation::RootOperation&);
};

void* clearMemberResponse(void*);

template <class T>
class MemberResponse : public ResponseBase
{
public:
   typedef void (T::*T_method)(const Atlas::Objects::Operation::RootOperation& op);

    MemberResponse(T *obj, void (T::*method)(const Atlas::Objects::Operation::RootOperation& op)) :
		m_object(obj),
		m_func(method)
	{
        obj->add_destroy_notify_callback(&m_object, &clearMemberResponse);
	}
	
    ~MemberResponse()
    {
        if (m_object) m_object->remove_destroy_notify_callback(&m_object);
    }
    
	virtual bool responseReceived(const Atlas::Objects::Operation::RootOperation& op)
	{
        if (m_object) (m_object->*m_func)(op);
        return true;
	}

private:
    T* m_object;
    T_method m_func;
};

class ResponseTracker
{
public:

    ~ResponseTracker();

    void await(int serialno, ResponseBase*);
    
    template <class T>
    void await(int serial, T* ins, void (T::*method)(const Atlas::Objects::Operation::RootOperation& op) )
    {
        await(serial, new MemberResponse<T>(ins, method));
    }
    
    void ignore(int serial)
    {
        await(serial, new NullResponse());
    }
    
    bool handleOp(const Atlas::Objects::Operation::RootOperation& op);

private:
    typedef std::map<int, ResponseBase*> RefnoResponseMap;
    RefnoResponseMap m_pending;
};

} // of namespace

#endif // of ERIS_RESPONSE_H
