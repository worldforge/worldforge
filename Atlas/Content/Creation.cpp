/*
        AtlasFactory.cpp
        ----------------
        begin           : 1999.12.28
        copyright       : (C) 1999 by Stefanus Du Toit
        email           : sdt@gmx.net
*/

#include "Creation.h"

AObject createOperation(const string& id, long serialno, Arg* args ...)
{
    AObject op;
    op.set("abstract_type", "operation");
    AObject parent = AObject::mkList(0);
    parent.append(id);
    op.set("parent", parent);
    op.set("serialno", serialno);

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

AObject createEntity(Arg* args ...)
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


