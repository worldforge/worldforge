// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2016 Erik Ogenvik
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

#ifndef RULESETS_DENSITY_PROPERTY_H
#define RULESETS_DENSITY_PROPERTY_H

#include "common/Property.h"
#include "rules/simulation/LocatedEntity.h"

/**
 * \brief Density property updates the mass automatically when the size of the entity changes.
 *
 * Density is expressed as kg/m3.
 *
 * \ingroup PropertyClasses
 */
class DensityProperty : public Property<double, LocatedEntity> {
public:

	static constexpr const char* property_name = "density";

	explicit DensityProperty() = default;

	void apply(LocatedEntity&) override;

	DensityProperty* copy() const override;

	void updateMass(LocatedEntity& entity) const;

protected:
	DensityProperty(const DensityProperty& rhs) = default;
};

#endif // RULESETS_DENSITY_PROPERTY_H
