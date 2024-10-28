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


#include "boost/noncopyable.hpp"
#include <functional>
#include <mutex>

/**
 * Hides state behind a mutex.
 * @tparam T
 */
template<typename T>
class SynchedState : boost::noncopyable {
private:
	mutable std::mutex mMutex;
	T mState;
public:
	template<typename TReturn>
	TReturn withState(std::function<TReturn(T& state)> block);

	void withState(std::function<void(T& state)> block);

	template<typename TReturn>
	TReturn withStateConst(std::function<TReturn(const T& state)> block) const;

	void withStateConst(std::function<void(const T& state)> block) const;
};
