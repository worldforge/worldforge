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

#include <Eris/TerrainMod.h>

#include <Mercator/TerrainMod.h>
#include <Eris/Entity.h>
#include <Eris/Log.h>
#include <Eris/TerrainMod_impl.h>

#include <Atlas/Message/Element.h>

#include <wfmath/atlasconv.h>

namespace Eris
{

InnerTerrainMod::InnerTerrainMod(TerrainMod& terrainMod, const std::string& typemod)
: mTerrainMod(terrainMod)
{
}

InnerTerrainMod::~InnerTerrainMod()
{
}

const std::string& InnerTerrainMod::getTypename() const
{
    return mTypeName;
}

WFMath::Point<3> InnerTerrainMod::parsePosition(const Atlas::Message::MapType& modElement)
{
    ///If the height is specified use that, else check for a height offset. If none is found, use the default height of the entity position
    WFMath::Point<3> pos = mTerrainMod.getEntity()->getPosition();
    Atlas::Message::MapType::const_iterator mod_I = modElement.find("height");
    if (mod_I != modElement.end()) {
        const Atlas::Message::Element& modHeightElem = mod_I->second;
        if (modHeightElem.isNum()) {
            float height = modHeightElem.asNum();
            pos.z() = height;
        }
    } else {
        mod_I = modElement.find("heightoffset");
        if (mod_I != modElement.end()) {
            const Atlas::Message::Element& modHeightElem = mod_I->second;
            if (modHeightElem.isNum()) {
                float heightoffset = modHeightElem.asNum();
                pos.z() += heightoffset;
            }
        }
    }
    return pos;
}

InnerTerrainModCrater::InnerTerrainModCrater(TerrainMod& terrainMod)
: InnerTerrainMod(terrainMod, "cratermod")
, mModifier_impl(0)
{
}

InnerTerrainModCrater::~InnerTerrainModCrater()
{
    delete mModifier_impl;
}

Mercator::TerrainMod* InnerTerrainModCrater::getModifier()
{
    return mModifier_impl->getModifier();
}


bool InnerTerrainModCrater::parseAtlasData(const Atlas::Message::MapType& modElement)
{
    WFMath::Point<3> pos = parsePosition(modElement);
    const Atlas::Message::Element* shapeMap(0);
    const std::string& shapeType = parseShape(modElement, &shapeMap);
    if (shapeMap) {
        if (shapeType == "ball") {
            InnerTerrainModCrater_impl<WFMath::Ball>* modifierImpl = new InnerTerrainModCrater_impl<WFMath::Ball>();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(*shapeMap, pos, mTerrainMod.getEntity()->getOrientation(), pos.z());
        }
    }
    error() << "Crater terrain mod defined with incorrect shape";
    return false;
}


InnerTerrainModSlope::InnerTerrainModSlope(TerrainMod& terrainMod)
: InnerTerrainMod(terrainMod, "slopemod")
, mModifier_impl(0)
{
}

InnerTerrainModSlope::~InnerTerrainModSlope()
{
    delete mModifier_impl;
}

Mercator::TerrainMod* InnerTerrainModSlope::getModifier()
{
    return mModifier_impl->getModifier();
}


bool InnerTerrainModSlope::parseAtlasData(const Atlas::Message::MapType& modElement)
{
    float dx, dy;
    // Get slopes
    Atlas::Message::MapType::const_iterator mod_I = modElement.find("slopes");
    if (mod_I != modElement.end()) {
        const Atlas::Message::Element& modSlopeElem = mod_I->second;
        if (modSlopeElem.isList()) {
            const Atlas::Message::ListType & slopes = modSlopeElem.asList();
            if (slopes.size() > 1) {
                if (slopes[0].isNum() && slopes[1].isNum()) {
                    dx = slopes[0].asNum();
                    dy = slopes[1].asNum();
                    WFMath::Point<3> pos = parsePosition(modElement);
                    const Atlas::Message::Element* shapeMap(0);
                    const std::string& shapeType = parseShape(modElement, &shapeMap);
                    if (shapeMap) {
                        if (shapeType == "ball") {
                            InnerTerrainModSlope_impl<WFMath::Ball>* modifierImpl = new InnerTerrainModSlope_impl<WFMath::Ball>();
                            mModifier_impl = modifierImpl;
                            return modifierImpl->createInstance(*shapeMap, pos, mTerrainMod.getEntity()->getOrientation(), pos.z(), dx, dy);
                        } else if (shapeType == "rotbox") {
                            InnerTerrainModSlope_impl<WFMath::RotBox>* modifierImpl = new InnerTerrainModSlope_impl<WFMath::RotBox>();
                            mModifier_impl = modifierImpl;
                            return modifierImpl->createInstance(*shapeMap, pos, mTerrainMod.getEntity()->getOrientation(), pos.z(), dx, dy);
                        } else if (shapeType == "polygon") {
                            InnerTerrainModSlope_impl<WFMath::Polygon>* modifierImpl = new InnerTerrainModSlope_impl<WFMath::Polygon>();
                            mModifier_impl = modifierImpl;
                            return modifierImpl->createInstance(*shapeMap, pos, mTerrainMod.getEntity()->getOrientation(), pos.z(), dx, dy);
                        }
                    }
                }
            }
        }
    }
    error() << "SlopeTerrainMod defined with incorrect shape";
    return false;
}


InnerTerrainModLevel::InnerTerrainModLevel(TerrainMod& terrainMod)
: InnerTerrainMod(terrainMod, "levelmod")
, mModifier_impl(0)
{
}

InnerTerrainModLevel::~InnerTerrainModLevel()
{
    delete mModifier_impl;
}

Mercator::TerrainMod* InnerTerrainModLevel::getModifier()
{
    return mModifier_impl->getModifier();
}

bool InnerTerrainModLevel::parseAtlasData(const Atlas::Message::MapType& modElement)
{
    WFMath::Point<3> pos = parsePosition(modElement);
    // Get level
    const Atlas::Message::Element* shapeMap(0);
    const std::string& shapeType = parseShape(modElement, &shapeMap);
    if (shapeMap) {
        if (shapeType == "ball") {
            InnerTerrainModLevel_impl<WFMath::Ball>* modifierImpl = new InnerTerrainModLevel_impl<WFMath::Ball>();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(*shapeMap, pos, mTerrainMod.getEntity()->getOrientation(), pos.z());
        } else if (shapeType == "rotbox") {
            InnerTerrainModLevel_impl<WFMath::RotBox>* modifierImpl = new InnerTerrainModLevel_impl<WFMath::RotBox>();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(*shapeMap, pos, mTerrainMod.getEntity()->getOrientation(), pos.z());
        } else if (shapeType == "polygon") {
            InnerTerrainModLevel_impl<WFMath::Polygon>* modifierImpl = new InnerTerrainModLevel_impl<WFMath::Polygon>();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(*shapeMap, pos, mTerrainMod.getEntity()->getOrientation(), pos.z());
        }
    }
    error() << "Level terrain mod defined with incorrect shape";
    return false;
}

InnerTerrainModAdjust::InnerTerrainModAdjust(TerrainMod& terrainMod)
: InnerTerrainMod(terrainMod, "adjustmod")
, mModifier_impl(0)
{
}

InnerTerrainModAdjust::~InnerTerrainModAdjust()
{
    delete mModifier_impl;
}

Mercator::TerrainMod* InnerTerrainModAdjust::getModifier()
{
    return mModifier_impl->getModifier();
}


bool InnerTerrainModAdjust::parseAtlasData(const Atlas::Message::MapType& modElement)
{

    WFMath::Point<3> pos = mTerrainMod.getEntity()->getPosition();
    const Atlas::Message::Element* shapeMap(0);
    const std::string& shapeType = parseShape(modElement, &shapeMap);
    if (shapeMap) {
        if (shapeType == "ball") {
            InnerTerrainModAdjust_impl<WFMath::Ball>* modifierImpl = new InnerTerrainModAdjust_impl<WFMath::Ball>();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(*shapeMap, pos, mTerrainMod.getEntity()->getOrientation(), pos.z());
        } else if (shapeType == "rotbox") {
            InnerTerrainModAdjust_impl<WFMath::RotBox>* modifierImpl = new InnerTerrainModAdjust_impl<WFMath::RotBox>();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(*shapeMap, pos, mTerrainMod.getEntity()->getOrientation(), pos.z());
        } else if (shapeType == "polygon") {
            InnerTerrainModAdjust_impl<WFMath::Polygon>* modifierImpl = new InnerTerrainModAdjust_impl<WFMath::Polygon>();
            mModifier_impl = modifierImpl;
            return modifierImpl->createInstance(*shapeMap, pos, mTerrainMod.getEntity()->getOrientation(), pos.z());
        }
    }
    error() << "Adjust terrain mod defined with incorrect shape";
    return false;
}


const std::string& InnerTerrainMod::parseShape(const Atlas::Message::MapType& modElement, const Atlas::Message::Element** shapeMap)
{
    Atlas::Message::MapType::const_iterator shape_I = modElement.find("shape");
    if (shape_I != modElement.end()) {
        const Atlas::Message::Element& shapeElement = shape_I->second;
        if (shapeElement.isMap()) {
            const Atlas::Message::MapType& localShapeMap = shapeElement.asMap();
            *shapeMap = &shapeElement;

            // Get shape's type
            Atlas::Message::MapType::const_iterator type_I = localShapeMap.find("type");
            if (type_I != localShapeMap.end()) {
                const Atlas::Message::Element& shapeTypeElem(type_I->second);
                if (shapeTypeElem.isString()) {
                    const std::string& shapeType = shapeTypeElem.asString();
                    return shapeType;
                }
            }
        }
    }
    static std::string empty("");
    return empty;
}

// template <typename InnerTerrainMod_implType>
// InnerTerrainMod_implType* InnerTerrainMod::createInnerTerrainMod_impInstance(const Atlas::Message::MapType& modElement)
// {
//
//  const Atlas::Message::MapType* shapeMap(0);
//  const std::string& shapeType = parseShape(modElement, &shapeMap);
//  if (shapeMap) {
//   if (shapeType == "ball") {
//    typename InnerTerrainMod_implType::template foo<WFMath::Ball<2> >();
//     return new typename InnerTerrainMod_implType::template<WFMath::Ball<2> >();
//   }
//  }
// }



TerrainMod::TerrainMod(Entity* entity)
: mEntity(entity)
, mInnerMod(0)
{
}


TerrainMod::~TerrainMod()
{
}

bool TerrainMod::init(bool alwaysObserve)
{
    bool successfulParsing = parseMod();
    if (successfulParsing || alwaysObserve) {
        observeEntity();
    }
    return successfulParsing;
}

bool TerrainMod::parseMod()
{
    if (!mEntity->hasAttr("terrainmod")) {
        ///Don't log anything since it's expected that instances of this can be attached to entities where not terrainmod is present.
        return false;
    }

    const Atlas::Message::Element& modifier(mEntity->valueOfAttr("terrainmod"));

    if (!modifier.isMap()) {
        error() << "Terrain modifier is not a map";
        return false;
    }
    const Atlas::Message::MapType & modMap = modifier.asMap();


    // Get modifier type
    Atlas::Message::MapType::const_iterator mod_I = modMap.find("type");
    if (mod_I != modMap.end()) {
        const Atlas::Message::Element& modTypeElem(mod_I->second);
        if (modTypeElem.isString()) {
            const std::string& modType = modTypeElem.asString();

            if (modType == "slopemod") {
                mInnerMod = new InnerTerrainModSlope(*this);
            } else if (modType == "levelmod") {
                mInnerMod = new InnerTerrainModLevel(*this);
            } else if (modType == "adjustmod") {
                mInnerMod = new InnerTerrainModAdjust(*this);
            } else  if (modType == "cratermod") {
                mInnerMod = new InnerTerrainModCrater(*this);
            } else {
                error() << "'" << modType << "' isn't a recognized terrain mod type.";
           }
        } else {
            error() << "Mod type must be a string value.";
        }
    } else {
        error() << "No type defined for terrain mod.";
    }
    if (mInnerMod) {
        if (mInnerMod->parseAtlasData(modMap)) {
            return true;
        } else {
            delete mInnerMod;
            return false;
        }
    }


    return false;
}

void TerrainMod::reparseMod()
{
    InnerTerrainMod* oldMod = mInnerMod;
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

void TerrainMod::attributeChanged(const Atlas::Message::Element& attributeValue)
{
    reparseMod();
}

void TerrainMod::entity_Moved()
{
    reparseMod();
}

void TerrainMod::entity_Deleted()
{
    onModDeleted();
    delete mInnerMod;
}

void TerrainMod::observeEntity()
{
    mAttrChangedSlot.disconnect();
    if (mEntity) {
        mAttrChangedSlot = sigc::mem_fun(*this, &TerrainMod::attributeChanged);
        mEntity->observe("terrainmod", mAttrChangedSlot);
        mEntity->Moved.connect(sigc::mem_fun(*this, &TerrainMod::entity_Moved));
        mEntity->BeingDeleted.connect(sigc::mem_fun(*this, &TerrainMod::entity_Deleted));
    }
}

Entity* TerrainMod::getEntity() const
{
    return mEntity;
}

void TerrainMod::onModDeleted()
{
    ModDeleted.emit();
}

void TerrainMod::onModChanged()
{
    ModChanged.emit();
}
} // close Namespace Eris

