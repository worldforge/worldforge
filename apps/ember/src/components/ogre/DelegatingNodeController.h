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

#ifndef DELEGATINGNodeController_H_
#define DELEGATINGNodeController_H_

#include "components/ogre/NodeController.h"

namespace Ember {
struct IEntityControlDelegate;
namespace OgreView {


/**
 * @author Erik Ogenvik <erik@ogenvik.org>
 * @brief A node controller which uses a delegate to perform the actual controlling.
 */
class DelegatingNodeController : public NodeController {
public:
	/**
	 * @brief Ctor.
	 * @param attachment The attachment which will be controlled.
	 * @param attachmentControlDelegate The delegate which will perform the actuall controlling.
	 */
	DelegatingNodeController(NodeAttachment& attachment, IEntityControlDelegate& attachmentControlDelegate);

	/**
	 * @brief Dtor.
	 */
	~DelegatingNodeController() override;

	/**
	 * @brief Gets the delegate.
	 * @return The delegate which performs the actual controlling.
	 */
	IEntityControlDelegate* getControlDelegate() const override;

private:
	/**
	 * @brief The delegate which performs the controlling.
	 */
	IEntityControlDelegate& mAttachmentControlDelegate;

	void updatePosition() override;

};

}

}

#endif /* DELEGATINGNodeController_H_ */
