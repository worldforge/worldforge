/*
        AtlasUserClient.cpp
        ----------------
        begin           : 1999.12.30
        copyright       : (C) 1999 by Stefanus Du Toit
        email           : sdt@gmx.net
*/

#include "AtlasUserClient.h"

void AUserClient::gotMsg(const AObject& msg)
{
    long refno;
    msg.get("refno", refno, 0);
    if((m_serialno) && (refno == m_serialno)) {
        m_reply = msg;
        m_serialno = 0;
        return;
    }
    // Lookup message handlers and call them
    multimap<string, fptr_abstract*>::iterator I;
    bool found = false;
    AObject parent; string msgOp;
    msg.get("parent", parent);
    parent.get(0, msgOp);
    for (I = m_msghandlers.begin(); I != m_msghandlers.end(); I++) {
        if ((*I).first == msgOp) {
            (*I).second->use(msg);
            found = true;
        }
    }
    if (!found) cerr << "Warning, could not find handler for message!\n";
}


AObject AUserClient::call(const AObject& msg)
{
    msg.get("serialno", m_serialno);
    sendMsg(msg);
    while (m_serialno) {
        doPoll();
    }
    return m_reply;
}


AObject AUserClient::createOperation(const string& id, Arg* args ...)
{
    AObject op;
    op.set("abstract_type", "operation");
    AObject parent = AObject::mkList(0);
    parent.append(id);
    op.set("parent", parent);
    op.set("serialno", m_nextserialno++);

    AObject arglist = AObject::mkList(0);

    va_list ap;
    va_start(ap, args);

    for (bool quit = false, i = false; quit == false; ) {
        Arg* arg;
        if (!i) arg = args; else arg = va_arg(ap, Arg*);
        if (arg->getName() == "") {
            switch (arg->getType()) {
                case AINT:    arglist.append(arg->getInt());
                              break;
                case AFLOAT:  arglist.append(arg->getFloat());
                              break;
                case ASTRING: arglist.append(arg->getString());
                              break;
                case AOBJECT: arglist.append(arg->getObject());
                              break;
                case AINVALID: quit = true;
                               break;
                default:      break;
            }
        } else {
            switch (arg->getType()) {
                case AINT:    op.set(arg->getName(), arg->getInt());
                              break;
                case AFLOAT:  op.set(arg->getName(), arg->getFloat());
                              break;
                case ASTRING: op.set(arg->getName(), arg->getString());
                              break;
                case AOBJECT: op.set(arg->getName(), arg->getObject());
                              break;
                case AINVALID: quit = true;
                               break;
                default:      break;
            }
        }
        delete arg;
        if (!i) i = !i;
    }
    
    va_end(ap);

    op.set("args", arglist);
    
    return op;
}

AObject AUserClient::createEntity(Arg* args ...)
{
    AObject ent;
    
    va_list ap;
    va_start(ap, args);

    for (bool quit = false, i = false; quit == false;) {
        Arg* arg;
        if (!i) arg = args; else arg = va_arg(ap, Arg*);
        switch (arg->getType()) {
            case AINT:    ent.set(arg->getName(), arg->getInt());
                          break;
            case AFLOAT:  ent.set(arg->getName(), arg->getFloat());
                          break;
            case ASTRING: ent.set(arg->getName(), arg->getString());
                          break;
            case AOBJECT: ent.set(arg->getName(), arg->getObject());
                          break;
            case AINVALID: quit = true;
                           break;
            default:      break;
        }
        delete arg;
        if (!i) i = !i;
    }

    va_end(ap);
    
    return ent;
}

AObject AUserClient::setCharacterArgs(const string& id, Arg* args ...)
{
    AObject ent = createEntity(A("id",id), AEND);
    
    va_list ap;
    va_start(ap, args);

    for (bool quit = false, i = false; quit == false;) {
        Arg* arg;
        if (!i) arg = args; else arg = va_arg(ap, Arg*);
        switch (arg->getType()) {
            case AINT:    ent.set(arg->getName(), arg->getInt());
                          break;
            case AFLOAT:  ent.set(arg->getName(), arg->getFloat());
                          break;
            case ASTRING: ent.set(arg->getName(), arg->getString());
                          break;
            case AOBJECT: ent.set(arg->getName(), arg->getObject());
                          break;
            case AINVALID: quit = true;
                           break;
            default:      break;
        }
        delete arg;
        if (!i) i = !i;
    }

    va_end(ap);
    
    AObject op = createOperation("set", A(ent), A("from", id), AEND);
    return call(op);
}
