// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include <string>
#include "Encoder.h"

using namespace std;

namespace Atlas { namespace Message {

Encoder::Encoder(Atlas::Bridge* bridge)
    : bridge(bridge)
{
}

void Encoder::ListItem(const Object& obj)
{
    if (obj.Is(Object::Int)) bridge->ListItem(obj.As(Object::Int));
    else if (obj.Is(Object::Float)) bridge->ListItem(obj.As(Object::Float));
    else if (obj.Is(Object::String)) bridge->ListItem(obj.As(Object::String));
    else if (obj.Is(Object::Map)) {
        bridge->ListItem(Bridge::MapBegin);
        Object::MapType::const_iterator I;
        for (I = obj.As(Object::Map).begin(); I != obj.As(Object::Map).end();
                I++)
            MapItem((*I).first, (*I).second);
        bridge->MapEnd();
    } else if (obj.Is(Object::List)) {
        bridge->ListItem(Bridge::ListBegin);
        Object::ListType::const_iterator I;
        for (I = obj.As(Object::List).begin(); I != obj.As(Object::List).end();
                I++)
            ListItem(*I);
        bridge->ListEnd();
    }
}

void Encoder::MapItem(const string& name,  const Object& obj)
{
    if (obj.Is(Object::Int)) bridge->MapItem(name, obj.As(Object::Int));
    else if (obj.Is(Object::Float)) bridge->MapItem(name,
            obj.As(Object::Float));
    else if (obj.Is(Object::String)) bridge->MapItem(name,
            obj.As(Object::String));
    else if (obj.Is(Object::Map)) {
        bridge->MapItem(name, Bridge::MapBegin);
        Object::MapType::const_iterator I;
        for (I = obj.As(Object::Map).begin(); I != obj.As(Object::Map).end();
                I++)
            MapItem((*I).first, (*I).second);
        bridge->MapEnd();
    } else if (obj.Is(Object::List)) {
        bridge->MapItem(name, Bridge::ListBegin);
        Object::ListType::const_iterator I;
        for (I = obj.As(Object::List).begin(); I != obj.As(Object::List).end();
                I++)
            ListItem(*I);
        bridge->ListEnd();
    }
}

void Encoder::StreamMessage(const Object& obj)
{
    if (obj.Is(Object::Map)) {
        bridge->StreamMessage(Bridge::MapBegin);
        Object::MapType::const_iterator I;
        for (I = obj.As(Object::Map).begin();
             I != obj.As(Object::Map).end(); I++)
            MapItem((*I).first, (*I).second);
        bridge->MapEnd();
    }
}



} } // namespace Atlas::Message
