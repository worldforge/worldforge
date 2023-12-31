/*
 * Copyright (C) 2012 Peter Szucs <peter.szucs.dev@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef EMBERIWINDOWPROVIDER_H
#define EMBERIWINDOWPROVIDER_H

#include <string>

namespace Ember {


/**
 * @brief Provides a Window interface for the Input class.
 */
struct IWindowProvider {

	virtual ~IWindowProvider() = default;

	/**
	 * @brief Returns wether the window is visible.
	 */
	virtual bool isWindowVisible() = 0;

	/**
	 * @brief Returns the window handle as string.
	 */
	virtual std::string getWindowHandle() = 0;

	/**
	 * @brief Returns the window handle as string.
	 */
	virtual void getWindowSize(unsigned int& width, unsigned int& height) = 0;

	/**
	 * @brief Processes input for the window.
	 */
	virtual void processInput() = 0;

};

}

#endif
