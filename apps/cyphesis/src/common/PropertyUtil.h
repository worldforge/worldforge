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

#ifndef WORLDFORGE_PROPERTYUTIL_H
#define WORLDFORGE_PROPERTYUTIL_H

#include "Property.h"
#include "common/ModifierType.h"
#include <string>


struct PropertyUtil {

	/**
	 * Extract the property visibility flags from the name.
	 * Names that starts with "__" are "private". Only visible to the simulation and to administrators.
	 * Names that starts with "_" are "protected". Only visible to the entity it belongs, the simulation and to administrators.
	 * All other properties are "public", i.e. visible to everyone.
	 * @param name A property name.
	 * @return
	 */
	static std::uint32_t flagsForPropertyName(const std::string& name);

	/**
	 * Checks if the name supplied is a valid property name.
	 *
	 * It should not be more than 32 characters, and can only contain ascii characters or numbers, dollar sign ("$"), underscores ("_") or hyphens ("-").
	 */
	static bool isValidName(const std::string& name);

	static std::pair<ModifierType, std::string> parsePropertyModification(const std::string& propertyName);
};

#endif //WORLDFORGE_PROPERTYUTIL_H
