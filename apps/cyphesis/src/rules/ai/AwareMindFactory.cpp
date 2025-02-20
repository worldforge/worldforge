/*
 Copyright (C) 2015 erik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "AwareMindFactory.h"
#include "AwareMind.h"


AwareMindFactory::AwareMindFactory(TypeStore<MemEntity>& typeStore)
		: mTypeStore(typeStore),
		  mSharedTerrain(new SharedTerrain()),
		  mAwarenessStoreProvider(new AwarenessStoreProvider(*mSharedTerrain)) {

}

BaseMind* AwareMindFactory::newMind(RouterId mind_id, const std::string& entity_id) const {
	return new AwareMind(mind_id, entity_id, mTypeStore, *mSharedTerrain, *mAwarenessStoreProvider);
}

