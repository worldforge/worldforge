//
// C++ Interface: OgreEntityRenderer
//
// Description:
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2006
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.//
//
#ifndef EMBEROGREOGREENTITYRENDERER_H
#define EMBEROGREOGREENTITYRENDERER_H

#include "MovableObjectRenderer.h"


namespace Ember::OgreView {
class SkeletonDisplay;
namespace Gui {

/**
 * Renders a single Ogre::Entity to a EntityCEGUITexture.
 *
 * @author Erik Ogenvik
 */
class OgreEntityRenderer :
		public MovableObjectRenderer {
public:
	explicit OgreEntityRenderer(CEGUI::Window* image);

	~OgreEntityRenderer() override;

	/**
	 * Renders the submitted Entity.
	 * @param modelName a mesh namel
	 */
	void showEntity(const std::string& mesh);

	/**
	 * @brief Unloads the Entity.
	 *
	 * The getEntity() will return nullptr after this call.
	 * You can call showEntity() after this call to load an entity again.
	 */
	void unloadEntity();

	/**
	 * @brief Returns the scene manager used by OgreEntityRenderer
	 */
	Ogre::SceneManager* getSceneManager();

	/**
	 * Returns the current rendered Entity, or null if none is set.
	 * @return
	 */
	Ogre::Entity* getEntity();

	bool getWireframeMode();

	void setWireframeMode(bool enabled);

	void setForcedLodLevel(int lodLevel);

	void clearForcedLodLevel();

	/**
	 * @brief Enables the supplied animation, stopping any currently enabled animation.
	 * @param animationName The name of the animation.
	 */
	void enableAnimation(const std::string& animationName);


	/**
	 * @brief Gets the names of all the animations in the current entity.
	 * @return A vector containing animation names for the current entity.
	 */
	std::vector<std::string> getEntityAnimationNames() const;

	bool frameStarted(const Ogre::FrameEvent& event) override;

	/**
	 * Toggles display of the skeleton.
	 * @param showSkeleton
	 */
	void setShowSkeleton(bool showSkeleton);

protected:

	Ogre::Entity* mEntity;

	/**
	 * The current active animation, if not null.
	 */
	Ogre::AnimationState* mActiveAnimation;

	/**
	 * Listens to the mesh being unloaded and stops the animation.
	 */
	std::unique_ptr<Ogre::Resource::Listener> mMeshListener;

	/**
	 * Listens to the skeleton being unloaded and stops the animation.
	 */
	std::unique_ptr<Ogre::Resource::Listener> mSkeletonListener;

	bool mShowSkeleton;

	std::unique_ptr<SkeletonDisplay> mSkeletonDisplay;

	float getMovableBoundingRadius() override;

	void setEntity(Ogre::Entity* entity);


};
}
}


#endif // ifndef EMBEROGREOGREENTITYRENDERER_H
