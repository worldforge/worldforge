//
// C++ Implementation: PolygonPointPickListener
//
// Description:
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2009
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
#include "PolygonPointPickListener.h"
#include "PolygonPointUserObject.h"
#include "PolygonPoint.h"

namespace Ember::OgreView::Authoring {

PolygonPointPickListener::PolygonPointPickListener(Polygon& polygon) :
		mPolygon(polygon), mPickedUserObject(nullptr) {
}

void PolygonPointPickListener::processPickResult(bool& continuePicking, PickResult& result, Ogre::Ray& /*cameraRay */, const MousePickerArgs& mousePickerArgs) {
	if (result.collisionInfo.type() == typeid(PolygonPointUserObject*)) {
		continuePicking = false;
		if (mousePickerArgs.pickType == MPT_PRESS) {
			//TODO: make sure that it's a point which belongs to our polygon
			mPickedUserObject = std::any_cast<PolygonPointUserObject*>(result.collisionInfo);
		}
	}
}

void PolygonPointPickListener::processDelayedPick(const MousePickerArgs& /*mousePickerArgs*/) {
	//We don't do any delayed selection
}


void PolygonPointPickListener::initializePickingContext(bool& willParticipate, const MousePickerArgs& pickArgs) {
	//We will only react on press events, but we want do silence click and pressed events if they happen with our markers too.
	if (pickArgs.pickType == MPT_PRESS || pickArgs.pickType == MPT_CLICK || pickArgs.pickType == MPT_PRESSED) {
		willParticipate = true;
		mPickedUserObject = nullptr;
	}

}

void PolygonPointPickListener::endPickingContext(const MousePickerArgs& /*mousePickerArgs*/, const std::vector<PickResult>& results) {
	if (mPickedUserObject) {
		EventPickedPoint.emit(mPickedUserObject->getPoint());
		mPickedUserObject = nullptr;
	}
}

}



