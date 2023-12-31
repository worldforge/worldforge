/*
 Copyright (C) 2010 Erik Ogenvik <erik@ogenvik.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef EMBEROGREILIGHTNING_H_
#define EMBEROGREILIGHTNING_H_

namespace WFMath {
template<int>
class Vector;
}

namespace Ogre {
class ColourValue;
}


namespace Ember::OgreView {

/**
 * @brief Provider of main light (like a sun or moon) lightning information.
 * @author Erik Ogenvik <erik@ogenvik.org>
 */
struct ILightning {
	virtual ~ILightning() = default;

	/**
	 * @brief Gets the direction of the main light, in world space.
	 * @returns The direction of the main light, in world space.
	 */
	virtual WFMath::Vector<3> getMainLightDirection() const = 0;

	/**
	 * @brief Gets the default ambient light colour.
	 * @returns The default ambient light colour.
	 */
	virtual Ogre::ColourValue getAmbientLightColour() const = 0;

};
}



#endif /* EMBEROGREILIGHTNING_H_ */
