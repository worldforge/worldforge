/*
 Copyright (C) 2011 Erik Ogenvik

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

#ifndef ENTITYTALK_H_
#define ENTITYTALK_H_

#include <Atlas/Objects/ObjectsFwd.h>
#include <string>
#include <vector>

namespace Ember {

/**
 * @author Erik Ogenvik <erik@ogenvik.org>
 * @brief Represent a talk operation from an entity.
 *
 * A talk operation primarily contains a message. It can however also contain a list of suggested responses.
 * In addition, it can be addressed to zero or many entities.
 * Just because it's addressed to entities different from the user's avatar doesn't mean that the it can't be heard, but usually it means that the message isn't of that much important to the user.
 */
struct EntityTalk {

	static EntityTalk parse(const Atlas::Objects::Root& talk);

	/**
	 * @brief Checks if the message isn't addressed to anyone in particular.
	 * @return True if the message isn't addressed to anyone in particular.
	 */
	bool isAddressedToNone() const;

	/**
	 * @brief Checks if the message is addressed to the specified entity.
	 * @param entityId The id of the entity to check.
	 * @return True if the message is addressed to the specified entity. Note that this will also return false if the message isn't addressed to anyone.
	 */
	bool isAddressedToEntity(const std::string& entityId) const;

	/**
	 * @brief Checks if the message is addressed to the specified entity, or to none.
	 * @param entityId The id of the entity to check.
	 * @return True if the message is addressed to the specified entity or if the message isn't addressed to anyone specific.
	 */
	bool isAddressedToEntityOrNone(const std::string& entityId) const;


	/**
	 * Any message that's being said.
	 */
	std::string message;

	/**
	 * Any sound that's made.
	 */
	std::string sound;

	/**
	 * @brief A list of suggested responses. This might be empty if there are none.
	 */
	std::vector<std::string> suggestedResponses;

	/**
	 * @brief A list of the addressed entities. This might be empty is the message isn't addressed to anyone in particular.
	 */
	std::vector<std::string> addressedEntityIds;
};
} /* namespace Ember */
#endif /* ENTITYTALK_H_ */
