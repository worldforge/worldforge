/*
        AtlasUserClient.h
        ----------------
        begin           : 1999.12.30
        copyright       : (C) 1999 by Stefanus Du Toit
        email           : sdt@gmx.net
*/

#ifndef __AtlasUserClient_h_
#define __AtlasUserClient_h_

#include <string>

#include <list>
using namespace std;

#include "../Net/Net.h"

using namespace Atlas;

#include "Arg.h"

class fptr_abstract {
public:
    virtual void use(const Object& msg) = 0;
};

template<class T>
class fptr : public fptr_abstract {
private:
    typedef void (T::*ptr_t)(const Object&);
    T& m_obj;
    ptr_t m_ptr;
public:
    fptr(T& obj, ptr_t ptr)
      : m_obj(obj), m_ptr(ptr)
    {
    }

    void use(const Object& msg)
    {
        (m_obj.*m_ptr)(msg);
    }

    const T& get_obj() const {return m_obj;}
    const ptr_t get_ptr() const {return m_ptr;}
};


class AUserClient : public Client{
public:
    
    AUserClient(Socket* socket, Codec* codec) : Client(socket, codec),
                                        m_serialno(0), m_nextserialno(1) { }
    virtual void gotMsg(const Object& msg);
    
    virtual Object call(const Object& msg);

    template<class T> void addMsgHandler(const string& type, T& obj, void(T::*handler)(const Object&))
	{
		m_msghandlers.insert(m_msghandlers.end(),
			   pair<string, fptr_abstract*>(type, new fptr<T>(obj, handler)));

	}
    
	template<class T> void remMsgHandler(const string& type, T& obj, void(T::*handler)(const Object&))
	{
		list< pair<string, fptr_abstract*> >::iterator I;
		bool quit = false;
		for (I = m_msghandlers.begin(); I != m_msghandlers.end() && !quit; I++) 
		{
			if (((*I).first == type) && (*(*I).second == fptr<T>(obj, handler))) 
			{
				delete (*I).second;
				m_msghandlers.erase(I);
				quit = true;
			}
		}
	}
										 

    Object createOperation(const string& id, Arg* args ...);
    Object createEntity(Arg* args ...);

    Object setCharacterArgs(const string& id, Arg* args ...);

    int parseOperation1Argument(const Object &op, string method, Object &arg0);
    void displayError(ostream& out, const Object &op, string ourMsg);
    
private:

    long m_serialno;
    long m_nextserialno;

    Object m_reply;
    list< pair<string, fptr_abstract*> > m_msghandlers;
    list< pair<string, void(*)(const Object&)> > m_msghandlers_simp;
};

#endif
