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
    virtual void responseReceived(const Atlas::Objects::Operation::RootOperation& op) = 0;
};


template <class T>
class MemberResponse : public ResponseBase
{
public:
   typedef void (T::*T_method)(const Atlas::Objects::Operation::RootOperation& op);

    MemberResponse(T *obj, void (T::*method)(const Atlas::Objects::Operation::RootOperation& op)) :
		m_object(obj),
		m_func(method)
	{
	}
	
	virtual void responseReceived(const Atlas::Objects::Operation::RootOperation& op)
	{
		(m_object->*m_func)(op);
	}

private:
    T* m_object;
    T_method m_func;
};

class ResponseTracker
{
public:
    void await(int serialno, ResponseBase*);
    
    template <class T>
    void await(int serial, T* ins, void (T::*method)(const Atlas::Objects::Operation::RootOperation& op) )
    {
        await(serial, new MemberResponse<T>(ins, method));
    }
    
    bool handleOp(const Atlas::Objects::Operation::RootOperation& op);

private:
    typedef std::map<int, ResponseBase*> RefnoResponseMap;
    RefnoResponseMap m_pending;
};

} // of namespace

#endif // of ERIS_RESPONSE_H
