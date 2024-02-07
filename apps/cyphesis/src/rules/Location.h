// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2005 Alistair Riddoch
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


#ifndef RULESETS_LOCATION_H
#define RULESETS_LOCATION_H

#include "EntityLocation.h"
#include "physics/Vector3D.h"
#include "physics/BBox.h"
#include "physics/Quaternion.h"

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/ObjectsFwd.h>

#include <wfmath/axisbox.h>
#include <wfmath/point.h>
#include <wfmath/quaternion.h>

#include <optional>
#include <iostream>

class EntityT;

template<typename EntityT>
class Location : public EntityLocation<EntityT> {
protected:

	double m_timeStamp;

	float m_radius; // Radius of bounding sphere of box
	float m_squareRadius;
public:
	/**
	 * Velocity vector, relative to m_parent entity.
	 */
	Vector3D m_velocity;
	/**
	 * Angular velocity of the entity, i.e. how it's being continuously rotated.
	 */
	Vector3D m_angularVelocity;
	Quaternion m_orientation;

	BBox m_bBox;

	Location();

	explicit Location(Ref<EntityT> rf);

	explicit Location(Ref<EntityT> rf,
					  const Point3D& pos);

	explicit Location(Ref<EntityT> rf,
					  const Point3D& pos,
					  Vector3D velocity);

	explicit Location(EntityT* rf);

	explicit Location(EntityT* rf,
					  const Point3D& pos);

	explicit Location(EntityT* rf,
					  const Point3D& pos,
					  Vector3D velocity);

	explicit Location(EntityLocation<EntityT> entityLocation);


	float radius() const { return m_radius; }

	float squareRadius() const { return m_squareRadius; }

	const Vector3D& velocity() const { return m_velocity; }

	const Quaternion& orientation() const { return m_orientation; }

	const BBox& bBox() const { return m_bBox; }

	const double& timeStamp() const {
		return m_timeStamp;
	}

	void update(const double& time) {
		m_timeStamp = time;
	}

	void setBBox(const BBox& b) {
		m_bBox = b;
		modifyBBox();
	}

	void resetTransformAndMovement() {
		this->m_pos = {};
		m_orientation = {};
		m_velocity = {};
		m_angularVelocity = {};
	}

	void addToMessage(Atlas::Message::MapType& ent) const;

	void addToEntity(const Atlas::Objects::Entity::RootEntity& ent) const;

	/**
	 * Reads data from a map message.
	 * @return True if map contained location data.
	 */
	bool readFromMessage(const Atlas::Message::MapType& message);

	/**
	 * Reads data from an entity message.
	 * @return True if entity contained location data.
	 */
	bool readFromEntity(const Atlas::Objects::Entity::RootEntity& ent);

	void modifyBBox();

	template<typename T>
	friend std::ostream& operator<<(std::ostream& s, Location<T>& v);
};

template<typename EntityT>
Vector3D distanceTo(const Location<EntityT>& self, const Location<EntityT>& other);

template<typename EntityT>
Point3D relativePos(const Location<EntityT>& self, const Location<EntityT>& other);

template<typename EntityT>
std::optional<WFMath::CoordType> squareDistance(const Location<EntityT>& self, const Location<EntityT>& other);

/**
 * \brief Gets the squared distance between the two supplied location, if possible, along with the common ancestor.
 *
 * Calculates the square distance between the entities, along with the common ancestor. If no common ancestor can be found
 * the ancestor parameter will point to a null value, and the distance returned should be ignored.
 *
 * If you suspect that one location might be a child location of the other location, send the parent location as the first parameter.
 * This speeds up the calculation.
 *
 * @param self One location.
 * @param other A second location.
 * @param ancestor Any ancestor location will be placed here.
 * @return The distance, squared. Note that this value is invalid if no ancestor could be found, so make sure to check the return value of the "ancestor" parameter.
 */
template<typename EntityT>
WFMath::CoordType squareDistanceWithAncestor(const Location<EntityT>& self, const Location<EntityT>& other, const Location<EntityT>** ancestor);

template<typename EntityT>
std::optional<WFMath::CoordType> squareHorizontalDistance(const Location<EntityT>& self, const Location<EntityT>& other);

template<typename EntityT>
inline std::optional<WFMath::CoordType> distance(const Location<EntityT>& self, const Location<EntityT>& other) {
	auto distance = squareDistance(self, other);
	if (!distance) {
		return {};
	}
	return std::sqrt(*distance);
}


#endif // RULESETS_LOCATION_H
