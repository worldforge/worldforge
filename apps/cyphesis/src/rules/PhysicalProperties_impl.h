/*
 Copyright (C) 2021 Erik Ogenvik

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
#pragma once

#include "PhysicalProperties.h"
#include "QuaternionProperty_impl.h"
#include "Point3Property_impl.h"
#include "Vector3Property_impl.h"
#include "common/Property_impl.h"

template<typename EntityT>
PositionProperty<EntityT>* PositionProperty<EntityT>::copy() const {
	return new PositionProperty(*this);
}

template<typename EntityT>
WFMath::Point<3> PositionProperty<EntityT>::extractPosition(const EntityT& entity) {
	auto posProp = entity.template getPropertyClassFixed<PositionProperty>();
	if (posProp) {
		return posProp->data();
	}
	return {};
}

template<typename EntityT>
VelocityProperty<EntityT>* VelocityProperty<EntityT>::copy() const {
	return new VelocityProperty(*this);
}

template<typename EntityT>
AngularVelocityProperty<EntityT>* AngularVelocityProperty<EntityT>::copy() const {
	return new AngularVelocityProperty(*this);
}

template<typename EntityT>
OrientationProperty<EntityT>* OrientationProperty<EntityT>::copy() const {
	return new OrientationProperty(*this);
}

template<typename EntityT>
SolidProperty<EntityT>* SolidProperty<EntityT>::copy() const {
	return new SolidProperty(*this);
}

template<typename EntityT>
void SolidProperty<EntityT>::apply(EntityT& owner) {
//    owner.m_location.setSolid(isTrue());
}
