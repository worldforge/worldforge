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
    if (obj.IsInt()) b->ListItem(obj.AsInt());
    else if (obj.IsFloat()) b->ListItem(obj.AsFloat());
    else if (obj.IsString()) b->ListItem(obj.AsString());
    else if (obj.IsMap()) {
        b->ListItem(Bridge::MapBegin);
        Object::MapType::const_iterator I;
        for (I = obj.AsMap().begin(); I != obj.AsMap().end();
                I++)
            MapItem((*I).first, (*I).second);
        b->MapEnd();
    } else if (obj.IsList()) {
        b->ListItem(Bridge::ListBegin);
        Object::ListType::const_iterator I;
        for (I = obj.AsList().begin(); I != obj.AsList().end();
                I++)
            ListItem(*I);
        b->ListEnd();
    }
}

void Encoder::MapItem(const string& name, const Object& obj)
{
    if (obj.IsInt()) b->MapItem(name, obj.AsInt());
    else if (obj.IsFloat()) b->MapItem(name,
            obj.AsFloat());
    else if (obj.IsString()) b->MapItem(name,
            obj.AsString());
    else if (obj.IsMap()) {
        b->MapItem(name, Bridge::MapBegin);
        Object::MapType::const_iterator I;
        for (I = obj.AsMap().begin(); I != obj.AsMap().end();
                I++)
            MapItem((*I).first, (*I).second);
        b->MapEnd();
    } else if (obj.IsList()) {
        b->MapItem(name, Bridge::ListBegin);
        Object::ListType::const_iterator I;
        for (I = obj.AsList().begin(); I != obj.AsList().end();
                I++)
            ListItem(*I);
        b->ListEnd();
    }
}

void Encoder::StreamMessage(const Object& obj)
{
    if (obj.IsMap()) {
        b->StreamMessage(Bridge::MapBegin);
        Object::MapType::const_iterator I;
        for (I = obj.AsMap().begin();
             I != obj.AsMap().end(); I++)
            MapItem((*I).first, (*I).second);
        b->MapEnd();
    }
}



} } // namespace Atlas::Message
