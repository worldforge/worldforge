/*
 Copyright (C) 2008  Alexey Torkhov <atorkhov@gmail.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "DetachedEntity.h"


namespace Ember::OgreView::Authoring {
DetachedEntity::DetachedEntity(std::string id, Eris::TypeInfo* ty) :
		Eris::Entity(id, ty, {
				.fetchEntity=[](const std::string&) { return nullptr; },
				.getEntity=[](const std::string&) { return nullptr; },
				.taskUpdated=[](Eris::Task&) {}
		}) {
}

DetachedEntity::~DetachedEntity() {
	//Since we might have faked that we have a parent we need to reset that to prevent base class from trying to remove us from it.
	m_location = nullptr;
}


void DetachedEntity::setFromMessage(const Atlas::Message::MapType& attrs) {
	beginUpdate();

	Atlas::Message::MapType::const_iterator A;

	for (A = attrs.begin(); A != attrs.end(); ++A) {
		if (A->first == "loc" || A->first == "id" || A->first == "contains")
			continue;

		// see if the value in the sight matches the existing value
		auto I = m_properties.find(A->first);
		if ((I != m_properties.end()) && (I->second == A->second))
			continue;

		setProperty(A->first, A->second);
	}

	endUpdate();
}

void DetachedEntity::setLocationEntity(Eris::Entity* location) {
	//We fake that it's a child, while it's really not in reality.
	m_location = location;
}


}


