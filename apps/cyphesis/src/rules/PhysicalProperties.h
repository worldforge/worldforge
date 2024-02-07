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

#ifndef CYPHESIS_PHYSICALPROPERTIES_H
#define CYPHESIS_PHYSICALPROPERTIES_H

#include "Vector3Property.h"
#include "Point3Property.h"
#include "QuaternionProperty.h"

template <typename EntityT>
class PositionProperty : public Point3Property<EntityT> {
public:
	static constexpr const char* property_name = "pos";

	PositionProperty<EntityT>* copy() const override;

	static WFMath::Point<3> extractPosition(const EntityT& entity);

};

template <typename EntityT>
class VelocityProperty : public Vector3Property<EntityT> {
public:
	static constexpr const char* property_name = "velocity";

	VelocityProperty<EntityT>* copy() const override;

};

template <typename EntityT>
class AngularVelocityProperty : public Vector3Property<EntityT> {
public:
	static constexpr const char* property_name = "angular";

	AngularVelocityProperty<EntityT>* copy() const override;

};

template <typename EntityT>
class OrientationProperty : public QuaternionProperty<EntityT> {
public:
	static constexpr const char* property_name = "orientation";

	OrientationProperty<EntityT>* copy() const override;

};

/// \brief Class to handle whether or not an entity is solid for collisions.
/// \ingroup PropertyClasses
template <typename EntityT>
class SolidProperty : public BoolProperty<EntityT> {
public:

	static constexpr const char* property_name = "solid";

	SolidProperty* copy() const override;

	void apply(EntityT&) override;
};


#endif //CYPHESIS_PHYSICALPROPERTIES_H
