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
    switch (obj.GetType()) {
        case Object::TYPE_INT: b->ListItem(obj.AsInt()); break;
        case Object::TYPE_FLOAT: b->ListItem(obj.AsFloat()); break;
        case Object::TYPE_STRING: b->ListItem(obj.AsString()); break;
        case Object::TYPE_MAP: {
            b->ListItem(Bridge::MapBegin);
            Object::MapType::const_iterator I;
            for (I = obj.AsMap().begin(); I != obj.AsMap().end();
                    I++)
                MapItem((*I).first, (*I).second);
            b->MapEnd();
            }
            break;
        case Object::TYPE_LIST: {
            b->ListItem(Bridge::ListBegin);
            Object::ListType::const_iterator I;
            for (I = obj.AsList().begin(); I != obj.AsList().end();
                    I++)
                ListItem(*I);
            b->ListEnd();
            }
            break;
        default: break;
    }
}

void Encoder::MapItem(const string& name, const Object& obj)
{
    switch (obj.GetType()) {
        case Object::TYPE_INT: b->MapItem(name, obj.AsInt()); break;
        case Object::TYPE_FLOAT: b->MapItem(name, obj.AsFloat()); break;
        case Object::TYPE_STRING: b->MapItem(name, obj.AsString()); break;
        case Object::TYPE_MAP: {
            b->MapItem(name, Bridge::MapBegin);
            Object::MapType::const_iterator I;
            for (I = obj.AsMap().begin(); I != obj.AsMap().end();
                    I++)
                MapItem((*I).first, (*I).second);
            b->MapEnd();
            }
            break;
        case Object::TYPE_LIST: {
            b->MapItem(name, Bridge::ListBegin);
            Object::ListType::const_iterator I;
            for (I = obj.AsList().begin(); I != obj.AsList().end();
                    I++)
                ListItem(*I);
            b->ListEnd();
            }
            break;
        default:
            break;
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
