// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "DecoderBase.h"

//#define DEBUG(a) a;
#define DEBUG(a) ;

namespace Atlas { namespace Message {

DecoderBase::DecoderBase()
{
}

void DecoderBase::streamBegin()
{
    DEBUG(cout << "DecoderBase::streamBegin" << endl)
    m_state.push(STATE_STREAM);
}

void DecoderBase::streamMessage(const Map&)
{
    DEBUG(cout << "DecoderBase::streamMessage" << endl)
    Object::MapType m;
    m_maps.push(m);
    m_state.push(STATE_MAP);
}

void DecoderBase::streamEnd()
{
    DEBUG(cout << "DecoderBase::streamEnd" << endl)
    m_state.pop();
}

void DecoderBase::mapItem(const std::string& name, const Map&)
{
    DEBUG(cout << "DecoderBase::mapItem Map" << endl)
    Object::MapType m;
    m_names.push(name);
    m_maps.push(m);
    m_state.push(STATE_MAP);
}

void DecoderBase::mapItem(const std::string& name, const List&)
{
    DEBUG(cout << "DecoderBase::mapItem List" << endl)
    Object::ListType l;
    m_names.push(name);
    m_lists.push(l);
    m_state.push(STATE_LIST);
}

void DecoderBase::mapItem(const std::string& name, long i)
{
    DEBUG(cout << "DecoderBase::mapItem" << endl)
    m_maps.top()[name] = i;
}

void DecoderBase::mapItem(const std::string& name, double d)
{
    DEBUG(cout << "DecoderBase::mapItem" << endl)
    m_maps.top()[name] = d;
}

void DecoderBase::mapItem(const std::string& name, const std::string& s)
{
    DEBUG(cout << "DecoderBase::mapItem" << endl)
    m_maps.top()[name] = s;
}

void DecoderBase::mapEnd()
{
    DEBUG(cout << "DecoderBase::mapEnd" << endl)
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
    DEBUG(cout << "DecoderBase::listItem Map" << endl)
    Object::MapType map;
    m_maps.push(map);
    m_state.push(STATE_MAP);
}

void DecoderBase::listItem(const List&)
{
    DEBUG(cout << "DecoderBase::listItem List" << endl)
    Object::ListType list;
    m_lists.push(list);
    m_state.push(STATE_LIST);
}

void DecoderBase::listItem(long i)
{
    DEBUG(cout << "DecoderBase::listItem" << endl)
    m_lists.top().push_back(i);
}

void DecoderBase::listItem(double d)
{
    DEBUG(cout << "DecoderBase::listItem" << endl)
    m_lists.top().push_back(d);
}

void DecoderBase::listItem(const std::string& s)
{
    DEBUG(cout << "DecoderBase::listItem" << endl)
    m_lists.top().push_back(s);
}

void DecoderBase::listEnd()
{
    DEBUG(cout << "DecoderBase::listEnd" << endl)
    Object::ListType list = m_lists.top();
    m_lists.pop();
    m_state.pop();
//    cout << "DecoderBase::listEnd 2" << endl;
    switch (m_state.top()) {
        case STATE_MAP:
            if (m_names.size() > 0)
            {
                //cout << "DecoderBase::listEnd: m_names.top(): " << m_names.top() << endl;
                m_maps.top()[m_names.top()] = list;
                m_names.pop();
            }
            else
            {
                cout << "DecoderBase::listEnd: Error: Forgot to push something to m_names ?" << endl;
            }
            break;
        case STATE_LIST:
            DEBUG(cout << "DecoderBase::listEnd: STATE_LIST" << endl)
            m_lists.top().push_back(list);
            break;
        case STATE_STREAM:
            cout << "DecoderBase::listEnd: Error" << endl;
            // XXX - report error?
            break;
    }
//    cout << "DecoderBase::listEnd - end" << endl;
}

} } // namespace Atlas::Message
