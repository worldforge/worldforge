/*
 Copyright (C) 2009 Erik Ogenvik <erik@ogenvik.org>

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

#include "DeepAttributeObserver.h"
#include <Eris/Entity.h>

namespace Ember {

DeepAttributeObserver::DeepAttributeObserver(Eris::Entity& entity,
											 sigc::signal<void(const Atlas::Message::Element&)>& eventChanged,
											 const std::vector<std::string>& elementPath)
		: mEntity(entity),
		  mEventChanged(eventChanged),
		  mElementPath(elementPath) {
	const std::string& firstAttributeName = elementPath.front();
	entity.observe(firstAttributeName, sigc::mem_fun(*this, &DeepAttributeObserver::entity_AttrChanged), true);
}

DeepAttributeObserver::~DeepAttributeObserver() = default;

void DeepAttributeObserver::forceEvaluation() {
	const std::string& firstAttributeName = mElementPath.front();
	if (mEntity.hasProperty(firstAttributeName)) {
		mEventChanged(getCurrentAttribute(mEntity.valueOfProperty(firstAttributeName)));
	}
}


void DeepAttributeObserver::entity_AttrChanged(const Atlas::Message::Element& attributeValue) {
	const Atlas::Message::Element& newElement = getCurrentAttribute(attributeValue);
	if (mLastElementValue != newElement) {
		mEventChanged(newElement);
		mLastElementValue = newElement;
	}

}

const Atlas::Message::Element& DeepAttributeObserver::getCurrentAttribute(const Atlas::Message::Element& baseAttribute) const {
	static Atlas::Message::Element nullElement;
	if (baseAttribute.isMap()) {
		const Atlas::Message::Element* currentElement = &baseAttribute;
		auto I = mElementPath.begin();
		I++;
		for (; I != mElementPath.end(); ++I) {
			if (currentElement->isMap()) {
				auto mapI = currentElement->asMap().find(*I);
				if (mapI != currentElement->asMap().end()) {
					currentElement = &(mapI->second);
					continue;
				}
			}
			return nullElement;
		}
		return *currentElement;
	}
	return nullElement;
}

}
