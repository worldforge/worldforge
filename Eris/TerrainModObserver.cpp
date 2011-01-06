//
// C++ Implementation: TerrainMod
//
// Description:
//
//
// Author: Tamas Bates <rhymer@gmail.com>, (C) 2008
// Author: Erik Hjortsberg <erik@worldforge.org>, (C) 2008
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
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.//
//
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eris/TerrainModObserver.h>
#include <Eris/TerrainMod.h>

#include <Mercator/TerrainMod.h>
#include <Eris/Log.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;

namespace Eris {

TerrainModObserver::TerrainModObserver(Entity* entity)
: mEntity(entity)
, mInnerMod(0)
{
}


TerrainModObserver::~TerrainModObserver()
{
}

Mercator::TerrainMod* TerrainModObserver::getMod() const
{
    if (mInnerMod) {
        return mInnerMod->getModifier();
    }
    return 0;
}

bool TerrainModObserver::init(bool alwaysObserve)
{
    bool successfulParsing = parseMod();
    if (successfulParsing || alwaysObserve) {
        observeEntity();
    }
    return successfulParsing;
}

bool TerrainModObserver::parseMod()
{
    if (!mEntity->hasAttr("terrainmod")) {
        ///Don't log anything since it's expected that instances of this can be attached to entities where not terrainmod is present.
        return false;
    }

    const Element& modifier(mEntity->valueOfAttr("terrainmod"));

    if (!modifier.isMap()) {
        error() << "Terrain modifier is not a map";
        return false;
    }
    const MapType & modMap = modifier.asMap();

    mInnerMod = new TerrainModTranslator;
    if (mInnerMod) {
        if (mInnerMod->parseData(mEntity->getPosition(), mEntity->getOrientation(), modMap)) {
            return true;
        } else {
            delete mInnerMod;
            return false;
        }
    }


    return false;
}

void TerrainModObserver::reparseMod()
{
    TerrainModTranslator* oldMod = mInnerMod;
    mInnerMod = 0;
    if (parseMod()) {
        onModChanged();
    } else {
        ///If the parsing failed and there was an old mod, we need to temporarily set the inner mod to the old one while we emit the deleted event.
        if (oldMod) {
            mInnerMod = oldMod;
            onModDeleted();
            mInnerMod = 0;
        }
    }
    delete oldMod;
}

void TerrainModObserver::attributeChanged(const Element& attributeValue)
{
    reparseMod();
}

void TerrainModObserver::entity_Moved()
{
    reparseMod();
}

void TerrainModObserver::entity_Deleted()
{
    onModDeleted();
    delete mInnerMod;
}

void TerrainModObserver::observeEntity()
{
    mAttrChangedSlot.disconnect();
    if (mEntity) {
        mAttrChangedSlot = sigc::mem_fun(*this, &TerrainModObserver::attributeChanged);
        mEntity->observe("terrainmod", mAttrChangedSlot);
        mEntity->Moved.connect(sigc::mem_fun(*this, &TerrainModObserver::entity_Moved));
        mEntity->BeingDeleted.connect(sigc::mem_fun(*this, &TerrainModObserver::entity_Deleted));
    }
}

Entity* TerrainModObserver::getEntity() const
{
    return mEntity;
}

void TerrainModObserver::onModDeleted()
{
    ModDeleted.emit();
}

void TerrainModObserver::onModChanged()
{
    ModChanged.emit();
}
} // close Namespace Eris

