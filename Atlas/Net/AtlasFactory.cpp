/*
        AtlasFactory.cpp
        ----------------
        begin           : 1999.12.28
        copyright       : (C) 1999 by Stefanus Du Toit
        email           : sdt@gmx.net
*/

#include "AtlasFactory.h"

Arg::Arg()
 : m_type(AINVALID), m_name("")
{
}

Arg::Arg(const string& name, const string& value)
 : m_type(ASTRING), m_name(name), m_valString(value)
{
}

Arg::Arg(const string& name, long value)
 : m_type(AINT), m_name(name), m_valInt(value)
{
}

Arg::Arg(const string& name, double value)
 : m_type(AFLOAT), m_name(name), m_valFloat(value)
{
}

Arg::Arg(const string& name, const AObject& value)
 : m_type(AOBJECT), m_name(name), m_valObject(value)
{
}

Arg::Arg(const string& value)
 : m_type(ASTRING), m_name(""), m_valString(value)
{
}

Arg::Arg(long value)
 : m_type(AINT), m_name(""), m_valInt(value)
{
}

Arg::Arg(double value)
 : m_type(AFLOAT), m_name(""), m_valFloat(value)
{
}

Arg::Arg(const AObject& value)
 : m_type(AOBJECT), m_name(""), m_valObject(value)
{
}

atype_t Arg::getType() const
{
    return m_type;
}

const string& Arg::getName() const 
{
    return m_name;
}
    
const string& Arg::getString() const
{
    return m_valString;
}

long Arg::getInt() const
{
    return m_valInt;
}

double Arg::getFloat() const
{
    return m_valFloat;
}

const AObject& Arg::getObject() const
{
    return m_valObject;
}

Arg* A(const string& id, const string& value)
{
    return new Arg(id, value);
}

Arg* A(const string& id, long value)
{
    return new Arg(id, value);
}

Arg* A(const string& id, double value)
{
    return new Arg(id, value);
}

Arg* A(const string& id, const AObject& value)
{
    return new Arg(id, value);
}

Arg* A(const string& value)
{
    return new Arg(value);
}

Arg* A(long value)
{
    return new Arg(value);
}

Arg* A(double value)
{
    return new Arg(value);
}

Arg* A(const AObject& value)
{
    return new Arg(value);
}

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


