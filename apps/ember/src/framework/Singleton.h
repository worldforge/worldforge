/*
    Copyright (C) 2004  Erik Ogenvik
    
    Taken from the file OgreSingleton.h from the Ogre project, release 0.14
    (www.ogre3d.org)

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

#ifndef EMBER_SINGLETON_H
#define EMBER_SINGLETON_H

#include <cassert>
#include <boost/noncopyable.hpp>

namespace Ember {

/**
@brief Template class for creating single-instance global classes.

Whenever you need a singleton in the system you should inherit from this, such as
@code
class Foo : public Ember::Singleton<Foo>
@endcode

Whenever you need the singleton instance in your project you can then get it by calling @see getSingleton() or @see getSingletonPtr()
Note that you still have to provide lifetime service and ownership and need to both create and destroy the instance where appropriate.
*/
template<typename T>
class Singleton : public boost::noncopyable {
protected:

	/**
	 * @brief Standard constructor.
	 */
	explicit Singleton() {
		assert(!ms_Singleton);
		ms_Singleton = static_cast<T*>( this );
	}

	virtual ~Singleton() {
		assert(ms_Singleton);
		ms_Singleton = nullptr;
	}

private:
	/**
	@brief The static variable holding the singleton instance.
	Remember to instantiate this to nullptr in your implementation.
	*/
	static T* ms_Singleton;

public:

	/**
	 *       @brief Gets the singleton instance.
	 * @return The singleton instance.
	 */
	static T& getSingleton() {
		assert(ms_Singleton);
		return (*ms_Singleton);
	}

	/**
	 *       @brief Gets a pointer to the singleton instance.
	 * @return A pointer to the singleton instance.
	 */
	static T* getSingletonPtr() {
		return ms_Singleton;
	}

	/**
	 *       @brief Returns true if there's a singleton registered with the system.
	 * @return True if there's a singleton available.
	 */
	static bool hasInstance() {
		return ms_Singleton != nullptr;
	}
};

template<typename T> T* Singleton<T>::ms_Singleton = nullptr;


}
#endif

