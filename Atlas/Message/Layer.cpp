// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day, Stefanus Du Toit

#include "Layer.h"

using namespace std;
using namespace Atlas;
using namespace Atlas::Message;

inline void recurseListObject(const Object& obj, Bridge* b);

inline void recurseMapObject(const Object& obj, Bridge* b, const string&
        name)
{
    if (obj.isMap()) {
        b->MapItem(name, Bridge::MapBegin);
        Object names = obj.keys();
        Object values = obj.vals();
        for (size_t i = 0; i < names.length(); i++) {
            string itemName;
            Object value;
            names.get(i, itemName);
            values.get(i, value);
            recurseMapObject(value, b, itemName);
        }
        b->MapEnd();
    }
    if (obj.isList()) {
        b->MapItem(name, Bridge::ListBegin);
        for (size_t i = 0; i < obj.length(); i++) {
            Object value;
            obj.get(i, value);
            recurseListObject(value, b);
        }
        b->ListEnd();
    }
    if (obj.isFloat()) {
        b->MapItem(name, obj.asFloat());
    }
    if (obj.isInt()) {
        b->MapItem(name, obj.asInt());
    }
    if (obj.isString()) {
        b->MapItem(name, obj.asString());
    }
}

inline void recurseListObject(const Object& obj, Bridge* b)
{
    if (obj.isMap()) {
        b->ListItem(Bridge::MapBegin);
        Object names = obj.keys();
        Object values = obj.vals();
        for (size_t i = 0; i < names.length(); i++) {
            string itemName;
            Object value;
            names.get(i, itemName);
            values.get(i, value);
            recurseMapObject(value, b, itemName);
        }
        b->MapEnd();
    }
    if (obj.isList()) {
        b->ListItem(Bridge::ListBegin);
        for (size_t i = 0; i < obj.length(); i++) {
            Object value;
            obj.get(i, value);
            recurseListObject(value, b);
        }
        b->ListEnd();
    }
    if (obj.isFloat()) {
        b->ListItem(obj.asFloat());
    }
    if (obj.isInt()) {
        b->ListItem(obj.asInt());
    }
    if (obj.isString()) {
        b->ListItem(obj.asString());
    }
}

