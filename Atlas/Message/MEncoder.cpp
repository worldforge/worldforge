// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include "MEncoder.h"
#include "Object.h"

namespace Atlas { namespace Message {

Encoder::Encoder(Atlas::Bridge* b)
    : EncoderBase(b)
{
}

void Encoder::listElementItem(const Element& obj)
{
    switch (obj.getType()) {
        case Element::TYPE_INT: m_bridge->listIntItem(obj.asInt()); break;
        case Element::TYPE_FLOAT: m_bridge->listFloatItem(obj.asFloat()); break;
        case Element::TYPE_STRING: m_bridge->listStringItem(obj.asString()); break;
        case Element::TYPE_MAP: {
            m_bridge->listMapItem();
            Element::MapType::const_iterator I;
            for (I = obj.asMap().begin(); I != obj.asMap().end(); I++) {
                mapElementItem((*I).first, (*I).second);
	    }
            m_bridge->mapEnd();
            }
            break;
        case Element::TYPE_LIST: {
            m_bridge->listListItem();
            Element::ListType::const_iterator I;
            for (I = obj.asList().begin(); I != obj.asList().end(); I++) {
                listElementItem(*I);
	    }
            m_bridge->listEnd();
            }
            break;
        default: break;
    }
}

void Encoder::listElementMapItem(const Element::MapType& obj)
{
    m_bridge->listMapItem();
    Element::MapType::const_iterator I;
    for (I = obj.begin(); I != obj.end(); I++) {
        mapElementItem(I->first, I->second);
    }
    m_bridge->mapEnd();
}

void Encoder::listElementListItem(const Element::ListType& obj)
{
    m_bridge->listListItem();
    Element::ListType::const_iterator I;
    for (I = obj.begin(); I != obj.end(); I++) {
        listElementItem(*I);
    }
    m_bridge->listEnd();    
}

void Encoder::mapElementItem(const std::string& name, const Element& obj)
{
    switch (obj.getType()) {
        case Element::TYPE_INT: m_bridge->mapIntItem(name, obj.asInt()); break;
        case Element::TYPE_FLOAT: m_bridge->mapFloatItem(name, obj.asFloat()); break;
        case Element::TYPE_STRING: m_bridge->mapStringItem(name, obj.asString()); break;
        case Element::TYPE_MAP: {
            m_bridge->mapMapItem(name);
            Element::MapType::const_iterator I;
            for (I = obj.asMap().begin(); I != obj.asMap().end(); I++) {
                mapElementItem((*I).first, (*I).second);
	    }
            m_bridge->mapEnd();
            }
            break;
        case Element::TYPE_LIST: {
            m_bridge->mapListItem(name);
            Element::ListType::const_iterator I;
            for (I = obj.asList().begin(); I != obj.asList().end(); I++) {
                listElementItem(*I);
	    }
            m_bridge->listEnd();
            }
            break;
        default:
            break;
    }                          
}

void Encoder::mapElementMapItem(const std::string& name, const Element::MapType& obj)
{
    m_bridge->mapMapItem(name);
    Element::MapType::const_iterator I;
    for (I = obj.begin(); I != obj.end(); I++) {
        mapElementItem((*I).first, (*I).second);
    }
    m_bridge->mapEnd();
}

void Encoder::mapElementListItem(const std::string& name, const Element::ListType& obj)
{
    m_bridge->mapListItem(name);
    Element::ListType::const_iterator I;
    for (I = obj.begin(); I != obj.end(); I++) {
        listElementItem(*I);
    }
    m_bridge->listEnd();
}

void Encoder::streamMessageElement(const Element::MapType& obj)
{
    m_bridge->streamMessage();
    Element::MapType::const_iterator I;
    for (I = obj.begin(); I != obj.end(); I++) {
        mapElementItem((*I).first, (*I).second);
    }
    m_bridge->mapEnd();
}

} } // namespace Atlas::Message
