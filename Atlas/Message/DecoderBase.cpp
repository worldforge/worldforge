// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

// $Id$

#include <Atlas/Message/DecoderBase.h>

#include <Atlas/Message/Element.h>

#include <Atlas/Debug.h>

#include <iostream>

#include <cassert>

static const bool debug_flag = false;

namespace Atlas { namespace Message {

DecoderBase::DecoderBase() : m_state(), m_maps(), m_lists(), m_names()
{
}

DecoderBase::~DecoderBase()
{
}

void DecoderBase::streamBegin()
{
    ATLAS_DEBUG(std::cout << "DecoderBase::streamBegin" << std::endl)
    m_state.push(STATE_STREAM);
}

void DecoderBase::streamMessage()
{
    ATLAS_DEBUG(std::cout << "DecoderBase::streamMessage" << std::endl)
    MapType m;
    m_maps.push(m);
    m_state.push(STATE_MAP);
}

void DecoderBase::streamEnd()
{
    ATLAS_DEBUG(std::cout << "DecoderBase::streamEnd" << std::endl)
    assert(!m_state.empty());
    m_state.pop();
}

void DecoderBase::mapMapItem(const std::string& name)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::mapMapItem Map" << std::endl)
    MapType m;
    m_names.push(name);
    m_maps.push(m);
    m_state.push(STATE_MAP);
}

void DecoderBase::mapListItem(const std::string& name)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::mapListItem List" << std::endl)
    ListType l;
    m_names.push(name);
    m_lists.push(l);
    m_state.push(STATE_LIST);
}

void DecoderBase::mapIntItem(const std::string& name, long i)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::mapIntItem" << std::endl)
    assert(!m_maps.empty());        
    m_maps.top()[name] = i;
}

void DecoderBase::mapFloatItem(const std::string& name, double d)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::mapFloatItem" << std::endl)
    assert(!m_maps.empty());       
    m_maps.top()[name] = d;
}

void DecoderBase::mapStringItem(const std::string& name, const std::string& s)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::mapStringItem" << std::endl)
    assert(!m_maps.empty());
    m_maps.top()[name] = s;
}

void DecoderBase::mapEnd()
{
    ATLAS_DEBUG(std::cout << "DecoderBase::mapEnd" << std::endl)
    assert(!m_maps.empty());
    assert(!m_state.empty());
    m_state.pop();
    switch (m_state.top()) {
        case STATE_MAP:
            {
                MapType map = m_maps.top();
                m_maps.pop();
                assert(!m_maps.empty());
                assert(!m_names.empty());
                m_maps.top()[m_names.top()] = map;
                m_names.pop();
            }
            break;
        case STATE_LIST:
            {
                MapType map = m_maps.top();
                m_maps.pop();
                assert(!m_lists.empty());
                m_lists.top().insert(m_lists.top().end(), map);
            }
            break;
        case STATE_STREAM:
            {
                MapType & map = m_maps.top();
                messageArrived(map);
                m_maps.pop();
            }
            break;
        default:
            {
                // MapType map = m_maps.top();
                m_maps.pop();
            }
            break;
    }
}

void DecoderBase::listMapItem()
{
    ATLAS_DEBUG(std::cout << "DecoderBase::listMapItem" << std::endl)
    MapType map;
    m_maps.push(map);
    m_state.push(STATE_MAP);
}

void DecoderBase::listListItem()
{
    ATLAS_DEBUG(std::cout << "DecoderBase::listListItem" << std::endl)
    ListType list;
    m_lists.push(list);
    m_state.push(STATE_LIST);
}

void DecoderBase::listIntItem(long i)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::listIntItem" << std::endl)
    assert(!m_lists.empty());       
    m_lists.top().push_back(i);
}

void DecoderBase::listFloatItem(double d)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::listFloatItem" << std::endl)
    m_lists.top().push_back(d);
}

void DecoderBase::listStringItem(const std::string& s)
{
    ATLAS_DEBUG(std::cout << "DecoderBase::listStringItem" << std::endl)
    assert(!m_lists.empty());       
    m_lists.top().push_back(s);
}

void DecoderBase::listEnd()
{
    ATLAS_DEBUG(std::cout << "DecoderBase::listEnd" << std::endl)
    assert(!m_lists.empty());
    assert(!m_state.empty());
    ListType list = m_lists.top();
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
