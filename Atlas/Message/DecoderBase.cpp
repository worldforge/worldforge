// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "../Atlas.h"

#include "DecoderBase.h"

#include <iostream>

namespace Atlas { namespace Message {

DecoderBase::DecoderBase()
{
}

void DecoderBase::streamBegin()
{
    ATLAS_DEBUG(std::cout << "DecoderBase::streamBegin" << std::endl)
    m_state.push(STATE_STREAM);
}

void DecoderBase::streamMessage(const Map&)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::streamMessage" << std::endl)
    Element::MapType m;
    m_maps.push(m);
    m_state.push(STATE_MAP);
}

void DecoderBase::streamEnd()
{
    ATLAS_DEBUG(std::cout << "DecoderBase::streamEnd" << std::endl)
    assert(!m_state.empty());
    m_state.pop();
}

void DecoderBase::mapItem(const std::string& name, const Map&)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::mapItem Map" << std::endl)
    Element::MapType m;
    m_names.push(name);
    m_maps.push(m);
    m_state.push(STATE_MAP);
}

void DecoderBase::mapItem(const std::string& name, const List&)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::mapItem List" << std::endl)
    Element::ListType l;
    m_names.push(name);
    m_lists.push(l);
    m_state.push(STATE_LIST);
}

void DecoderBase::mapItem(const std::string& name, long i)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::mapItem" << std::endl)
    assert(!m_maps.empty());	    
    m_maps.top()[name] = i;
}

void DecoderBase::mapItem(const std::string& name, double d)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::mapItem" << std::endl)
    assert(!m_maps.empty());	   
    m_maps.top()[name] = d;
}

void DecoderBase::mapItem(const std::string& name, const std::string& s)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::mapItem" << std::endl)
    assert(!m_maps.empty());
    m_maps.top()[name] = s;
}

void DecoderBase::mapEnd()
{
    ATLAS_DEBUG(std::cout << "DecoderBase::mapEnd" << std::endl)
    assert(!m_maps.empty());
    assert(!m_state.empty());
    Element::MapType map = m_maps.top();
    m_maps.pop();
    m_state.pop();
    switch (m_state.top()) {
        case STATE_MAP:
	    assert(!m_maps.empty());
            assert(!m_names.empty());
            m_maps.top()[m_names.top()] = map;
            m_names.pop();
            break;
        case STATE_LIST:
	    assert(!m_lists.empty());
            m_lists.top().insert(m_lists.top().end(), map);
            break;
        case STATE_STREAM:
            objectArrived(map);
            break;
    }
}

void DecoderBase::listItem(const Map&)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::listItem Map" << std::endl)
    Element::MapType map;
    m_maps.push(map);
    m_state.push(STATE_MAP);
}

void DecoderBase::listItem(const List&)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::listItem List" << std::endl)
    Element::ListType list;
    m_lists.push(list);
    m_state.push(STATE_LIST);
}

void DecoderBase::listItem(long i)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::listItem" << std::endl)
    assert(!m_lists.empty());	    
    m_lists.top().push_back(i);
}

void DecoderBase::listItem(double d)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::listItem" << std::endl)
    m_lists.top().push_back(d);
}

void DecoderBase::listItem(const std::string& s)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::listItem" << std::endl)
    assert(!m_lists.empty());	    
    m_lists.top().push_back(s);
}

void DecoderBase::listEnd()
{
    ATLAS_DEBUG(std::cout << "DecoderBase::listEnd" << std::endl)
    assert(!m_lists.empty());
    assert(!m_state.empty());
    Element::ListType list = m_lists.top();
    m_lists.pop();
    m_state.pop();
    switch (m_state.top()) {
        case STATE_MAP:
            assert(!m_maps.empty());
            assert(!m_names.empty());
            m_maps.top()[m_names.top()] = list;
            m_names.pop();
            break;
        case STATE_LIST:
	    assert(!m_lists.empty());
            m_lists.top().push_back(list);
            break;
        case STATE_STREAM:
            std::cerr << "DecoderBase::listEnd: Error" << std::endl;
            // XXX - report error?
            break;
    }
}

} } // namespace Atlas::Message
