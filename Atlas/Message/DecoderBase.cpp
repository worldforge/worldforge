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
    Object::MapType m;
    maps.push(m);
    state.push(STATE_MAP);
}

void DecoderBase::StreamEnd()
{
    state.pop();
}

void DecoderBase::MapItem(const std::string& name, const Map&)
{
    Object::MapType m;
    names.push(name);
    maps.push(m);
    state.push(STATE_MAP);
}

void DecoderBase::MapItem(const std::string& name, const List&)
{
    Object::ListType l;
    names.push(name);
    lists.push(l);
    state.push(STATE_LIST);
}
    
void DecoderBase::MapItem(const std::string& name, int i)
{
    maps.top()[name] = i;
}
    
void DecoderBase::MapItem(const std::string& name, double d)
{
    maps.top()[name] = d;
}
    
void DecoderBase::MapItem(const std::string& name, const std::string& s)
{
    maps.top()[name] = s;
}

void DecoderBase::MapEnd()
{
    Object::MapType map = maps.top();
    maps.pop();
    state.pop();
    switch (state.top()) {
        case STATE_MAP:
            maps.top()[names.top()] = map;
            names.pop();
            break;
        case STATE_LIST:
            lists.top().insert(lists.top().end(), map);
            break;
        case STATE_STREAM:
            ObjectArrived(map);
            break;
    }
}
  
void DecoderBase::ListItem(const Map&)
{
    Object::MapType map;
    maps.push(map);
    state.push(STATE_MAP);
}
    
void DecoderBase::ListItem(const List&)
{
    Object::ListType list;
    lists.push(list);
    state.push(STATE_LIST);
}
    
void DecoderBase::ListItem(int i)
{
    lists.top().push_back(i);
}

void DecoderBase::ListItem(double d)
{
    lists.top().push_back(d);
}
    
void DecoderBase::ListItem(const std::string& s)
{
    lists.top().push_back(s);
}
    
void DecoderBase::ListEnd()
{
    Object::ListType list = lists.top();
    lists.pop();
    state.pop();
    switch (state.top()) {
        case STATE_MAP:
            maps.top()[names.top()] = list;
            names.pop();
            break;
        case STATE_LIST:
            lists.top().push_back(list);
            break;
        case STATE_STREAM:
            // XXX - report error?
            break;
    }
}
    
} } // namespace Atlas::Message
