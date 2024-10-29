/*
 Copyright (C) 2024 Erik Ogenvik

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

#include <functional>
#include <mutex>
#include <boost/noncopyable.hpp>
#include <gsl/pointers>

/**
 * Hides state behind a mutex.
 *
 * Note that the callback takes a "gsl::not_null" instance. Initially we used a reference, but it was too easy to by mistake declare the callback with a value type, thus inadvertently causing
 * a copy operation.
 * @tparam T
 */
template<typename T>
class SynchedState : boost::noncopyable {
private:
	mutable std::mutex mMutex;
	T mState;
public:
	template<typename TReturn>
	TReturn withState(std::function<TReturn(gsl::not_null<T*> state)> block);

	void withState(std::function<void(gsl::not_null<T*> state)> block);

	template<typename TReturn>
	TReturn withStateConst(std::function<TReturn(gsl::not_null<const T*> state)> block) const;

	void withStateConst(std::function<void(gsl::not_null<const T*> state)> block) const;
};
