// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "DecoderBase.h"

namespace Atlas { namespace Message {

DecoderBase::DecoderBase()
{
}

void DecoderBase::streamBegin()
{
    m_state.push(STATE_STREAM);
}

void DecoderBase::streamMessage(const Map&)
{
    Object::MapType m;
    m_maps.push(m);
    m_state.push(STATE_MAP);
}

void DecoderBase::streamEnd()
{
    m_state.pop();
}

void DecoderBase::mapItem(const std::string& name, const Map&)
{
    Object::MapType m;
    m_names.push(name);
    m_maps.push(m);
    m_state.push(STATE_MAP);
}

void DecoderBase::mapItem(const std::string& name, const List&)
{
    Object::ListType l;
    m_names.push(name);
    m_lists.push(l);
    m_state.push(STATE_LIST);
}
    
void DecoderBase::mapItem(const std::string& name, long i)
{
    m_maps.top()[name] = i;
}
    
void DecoderBase::mapItem(const std::string& name, double d)
{
    m_maps.top()[name] = d;
}
    
void DecoderBase::mapItem(const std::string& name, const std::string& s)
{
    m_maps.top()[name] = s;
}

void DecoderBase::mapEnd()
{
    Object::MapType map = m_maps.top();
    m_maps.pop();
    m_state.pop();
    switch (m_state.top()) {
        case STATE_MAP:
            m_maps.top()[m_names.top()] = map;
            m_names.pop();
            break;
        case STATE_LIST:
            m_lists.top().insert(m_lists.top().end(), map);
            break;
        case STATE_STREAM:
            objectArrived(map);
            break;
    }
}
  
void DecoderBase::listItem(const Map&)
{
    Object::MapType map;
    m_maps.push(map);
    m_state.push(STATE_MAP);
}
    
void DecoderBase::listItem(const List&)
{
    Object::ListType list;
    m_lists.push(list);
    m_state.push(STATE_LIST);
}
    
void DecoderBase::listItem(long i)
{
    m_lists.top().push_back(i);
}

void DecoderBase::listItem(double d)
{
    m_lists.top().push_back(d);
}
    
void DecoderBase::listItem(const std::string& s)
{
    m_lists.top().push_back(s);
}
    
void DecoderBase::listEnd()
{
    Object::ListType list = m_lists.top();
    m_lists.pop();
    m_state.pop();
    switch (m_state.top()) {
        case STATE_MAP:
            m_maps.top()[m_names.top()] = list;
            m_names.pop();
            break;
        case STATE_LIST:
            m_lists.top().push_back(list);
            break;
        case STATE_STREAM:
            // XXX - report error?
            break;
    }
}
    
} } // namespace Atlas::Message
