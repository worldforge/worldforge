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


