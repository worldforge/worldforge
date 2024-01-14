/*
 Copyright (C) 2011 Erik Ogenvik

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

#include "EntityTalk.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/RootOperation.h>
#include <algorithm>

namespace Ember {

EntityTalk EntityTalk::parse(const Atlas::Objects::Root& talk) {
	EntityTalk entityTalk;

	Atlas::Message::Element sayAttrib;
	if (!talk->copyAttr("say", sayAttrib) && sayAttrib.isString()) {
		entityTalk.message = sayAttrib.String();
	}

	Atlas::Message::Element soundAttrib;
	if (!talk->copyAttr("sound", soundAttrib) && soundAttrib.isString()) {
		entityTalk.sound = soundAttrib.String();
	}

	Atlas::Message::Element responseAttrib;
	//some talk operations come with a predefined set of suitable responses, so we'll store those so that they can later on be queried by the GUI for example
	if (!talk->copyAttr("responses", responseAttrib) && responseAttrib.isList()) {
		const Atlas::Message::ListType& responseList = responseAttrib.List();
		auto I = responseList.begin();
		for (; I != responseList.end(); ++I) {
			if (I->isString()) {
				entityTalk.suggestedResponses.emplace_back(I->String());
			}
		}
	}

	Atlas::Message::Element addressAttrib;
	//some talk operations come with a predefined set of suitable responses, so we'll store those so that they can later on be queried by the GUI for example
	if (!talk->copyAttr("address", addressAttrib) && addressAttrib.isList()) {
		const Atlas::Message::ListType& addressList = addressAttrib.List();
		auto I = addressList.begin();
		for (; I != addressList.end(); ++I) {
			if (I->isString()) {
				entityTalk.addressedEntityIds.emplace_back(I->String());
			}
		}
	}

	return entityTalk;
}


bool EntityTalk::isAddressedToNone() const {
	return addressedEntityIds.empty();
}

bool EntityTalk::isAddressedToEntity(const std::string& entityId) const {
	return std::find(addressedEntityIds.begin(), addressedEntityIds.end(), entityId) != addressedEntityIds.end();
}

bool EntityTalk::isAddressedToEntityOrNone(const std::string& entityId) const {
	return isAddressedToNone() || isAddressedToEntity(entityId);
}


}
