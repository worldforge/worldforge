/*
 Copyright (C) 2009 Erik Ogenvik

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

#ifndef IMOVEMENTPROVIDER_H_
#define IMOVEMENTPROVIDER_H_

namespace WFMath {
template<int>
class Vector;
}


namespace Ember::OgreView {

/**
 * @author Erik Ogenvik <erik@ogenvik.org>
 * @brief Provides movement updates for a frame.
 * A typical user of this interface would be a camera which needs to get movement updates each frame.
 */
struct IMovementProvider {
	/**
	 * @brief Dtor.
	 */
	virtual ~IMovementProvider() = default;

	/**
	 * @brief Provide the movement for the current frame.
	 * @return The movement for the current frame.
	 */
	virtual WFMath::Vector<3> getMovementForCurrentFrame() const = 0;
};

}



#endif /* IMOVEMENTPROVIDER_H_ */
