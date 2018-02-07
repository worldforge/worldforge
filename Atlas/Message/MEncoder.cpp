// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

// $Id$

#include <Atlas/Message/MEncoder.h>

#include <Atlas/Message/Element.h>

namespace Atlas { namespace Message {

Encoder::Encoder(Atlas::Bridge & b)
    : EncoderBase(b)
{
}


void Encoder::listElementItem(const Element& obj)
{
    switch (obj.getType()) {
        case Element::TYPE_INT: m_b.listIntItem(obj.Int()); break;
        case Element::TYPE_FLOAT: m_b.listFloatItem(obj.Float()); break;
        case Element::TYPE_STRING: m_b.listStringItem(obj.String()); break;
        case Element::TYPE_MAP: {
            m_b.listMapItem();
            MapType::const_iterator I;
            for (I = obj.Map().begin(); I != obj.Map().end(); I++) {
                mapElementItem((*I).first, (*I).second);
            }
            m_b.mapEnd();
            }
            break;
        case Element::TYPE_LIST: {
            m_b.listListItem();
            ListType::const_iterator I;
            for (I = obj.List().begin(); I != obj.List().end(); I++) {
                listElementItem(*I);
            }
            m_b.listEnd();
            }
            break;
        default: break;
    }
}

void Encoder::listElementMapItem(const MapType& obj)
{
    m_b.listMapItem();
    MapType::const_iterator I;
    for (I = obj.begin(); I != obj.end(); I++) {
        mapElementItem(I->first, I->second);
    }
    m_b.mapEnd();
}

void Encoder::listElementListItem(const ListType& obj)
{
    m_b.listListItem();
    ListType::const_iterator I;
    for (I = obj.begin(); I != obj.end(); I++) {
        listElementItem(*I);
    }
    m_b.listEnd();    
}

void Encoder::mapElementItem(const std::string& name, const Element& obj)
{
    switch (obj.getType()) {
        case Element::TYPE_INT: m_b.mapIntItem(name, obj.Int()); break;
        case Element::TYPE_FLOAT: m_b.mapFloatItem(name, obj.Float()); break;
        case Element::TYPE_STRING: m_b.mapStringItem(name, obj.String()); break;
        case Element::TYPE_MAP: {
            m_b.mapMapItem(name);
            MapType::const_iterator I;
            for (I = obj.Map().begin(); I != obj.Map().end(); I++) {
                mapElementItem((*I).first, (*I).second);
            }
            m_b.mapEnd();
            }
            break;
        case Element::TYPE_LIST: {
            m_b.mapListItem(name);
            ListType::const_iterator I;
            for (I = obj.List().begin(); I != obj.List().end(); I++) {
                listElementItem(*I);
            }
            m_b.listEnd();
            }
            break;
        default:
            break;
    }                          
}

void Encoder::mapElementMapItem(const std::string& name, const MapType& obj)
{
    m_b.mapMapItem(name);
    MapType::const_iterator I;
    for (I = obj.begin(); I != obj.end(); I++) {
        mapElementItem((*I).first, (*I).second);
    }
    m_b.mapEnd();
}

void Encoder::mapElementListItem(const std::string& name, const ListType& obj)
{
    m_b.mapListItem(name);
    ListType::const_iterator I;
    for (I = obj.begin(); I != obj.end(); I++) {
        listElementItem(*I);
    }
    m_b.listEnd();
}

void Encoder::streamMessageElement(const MapType& obj)
{
    m_b.streamMessage();
    MapType::const_iterator I;
    for (I = obj.begin(); I != obj.end(); I++) {
        mapElementItem((*I).first, (*I).second);
    }
    m_b.mapEnd();
}

} } // namespace Atlas::Message
