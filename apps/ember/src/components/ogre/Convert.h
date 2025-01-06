/*
-----------------------------------------------------------------------------
Convert.h by Miguel Guzman Miranda (Aglanor)
Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2005

Copyright � 2003 The Worldforge Project (http://www.worldforge.org)
This file is part of Ember client (http://www.worldforge.org/dev/eng/clients/Ember)

This code is distributed under the GNU GPL (General Public License).
See file COPYING for details.

-----------------------------------------------------------------------------

-----------------------------------------------------------------------------
Point, Vector and Quaternion converter
for between Ogre and Atlas.


-----------------------------------------------------------------------------
*/
#ifndef EMBER_CONVERT_H_
#define EMBER_CONVERT_H_

#include <wfmath/point.h>
#include <wfmath/vector.h>
#include <wfmath/axisbox.h>
#include <wfmath/quaternion.h>
#include <wfmath/const.h>

#include <OgreVector.h>
#include <OgreQuaternion.h>
#include <OgreAxisAlignedBox.h>
#include <OgrePrerequisites.h>
#include <OgreCommon.h>

#include <cassert>

namespace Ember::OgreView {

/**
 * @author Erik Ogenvik <erik@ogenvik.org>
 * @brief Useful conversion method, for converting between Ogre and Worldforge space.
 */
class Convert {
public:
/**
 * @brief Converts a 2d WFMath::Point<2> into an Ogre structure.
 * @param p A 2d Worldforge position.
 */
	template<typename T>
	static T toOgre(const WFMath::Point<2>& p);

/**
 * @brief Converts a 2d vector into a 2d Ogre vector.
 * @param p A 2d Worldforge vector.
 * @return A 2d Ogre vector.
 */
	static Ogre::Vector2 toOgre(const WFMath::Vector<2>& p);

/**
 * @brief Converts an Ogre 3d vector into a Worldforge structure.
 * @param p A 3d Ogre vector.
 */
	template<typename T>
	static T toWF(const Ogre::Vector3& p);

/**
 * @brief Converts an Ogre 2d vector into a Worldforge 2d point.
 * @param p A 2d Ogre vector.
 * @return A 2d Worldforge point.
 */
	static WFMath::Point<2> toWF(const Ogre::Vector2& p);

/**
 * @brief Converts a Worldforge 3d point into an Ogre 3d vector.
 * @param p A 3d Worldforge point.
 * @return A 3d Ogre vector.
 */
	static Ogre::Vector3 toOgre(const WFMath::Point<3>& p);

/**
 * @brief Converts a Worldforge 3d vector into an Ogre 3d vector.
 * @param v A 3d Worldforge vector.
 * @return A 3d Ogre vector.
 */
	static Ogre::Vector3 toOgre(const WFMath::Vector<3>& v);

/**
 * @brief Converts a Worldforge quaternion into an Ogre quaternion.
 * @param aq A Worldforge quaternion.
 * @return An Ogre quaternion.
 */
	static Ogre::Quaternion toOgre(const WFMath::Quaternion& aq);

/**
 * @brief Converts an Ogre quaternion into a Worldforge quaternion.
 * @param aq An Ogre quaternion.
 * @return A Worldforge quaternion.
 */
	static WFMath::Quaternion toWF(const Ogre::Quaternion& aq);

/**
 * @brief Converts a Worldforge 3d axisbox into an Ogre 3d axisbox.
 * @param axisBox A Worldforge 3d axisbox.
 * @return An Ogre 3d axisbox.
 */
	static Ogre::AxisAlignedBox toOgre(const WFMath::AxisBox<3>& axisBox);

/**
 * @brief Converts a Worldforge 2d axisbox into an Ogre TRect.
 * @param atlasBox A Worldforge 2d axisbox.
 * @return An Ogre TRect.
 */
	static Ogre::TRect<Ogre::Real> toOgre(const WFMath::AxisBox<2>& atlasBox);


/**
 * @brief Converts an Ogre 3d axisbox into a Worldforge 3d axisbox.
 * @param axisBox An Ogre 3d axisbox.
 * @return A Worldforge 3d axisbox.
 */
	static WFMath::AxisBox<3> toWF(const Ogre::AxisAlignedBox& axisBox);

/**
 * @brief Converts an Ogre TRect into a Worldforge 2d axisbox.
 * @param bounds An Ogre TRect instance.
 * @return A Worldforge 2d axisbox.
 */
	static WFMath::AxisBox<2> toWF(const Ogre::TRect<Ogre::Real>& bounds);

};

///Implementations

template<>
inline Ogre::Vector3 Convert::toOgre<Ogre::Vector3>(const WFMath::Point<2>& p) {
	assert(p.isValid() && "Never convert an invalid WFMath point into Ogre as there will only be pain on the other side.");
	return {static_cast<Ogre::Real>(p.x()),
			0,
			static_cast<Ogre::Real>(p.y())};
}

template<>
inline Ogre::Vector2 Convert::toOgre<Ogre::Vector2>(const WFMath::Point<2>& p) {
	assert(p.isValid() && "Never convert an invalid WFMath point into Ogre as there will only be pain on the other side.");
	return {static_cast<Ogre::Real>(p.x()),
			static_cast<Ogre::Real>(p.y())};
}

inline Ogre::Vector2 Convert::toOgre(const WFMath::Vector<2>& p) {
	assert(p.isValid() && "Never convert an invalid WFMath vector into Ogre as there will only be pain on the other side.");
	return {static_cast<Ogre::Real>(p.x()),
			static_cast<Ogre::Real>(p.y())};
}


inline WFMath::Point<2> Convert::toWF(const Ogre::Vector2& p) {
	return {p.x, p.y};
}

template<>
inline WFMath::Point<3> Convert::toWF<WFMath::Point<3>>(const Ogre::Vector3& p) {
	return {p.x, p.y, p.z};
}

template<>
inline WFMath::Point<2> Convert::toWF<WFMath::Point<2>>(const Ogre::Vector3& p) {
	return {p.x, p.z};
}

template<>
inline WFMath::Vector<3> Convert::toWF<WFMath::Vector<3>>(const Ogre::Vector3& p) {
	return {p.x, p.y, p.z};
}

inline Ogre::Vector3 Convert::toOgre(const WFMath::Point<3>& p) {
	assert(p.isValid() && "Never convert an invalid WFMath point into Ogre as there will only be pain on the other side.");
	return {static_cast<Ogre::Real>(p.x()),
			static_cast<Ogre::Real>(p.y()),
			static_cast<Ogre::Real>(p.z())};
}

inline Ogre::Vector3 Convert::toOgre(const WFMath::Vector<3>& v) {
	assert(v.isValid() && "Never convert an invalid WFMath vector into Ogre as there will only be pain on the other side.");
	return {static_cast<Ogre::Real>(v.x()),
			static_cast<Ogre::Real>(v.y()),
			static_cast<Ogre::Real>(v.z())};
}

inline Ogre::Quaternion Convert::toOgre(const WFMath::Quaternion& aq) {
	assert(aq.isValid() && "Never convert an invalid WFMath quaternion into Ogre as there will only be pain on the other side.");
	return {static_cast<Ogre::Real>(aq.scalar()),
			static_cast<Ogre::Real>(aq.vector().x()),
			static_cast<Ogre::Real>(aq.vector().y()),
			static_cast<Ogre::Real>(aq.vector().z())};
}

inline WFMath::Quaternion Convert::toWF(const Ogre::Quaternion& aq) {
	return {aq.w, aq.x, aq.y, aq.z};
}

inline Ogre::AxisAlignedBox Convert::toOgre(const WFMath::AxisBox<3>& axisBox) {
	assert(axisBox.isValid() && "Never convert an invalid WFMath axis box into Ogre as there will only be pain on the other side.");
	return {static_cast<Ogre::Real>(axisBox.lowCorner().x()),
			static_cast<Ogre::Real>(axisBox.lowCorner().y()),
			static_cast<Ogre::Real>(axisBox.lowCorner().z()),
			static_cast<Ogre::Real>(axisBox.highCorner().x()),
			static_cast<Ogre::Real>(axisBox.highCorner().y()),
			static_cast<Ogre::Real>(axisBox.highCorner().z())};
}

inline Ogre::TRect<Ogre::Real> Convert::toOgre(const WFMath::AxisBox<2>& atlasBox) {
	assert(atlasBox.isValid() && "Never convert an invalid WFMath axis box into Ogre as there will only be pain on the other side.");
	return Ogre::TRect<Ogre::Real>(static_cast<Ogre::Real>(atlasBox.lowCorner().x()),
								   static_cast<Ogre::Real>(atlasBox.lowCorner().y()),
								   static_cast<Ogre::Real>(atlasBox.highCorner().x()),
								   static_cast<Ogre::Real>(atlasBox.highCorner().y()));
}

inline WFMath::AxisBox<3> Convert::toWF(const Ogre::AxisAlignedBox& axisBox) {
	if (axisBox.isNull() || axisBox.isInfinite()) {
		return {};
	}
	return WFMath::AxisBox<3>(WFMath::Point<3>(axisBox.getMinimum().x, axisBox.getMinimum().y, axisBox.getMinimum().z),
							  WFMath::Point<3>(axisBox.getMaximum().x, axisBox.getMaximum().y, axisBox.getMaximum().z));
}

inline WFMath::AxisBox<2> Convert::toWF(const Ogre::TRect<Ogre::Real>& bounds) {
	return WFMath::AxisBox<2>(WFMath::Point<2>(bounds.left, bounds.bottom),
							  WFMath::Point<2>(bounds.right, bounds.top));
}

}

#endif
