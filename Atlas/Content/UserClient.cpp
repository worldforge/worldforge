/*
        AtlasUserClient.cpp
        ----------------
        begin           : 1999.12.30
        copyright       : (C) 1999 by Stefanus Du Toit
        email           : sdt@gmx.net
*/

#include <iostream>
#include "UserClient.h"
using namespace std;

namespace Atlas {
  
UserClient::~UserClient()
{
}

void UserClient::gotMsg(const Object& msg)
{
    long refno;
    msg.get("refno", refno, 0);
    if((m_serialno) && (refno == m_serialno)) {
        m_reply = msg;
        m_serialno = 0;
        return;
    }
    // Lookup message handlers and call them
    list< pair<string, fptr_abstract*> >::iterator I;
    bool found = false;
    Object parent; string msgOp;
    msg.get("parent", parent);
    Object msgOpObj;
    parent.get(0, msgOpObj);
    msgOp=msgOpObj.asString();
    for (I = m_msghandlers.begin(); I != m_msghandlers.end(); I++) {
        if ((*I).first == msgOp) {
            (*I).second->use(msg);
            found = true;
        }
    }
    list< pair<string, void(*)(const Object&)> >::iterator J;
    for (J = m_msghandlers_simp.begin(); J != m_msghandlers_simp.end(); J++) {
        if ((*J).first == msgOp) {
            (*(*J).second)(msg);
            found = true;
        }
    }
    if (!found)
        cerr << "Warning, could not find handler for message of type "
             << msgOp << "!\n";
}


Object UserClient::call(const Object& msg)
{
    msg.get("serialno", m_serialno);
    sendMsg(msg);
    while (m_serialno) {
        doPoll();
    }
    return m_reply;
}

Object UserClient::createOperation(const string& id, Arg* args ...)
{
  //example result:
  // <obj>
  //     <map>
  //         <string name="abstract_type">operation</string>
  //         <list name="parent">
  //             <string>login</string>
  //         </list>
  //         <int name="serialno">1</int>
  //         <list name="args">
  //         </list>
  //     </map>
  // </obj>
    Object op;
    op.set("abstract_type", "operation");
    Object parent(List);
    parent.append(id);
    op.set("parent", parent);
    op.set("serialno", m_nextserialno++);

    Object arglist = Object::mkList(0);

    va_list ap;
    va_start(ap, args);

    for (bool quit = false, i = false; quit == false; ) {
        Arg* arg;
        if (!i) arg = args; else arg = va_arg(ap, Arg*);
        if (arg == AEND) quit = true;
        else if (arg->getName() == "") arglist.append(arg->getVal());
        else op.set(arg->getName(), arg->getVal());
        
        if (arg) delete arg;
        if (!i) i = !i;
    }
    
    va_end(ap);

    op.set("args", arglist);
    
    return op;
}

Object UserClient::createEntity(Arg* args ...)
{
    Object ent;
    
    va_list ap;
    va_start(ap, args);

    for (bool quit = false, i = false; quit == false;) {
        Arg* arg;
        if (!i) arg = args; else arg = va_arg(ap, Arg*);
        if (arg == AEND) quit = true;
        else ent.set(arg->getName(), arg->getVal());
        
        if (arg) delete arg;
        if (!i) i = !i;
    }

    va_end(ap);
    
    return ent;
}

Object UserClient::setCharacterArgs(const string& id, Arg* args ...)
{
    Object ent = createEntity(A("id",id), AEND);
    
    va_list ap;
    va_start(ap, args);

    for (bool quit = false, i = false; quit == false;) {
        Arg* arg;
        if (!i) arg = args; else arg = va_arg(ap, Arg*);
        if (arg == AEND) quit = true;
          else ent.set(arg->getName(), arg->getVal());
        
        if (arg) delete arg;
        if (!i) i = !i;
    }

    va_end(ap);
    
    Object op = createOperation("set", A(ent), A("from", id), AEND);
    return call(op);
}

int UserClient::parseOp1Arg(const Object &op, string method, Object &arg0)
{
    Object parent, args;
    string abstract_type, emptyString="";
    if (!op.get("abstract_type",abstract_type,emptyString)) return 0;
    if (abstract_type != "operation") return 0;
    if (!op.get("parent",parent)) return 0;
    Object opMethodObj;
    string opMethod;
    if (!parent.get(0,opMethod)) return 0;
    if (method != opMethod) return 0;
    if (!op.get("args",args)) return 0;
    if (args.length() != 1) return 0;
    if (!args.get(0,arg0)) return 0;
    return 1;
}

void UserClient::displayError(ostream& out, const Object &op, string ourMsg)
{
    Object args, arg0;
    out << ourMsg;
    op.get("args",args);
    args.get(0,arg0);
    string message, defMessage="Unknown Error";
    arg0.get("message", message, defMessage);
    out<<message<<endl;
}

}

