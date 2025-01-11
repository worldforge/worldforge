// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

// $Id$

#include <Atlas/Message/MEncoder.h>

#include <Atlas/Message/Element.h>


namespace Atlas::Message {

Encoder::Encoder(Bridge& b)
		: EncoderBase(b) {
}


void Encoder::listElementItem(const Element& obj) {
	switch (obj.getType()) {
		case Element::TYPE_INT:
			m_b.listIntItem(obj.Int());
			break;
		case Element::TYPE_FLOAT:
			m_b.listFloatItem(obj.Float());
			break;
		case Element::TYPE_STRING:
			m_b.listStringItem(obj.String());
			break;
		case Element::TYPE_MAP: {
			m_b.listMapItem();
			for (auto& entry: obj.Map()) {
				mapElementItem(entry.first, entry.second);
			}
			m_b.mapEnd();
		}
			break;
		case Element::TYPE_LIST: {
			m_b.listListItem();
			for (auto& element: obj.List()) {
				listElementItem(element);
			}
			m_b.listEnd();
		}
			break;
		case Element::TYPE_NONE:
			m_b.listNoneItem();
			break;
		default:
			break;
	}
}

void Encoder::listElementMapItem(const MapType& obj) {
	m_b.listMapItem();
	for (auto& entry: obj) {
		mapElementItem(entry.first, entry.second);
	}
	m_b.mapEnd();
}

void Encoder::listElementListItem(const ListType& obj) {
	m_b.listListItem();
	for (auto& element: obj) {
		listElementItem(element);
	}
	m_b.listEnd();
}

void Encoder::mapElementItem(const std::string& name, const Element& obj) {
	switch (obj.getType()) {
		case Element::TYPE_INT:
			m_b.mapIntItem(name, obj.Int());
			break;
		case Element::TYPE_FLOAT:
			m_b.mapFloatItem(name, obj.Float());
			break;
		case Element::TYPE_STRING:
			m_b.mapStringItem(name, obj.String());
			break;
		case Element::TYPE_MAP: {
			m_b.mapMapItem(name);
			for (auto& entry: obj.Map()) {
				mapElementItem(entry.first, entry.second);
			}
			m_b.mapEnd();
		}
			break;
		case Element::TYPE_LIST: {
			m_b.mapListItem(name);
			for (auto& element: obj.List()) {
				listElementItem(element);
			}
			m_b.listEnd();
		}
			break;
		case Element::TYPE_NONE:
			m_b.mapNoneItem(name);
			break;
		default:
			break;
	}
}

void Encoder::mapElementMapItem(const std::string& name, const MapType& obj) {
	m_b.mapMapItem(name);
	for (auto& entry: obj) {
		mapElementItem(entry.first, entry.second);
	}
	m_b.mapEnd();
}

void Encoder::mapElementListItem(const std::string& name, const ListType& obj) {
	m_b.mapListItem(name);
	for (auto& element: obj) {
		listElementItem(element);
	}
	m_b.listEnd();
}

void Encoder::streamMessageElement(const MapType& obj) {
	m_b.streamMessage();
	for (auto& entry: obj) {
		mapElementItem(entry.first, entry.second);
	}
	m_b.mapEnd();
}

}
// namespace Atlas::Message
