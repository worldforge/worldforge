/*
 Copyright (C) 2017 Erik Ogenvik

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

#ifndef ERIS_ACTIVEMARKER_H
#define ERIS_ACTIVEMARKER_H

#include <boost/noncopyable.hpp>
#include <memory>

namespace  Eris {
/**
 *
 * To be used with EventService::runOnMainThread.
 *
 * Use an instance of this as a field on your class to handle cancellation of handlers automatically when your
 * instance is destroyed. The destructor will automatically set the marker to "false".
 */
class ActiveMarker : private boost::noncopyable {
public:
	/**
	 * Ctor. Will initialize the marker to "true".
	 */
	ActiveMarker();

	/**
	 * Dtor. Will set the marker to "false".
	 */
	~ActiveMarker();

	/**
	 * Implicit conversion operator for shared_ptr<bool>.
	 * @return
	 */
	operator std::shared_ptr<bool>();

	const std::shared_ptr<bool> getMarker() const;

private:
	std::shared_ptr<bool> m_marker;
};

}

#endif //ERIS_ACTIVEMARKER_H
