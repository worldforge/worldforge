// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include <cstdio>
#include "../Stream/Codec.h"
#include "../Object/Object.h"

using namespace std;
using namespace Atlas::Stream;

inline const string charToHex(char c)
{
    char hex[3];
    snprintf(hex, 3, "%x", c);
    return hex;
}

inline char hexToChar(const string& hex)
{
    char c;
    sscanf(hex.c_str(), "%x", c);
    return c;
}

inline const string hexEncode(const string& prefix, const string& special,
        const string& postfix, const string& message)
{
    string newMessage;
    
    for (int i = 0; i < message.size(); i++) {
        if (find(special.begin(), special.end(), message[i]) != special.end()) {
            newMessage += prefix;
            newMessage += charToHex(message[i]);
            newMessage += postfix;
        } else newMessage += message[i];
    }

    return newMessage;
}

// FIXME - postfix is pretty idiotic since it isn't actually checked for :/

inline const string hexDecode(const string& prefix, const string& postfix,
        const string& message)
{
    string newMessage;
    
    for (int i = 0; i < message.size(); i++) {
        if (message[i] == prefix[0]) {
            string hex;
            hex += message[++i];
            hex += message[++i];
            newMessage += hexToChar(hex);
            i += postfix.size();
        } else newMessage += message[i];
    }

    return newMessage;
}

inline void recurseListObject(const Atlas::Object& obj, Bridge* b);

inline void recurseMapObject(const Atlas::Object& obj, Bridge* b, const string&
        name)
{
    if (obj.isMap()) {
        b->MapItem(name, Bridge::MapBegin);
        Atlas::Object names = obj.keys();
        Atlas::Object values = obj.vals();
        for (size_t i = 0; i < names.length(); i++) {
            string itemName;
            Atlas::Object value;
            names.get(i, itemName);
            values.get(i, value);
            recurseMapObject(value, b, itemName);
        }
        b->MapEnd();
    }
    if (obj.isList()) {
        b->MapItem(name, Bridge::ListBegin);
        for (size_t i = 0; i < obj.length(); i++) {
            Atlas::Object value;
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

inline void recurseListObject(const Atlas::Object& obj, Bridge* b)
{
    if (obj.isMap()) {
        b->ListItem(Bridge::MapBegin);
        Atlas::Object names = obj.keys();
        Atlas::Object values = obj.vals();
        for (size_t i = 0; i < names.length(); i++) {
            string itemName;
            Atlas::Object value;
            names.get(i, itemName);
            values.get(i, value);
            recurseMapObject(value, b, itemName);
        }
        b->MapEnd();
    }
    if (obj.isList()) {
        b->ListItem(Bridge::ListBegin);
        for (size_t i = 0; i < obj.length(); i++) {
            Atlas::Object value;
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
