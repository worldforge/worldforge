// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "DecoderBase.h"

namespace Atlas { namespace Message {

DecoderBase::DecoderBase()
{
}

void DecoderBase::StreamBegin()
{
    state.push(STATE_STREAM);
}

void DecoderBase::StreamMessage(const Map&)
{
    Object map = Object::mkMap();
    objects.push(map);
    state.push(STATE_MAP);
}

void DecoderBase::StreamEnd()
{
    state.pop();
}

void DecoderBase::MapItem(const std::string& name, const Map&)
{
    Object map = Object::mkMap();
    names.push(name);
    objects.push(map);
    state.push(STATE_MAP);
}

void DecoderBase::MapItem(const std::string& name, const List&)
{
    Object list = Object::mkList();
    names.push(name);
    objects.push(list);
    state.push(STATE_LIST);
}
    
void DecoderBase::MapItem(const std::string& name, int i)
{
    objects.top().set(name, i);
}
    
void DecoderBase::MapItem(const std::string& name, double d)
{
    objects.top().set(name, d);
}
    
void DecoderBase::MapItem(const std::string& name, const std::string& s)
{
    objects.top().set(name, s);
}

void DecoderBase::MapEnd()
{
    Object map = objects.top();
    objects.pop();
    state.pop();
    switch (state.top()) {
        case STATE_MAP:
            objects.top().set(names.top(), map);
            names.pop();
            break;
        case STATE_LIST:
            objects.top().append(map);
            break;
        case STATE_STREAM:
            ObjectArrived(map);
            break;
    }
}
  
void DecoderBase::ListItem(const Map&)
{
    Object map = Object::mkMap();
    objects.push(map);
    state.push(STATE_MAP);
}
    
void DecoderBase::ListItem(const List&)
{
    Object list = Object::mkList();
    objects.push(list);
    state.push(STATE_LIST);
}
    
void DecoderBase::ListItem(int i)
{
    objects.top().append(i);
}

void DecoderBase::ListItem(double d)
{
    objects.top().append(d);
}
    
void DecoderBase::ListItem(const std::string& s)
{
    objects.top().append(s);
}
    
void DecoderBase::ListEnd()
{
    Object list = objects.top();
    objects.pop();
    state.pop();
    switch (state.top()) {
        case STATE_MAP:
            objects.top().set(names.top(), list);
            names.pop();
            break;
        case STATE_LIST:
            objects.top().append(list);
            break;
        case STATE_STREAM:
            // FIXME - report error?
            break;
    }
}
    
} } // namespace Atlas::Message
