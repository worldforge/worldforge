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
#include <multimap.h>
using namespace std;

#include <Atlas.h>

#include "Arg.h"

class fptr_abstract {
public:
    virtual void use(const AObject& msg) = 0;
};

template<class T>
class fptr : public fptr_abstract {
private:
    typedef void (T::*ptr_t)(const AObject&);
    T& m_obj;
    ptr_t m_ptr;
public:
    fptr(T& obj, ptr_t ptr)
      : m_obj(obj), m_ptr(ptr)
    {
    }

    void use(const AObject& msg)
    {
        (m_obj.*m_ptr)(msg);
    }

    const T& get_obj() const {return m_obj;}
    const ptr_t get_ptr() const {return m_ptr;}
};


class AUserClient : public AClient{
public:
    
    AUserClient(ASocket* socket, ACodec* codec) : AClient(socket, codec),
                                        m_serialno(0), m_nextserialno(1) { }
    virtual void gotMsg(const AObject& msg);
    
    virtual AObject call(const AObject& msg);

    void addMsgHandler(const string& type, void(*hdl)(const AObject&));
    void remMsgHandler(const string& type, void(*hdl)(const AObject&));
    
    template<class T> void addMsgHandler(const string& type, T& obj,
                                         void(T::*handler)(const AObject&));
    template<class T> void remMsgHandler(const string& type, T& obj,
                                         void(T::*handler)(const AObject&));

    AObject createOperation(const string& id, Arg* args ...);
    AObject createEntity(Arg* args ...);

    AObject setCharacterArgs(const string& id, Arg* args ...);
    
private:

    long m_serialno;
    long m_nextserialno;

    AObject m_reply;
    multimap<string, fptr_abstract*> m_msghandlers;
    multimap<string, void(*)(const AObject&)> m_msghandlers_simp;
};

// need to put these here so all instantiations get compiled properly

template<class T> void AUserClient::addMsgHandler(const string& type,
                              T& obj, void(T::*handler)(const AObject&))
{
    m_msghandlers.insert(m_msghandlers.end(),
               pair<string, fptr_abstract*>(type, new fptr<T>(obj, handler)));
}

template<class T> void AUserClient::remMsgHandler(const string& type,
                              T& obj, void(T::*handler)(const AObject&))
{
    multimap<string, fptr_abstract*>::iterator I;
    bool quit = false;
    for (I = m_msghandlers.begin(); I != m_msghandlers.end() && !quit; I++) {
        if (((*I).first == type) && (*(*I).second == fptr<T>(obj, handler))) {
            m_msghandlers.erase(I);
            quit = true;
        }
    }
}


#endif
