/*
        AtlasUserClient.h
        ----------------
        begin           : 1999.12.30
        copyright       : (C) 1999 by Stefanus Du Toit
        email           : sdt@gmx.net
*/

#ifndef __UserClient_h_
#define __UserClient_h_

#include <iostream>
#include <string>
#include <list>
using std::ostream;
using std::string;
using std::list;

#include "../Net/Net.h"
#include "Arg.h"

namespace Atlas
{
    
/// Helper class for UserClient
class fptr_abstract {
public:
    virtual void use(const Object& msg) = 0;
};

/// Helper class for UserClient
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

/**
UserClient provides some useful functions that correspond with the Atlas
operations model and a message handler with callback support.
**/
class UserClient : public Client{
public:

    ///    
    UserClient(Socket* socket, Codec* codec) :
        Client(socket, codec), m_serialno(0), m_nextserialno(1)
    {
    }

    ///
    virtual ~UserClient();
    
    /// Looks up message type in handler list and calls appropriate handler
    virtual void gotMsg(const Object& msg);
    /// Send a message to other end and wait for a reply
    virtual Object call(const Object& msg);
    /// Add a new message handler
    template<class T>
    void addMsgHandler(const string& type, T& obj,
                       void(T::*handler)(const Object&))
    {
        m_msghandlers.insert(m_msghandlers.end(),
                pair<string, fptr_abstract*>(type, new fptr<T>(obj, handler)));
    }
    /// Remove an existing message handler
    template<class T>
    void remMsgHandler(const string& type, T& obj,
                       void(T::*handler)(const Object&))
    {
        list< pair<string, fptr_abstract*> >::iterator I;
        bool quit = false;
        for (I = m_msghandlers.begin(); I != m_msghandlers.end() && !quit; I++)
        {
            if (((*I).first == type)
                && (*(*I).second == fptr<T>(obj, handler))) {
                    delete (*I).second;
                    m_msghandlers.erase(I);
                    quit = true;
            }
        }
    }										 
    /// Create an operation corresponding to the Atlas operations model
    Object createOperation(const string& id, Arg* args ...);
    /// Create an entity with named attributes
    Object createEntity(Arg* args ...);
    /// Set arguments for a player character
    Object setCharacterArgs(const string& id, Arg* args ...);
    /// Parse an Atlas operation with 1 argument
    int parseOp1Arg(const Object &op, string method, Object &arg0);
    /// Display an Atlas error to an output stream
    void displayError(ostream& out, const Object &op, string ourMsg);
    
private:

    long m_serialno;
    long m_nextserialno;

    Object m_reply;
    list< pair<string, fptr_abstract*> > m_msghandlers;
    list< pair<string, void(*)(const Object&)> > m_msghandlers_simp;
};

} // end namespace Atlas

#endif
