#ifndef ERIS_RESPONSE_H
#define ERIS_RESPONSE_H

#include <Atlas/Objects/ObjectsFwd.h>
#include <unordered_map>
#include "Router.h"
#include <functional>

namespace Eris
{

std::string getErrorMessage(const Atlas::Objects::Operation::RootOperation & err);


class ResponseBase
{
public:
    virtual ~ResponseBase();
    
    /**
    Process a response.
    */
    virtual Router::RouterResult responseReceived(const Atlas::Objects::Operation::RootOperation& op) = 0;
};

class NullResponse : public ResponseBase
{
public:
	Router::RouterResult responseReceived(const Atlas::Objects::Operation::RootOperation&) override;
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
	
    ~MemberResponse() override {
        if (m_object) m_object->remove_destroy_notify_callback(&m_object);
    }

	Router::RouterResult responseReceived(const Atlas::Objects::Operation::RootOperation& op) override {
        if (m_object) (m_object->*m_func)(op);
        return Router::HANDLED;
	}

private:
    T* m_object;
    T_method m_func;
};

class ResponseTracker
{
public:

	typedef std::function<Router::RouterResult(const Atlas::Objects::Operation::RootOperation& op)> Callback;

    ~ResponseTracker();

    void await(long serialno, ResponseBase*);

    void await(long serial, Callback callback);
    
    template <class T>
    void await(long serial, T* ins, void (T::*method)(const Atlas::Objects::Operation::RootOperation& op) )
    {
        await(serial, new MemberResponse<T>(ins, method));
    }
    
    void ignore(long serial)
    {
        await(serial, new NullResponse());
    }
    
    Router::RouterResult handleOp(const Atlas::Objects::Operation::RootOperation& op);

private:
    typedef std::unordered_map<long, ResponseBase*> RefnoResponseMap;
//    RefnoResponseMap m_pending;
    std::unordered_map<long, Callback> m_pending;
};

} // of namespace

#endif // of ERIS_RESPONSE_H
