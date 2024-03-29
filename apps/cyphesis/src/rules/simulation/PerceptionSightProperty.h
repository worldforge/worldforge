/*
 Copyright (C) 2018 Erik Ogenvik

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

#ifndef CYPHESIS_PERCEPTIONSIGHTPROPERTY_H
#define CYPHESIS_PERCEPTIONSIGHTPROPERTY_H

#include "common/Property.h"
#include "rules/simulation/LocatedEntity.h"

/**
 * Determines whether the entity is perceptive or not, and by which factor [0..1].
 *
 * \ingroup PropertyClasses
 */
class PerceptionSightProperty : public Property<double, LocatedEntity> {
public:
	static constexpr const char* property_name = "perception_sight";

	PerceptionSightProperty() = default;

	~PerceptionSightProperty() override = default;

	void apply(LocatedEntity&) override;

	PerceptionSightProperty* copy() const override;

protected:
	PerceptionSightProperty(const PerceptionSightProperty& rhs) = default;

};


#endif //CYPHESIS_PERCEPTIONSIGHTPROPERTY_H
