// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#pragma once

#ifndef COMMON_PROPERTY_IMPL_H
#define COMMON_PROPERTY_IMPL_H

#include "Property.h"

#include <Atlas/Objects/RootEntity.h>


template<typename EntityT>
PropertyCore<EntityT>::PropertyCore(std::uint32_t flags) : m_flags(flags) {
}

template<typename EntityT>
void PropertyCore<EntityT>::install(EntityT&, const std::string& name) {
}

template<typename EntityT>
void PropertyCore<EntityT>::install(TypeNode<EntityT>&, const std::string& name) {
}

template<typename EntityT>
void PropertyCore<EntityT>::remove(EntityT&, const std::string& name) {
}

template<typename EntityT>
void PropertyCore<EntityT>::apply(EntityT&) {
}

template<typename EntityT>
void PropertyCore<EntityT>::add(const std::string& s,
								Atlas::Message::MapType& ent) const {
	get(ent[s]);
}

template<typename EntityT>
void PropertyCore<EntityT>::add(const std::string& s,
								const Atlas::Objects::Entity::RootEntity& ent) const {
	Atlas::Message::Element val;
	get(val);
	ent->setAttr(s, val);
}

template<typename EntityT>
HandlerResult PropertyCore<EntityT>::operation(EntityT&,
											   const Operation&,
											   OpVector& res) {
	return OPERATION_IGNORED;
}

template<typename EntityT>
bool PropertyCore<EntityT>::operator==(const PropertyCore& rhs) const {
	Atlas::Message::Element thisElement;
	get(thisElement);
	Atlas::Message::Element thatElement;
	rhs.get(thatElement);
	return thisElement == thatElement;
}

template<typename EntityT>
bool PropertyCore<EntityT>::operator!=(const PropertyCore& rhs) const {
	return !operator==(rhs);
}

/// \brief Constructor for immutable Properties
template<typename T, typename EntityT>
Property<T, EntityT>::Property(unsigned int flags) :
		PropertyCore<EntityT>(flags) {
}

template<typename T, typename EntityT>
int Property<T, EntityT>::get(Atlas::Message::Element& e) const {
	e = m_data;
	return 0;
}

// The following two are obsolete.
template<typename T, typename EntityT>
void Property<T, EntityT>::add(const std::string& s,
							   Atlas::Message::MapType& ent) const {
	get(ent[s]);
}

template<typename T, typename EntityT>
void Property<T, EntityT>::add(const std::string& s,
							   const Atlas::Objects::Entity::RootEntity& ent) const {
	Atlas::Message::Element val;
	get(val);
	ent->setAttr(s, val);
}

template<typename T, typename EntityT>
Property<T, EntityT>* Property<T, EntityT>::copy() const {
	return new Property(*this);
}


template<typename EntityT>
SoftProperty<EntityT>::SoftProperty(Atlas::Message::Element data) :
		PropertyCore<EntityT>(0), m_data(std::move(data)) {
}

template<typename EntityT>
int SoftProperty<EntityT>::get(Atlas::Message::Element& val) const {
	val = m_data;
	return 0;
}

template<typename EntityT>
void SoftProperty<EntityT>::set(const Atlas::Message::Element& val) {
	m_data = val;
}

template<typename EntityT>
SoftProperty<EntityT>* SoftProperty<EntityT>::copy() const {
	return new SoftProperty(*this);
}

template<typename EntityT>
Atlas::Message::Element& SoftProperty<EntityT>::data() {
	return m_data;
}

template<typename EntityT>
const Atlas::Message::Element& SoftProperty<EntityT>::data() const {
	return m_data;
}


template<typename EntityT>
int BoolProperty<EntityT>::get(Atlas::Message::Element& ent) const {
	ent = this->m_flags.hasFlags(prop_flag_bool) ? 1 : 0;
	return 0;
}

template<typename EntityT>
void BoolProperty<EntityT>::set(const Atlas::Message::Element& ent) {
	if (ent.isInt()) {
		if (ent.Int() == 0) {
			this->m_flags.removeFlags(prop_flag_bool);
		} else {
			this->m_flags.addFlags(prop_flag_bool);
		}
	}
}

template<typename EntityT>
BoolProperty<EntityT>* BoolProperty<EntityT>::copy() const {
	return new BoolProperty(*this);
}

template<typename EntityT>
bool BoolProperty<EntityT>::isTrue() const {
	return this->m_flags.hasFlags(prop_flag_bool);
}




#endif // COMMON_PROPERTY_IMPL_H
