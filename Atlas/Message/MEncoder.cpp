// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include <string>
#include "Encoder.h"

namespace Atlas { namespace Message {

void * Object::freeList = NULL;

Encoder::Encoder(Atlas::Bridge* b)
    : EncoderBase(b)
{
}

void Encoder::listItem(const Object& obj)
{
    switch (obj.getType()) {
        case Object::TYPE_INT: b->listItem(obj.asInt()); break;
        case Object::TYPE_FLOAT: b->listItem(obj.asFloat()); break;
        case Object::TYPE_STRING: b->listItem(obj.asString()); break;
        case Object::TYPE_MAP: {
            b->listItem(Atlas::Bridge::mapBegin);
            Object::MapType::const_iterator I;
            for (I = obj.asMap().begin(); I != obj.asMap().end();
                    I++)
                mapItem((*I).first, (*I).second);
            b->mapEnd();
            }
            break;
        case Object::TYPE_LIST: {
            b->listItem(Atlas::Bridge::listBegin);
            Object::ListType::const_iterator I;
            for (I = obj.asList().begin(); I != obj.asList().end();
                    I++)
                listItem(*I);
            b->listEnd();
            }
            break;
        default: break;
    }
}

void Encoder::mapItem(const std::string& name, const Object& obj)
{
    switch (obj.getType()) {
        case Object::TYPE_INT: b->mapItem(name, obj.asInt()); break;
        case Object::TYPE_FLOAT: b->mapItem(name, obj.asFloat()); break;
        case Object::TYPE_STRING: b->mapItem(name, obj.asString()); break;
        case Object::TYPE_MAP: {
            b->mapItem(name, Atlas::Bridge::mapBegin);
            Object::MapType::const_iterator I;
            for (I = obj.asMap().begin(); I != obj.asMap().end();
                    I++)
                mapItem((*I).first, (*I).second);
            b->mapEnd();
            }
            break;
        case Object::TYPE_LIST: {
            b->mapItem(name, Atlas::Bridge::listBegin);
            Object::ListType::const_iterator I;
            for (I = obj.asList().begin(); I != obj.asList().end();
                    I++)
                listItem(*I);
            b->listEnd();
            }
            break;
        default:
            break;
    }                          
}

void Encoder::streamMessage(const Object& obj)
{
    if (obj.isMap()) {
        b->streamMessage(Atlas::Bridge::mapBegin);
        Object::MapType::const_iterator I;
        for (I = obj.asMap().begin();
             I != obj.asMap().end(); I++)
            mapItem((*I).first, (*I).second);
        b->mapEnd();
    }
}



} } // namespace Atlas::Message
