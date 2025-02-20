//
// C++ Implementation: CustomAdapter
//
// Description: 
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2007
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.//
//
#include "CustomAdapter.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;


namespace Ember::OgreView::Gui::Adapters::Atlas {

CustomAdapter::CustomAdapter(const ::Atlas::Message::Element& element)
		: AdapterBase(element) {
}


CustomAdapter::~CustomAdapter() = default;

void CustomAdapter::updateGui(const ::Atlas::Message::Element& element) {
	QueryUpdateGui(&element);
}

void CustomAdapter::fillElementFromGui() {
	Element element;
	QueryFillElementFromGui(&element);
}

bool CustomAdapter::_hasChanges() {
	bool hasChanges;
	QueryHasChanges(hasChanges);
	return hasChanges;
}

::Atlas::Message::Element CustomAdapter::_getChangedElement() {
	Element element;
	QueryGetChangedElement(&element);
	return element;
}


}







