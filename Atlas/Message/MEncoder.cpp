// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include <string>
#include "Encoder.h"

using namespace Atlas;
using namespace std;

namespace Atlas { namespace Message {

Encoder::Encoder(Atlas::Bridge* b)
    : EncoderBase(b)
{
}

void Encoder::ListItem(const Object& obj)
{
    if (obj.Is(Object::Int)) b->ListItem(obj.As(Object::Int));
    else if (obj.Is(Object::Float)) b->ListItem(obj.As(Object::Float));
    else if (obj.Is(Object::String)) b->ListItem(obj.As(Object::String));
    else if (obj.Is(Object::Map)) {
        b->ListItem(Bridge::MapBegin);
        Object::MapType::const_iterator I;
        for (I = obj.As(Object::Map).begin(); I != obj.As(Object::Map).end();
                I++)
            MapItem((*I).first, (*I).second);
        b->MapEnd();
    } else if (obj.Is(Object::List)) {
        b->ListItem(Bridge::ListBegin);
        Object::ListType::const_iterator I;
        for (I = obj.As(Object::List).begin(); I != obj.As(Object::List).end();
                I++)
            ListItem(*I);
        b->ListEnd();
    }
}

void Encoder::MapItem(const string& name,  const Object& obj)
{
    if (obj.Is(Object::Int)) b->MapItem(name, obj.As(Object::Int));
    else if (obj.Is(Object::Float)) b->MapItem(name,
            obj.As(Object::Float));
    else if (obj.Is(Object::String)) b->MapItem(name,
            obj.As(Object::String));
    else if (obj.Is(Object::Map)) {
        b->MapItem(name, Bridge::MapBegin);
        Object::MapType::const_iterator I;
        for (I = obj.As(Object::Map).begin(); I != obj.As(Object::Map).end();
                I++)
            MapItem((*I).first, (*I).second);
        b->MapEnd();
    } else if (obj.Is(Object::List)) {
        b->MapItem(name, Bridge::ListBegin);
        Object::ListType::const_iterator I;
        for (I = obj.As(Object::List).begin(); I != obj.As(Object::List).end();
                I++)
            ListItem(*I);
        b->ListEnd();
    }
}

void Encoder::StreamMessage(const Object& obj)
{
    if (obj.Is(Object::Map)) {
        b->StreamMessage(Bridge::MapBegin);
        Object::MapType::const_iterator I;
        for (I = obj.As(Object::Map).begin();
             I != obj.As(Object::Map).end(); I++)
            MapItem((*I).first, (*I).second);
        b->MapEnd();
    }
}



} } // namespace Atlas::Message
