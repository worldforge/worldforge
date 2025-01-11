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


#include "AreaProperty.h"

#include <common/log.h>

#include "physics/Shape.h"
#include <wfmath/polygon.h>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;


namespace {
std::unique_ptr<MathShape<WFMath::Polygon>> newFromAtlas(const MapType& data) {
	auto I = data.find("type");
	if (I == data.end() || !I->second.isString()) {
		return nullptr;
	}
	const std::string& type = I->second.String();
	if (type == "polygon") {
		auto new_shape = std::make_unique<MathShape<WFMath::Polygon>>();
		int res = new_shape->fromAtlas(data);
		if (res != 0) {
			new_shape.reset();
		}
		return new_shape;
	} else {
		return {};
	}
}
}

AreaProperty::AreaProperty(const AreaProperty& other)
		: TerrainEffectorProperty(other),
		  mState(State{
				  .layer=other.mState.layer,
				  .shape = other.mState.shape ? std::unique_ptr<MathShape<WFMath::Polygon>>(other.mState.shape->copy()) : nullptr
		  }) {
}

/// \brief AreaProperty constructor
AreaProperty::AreaProperty() : TerrainEffectorProperty(), mState() {
}

AreaProperty::~AreaProperty() = default;

void AreaProperty::set(const Element& ent) {
	if (!ent.isMap()) {
		return;
	}
	m_data = ent.Map();

	auto I = m_data.find("shape");
	auto Iend = m_data.end();

	if (I == m_data.end() || !I->second.isMap()) {
		spdlog::error("Area shape data missing or is not map");
		return;
	}
	mState.shape = newFromAtlas(I->second.Map());

	I = m_data.find("layer");
	if (I != Iend && I->second.isInt()) {
		mState.layer = static_cast<int>(I->second.Int());
	} else {
		mState.layer = 0;
	}

	I = m_data.find("scaled");
	if (I != Iend && I->second.isInt()) {
		mState.scaled = I->second.Int() == 1;
	} else {
		mState.scaled = false;
	}
}

AreaProperty* AreaProperty::copy() const {
	return new AreaProperty(*this);
}
