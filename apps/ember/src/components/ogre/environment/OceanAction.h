/*
 Copyright (C) 2009 Erik Ogenvik <erik@ogenvik.org>

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

#ifndef OCEANACTION_H_
#define OCEANACTION_H_

#include "components/entitymapping/Actions/Action.h"
#include <functional>

namespace Ember {
class EmberEntity;
namespace OgreView {
class Scene;
namespace Environment {

class OceanRepresentation;

/**
 * @author Erik Ogenvik <erik@ogenvik.org>
 *
 * @brief Creates an OceanRepresentation and binds it to an entity.
 */
class OceanAction : public EntityMapping::Actions::Action {
public:
	/**
	 * @brief Ctor.
	 */
	explicit OceanAction(std::function<void(bool)> attachmentFunction);

	/**
	 * @brief Dtor.
	 */
	~OceanAction() override = default;

	/**
	 * @copydoc EntityMapping::Actions::Action::activate()
	 */
	void activate(EntityMapping::ChangeContext& context) override;

	/**
	 * @copydoc EntityMapping::Actions::Action::deactivate()
	 */
	void deactivate(EntityMapping::ChangeContext& context) override;

private:

	std::function<void(bool)> mAttachmentFunction;
};
}
}

}

#endif /* OCEANACTION_H_ */
