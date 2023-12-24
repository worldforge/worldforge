//
// C++ Interface: ModelBackgroundLoader
//
// Description:
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2009
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
#ifndef EMBEROGRE_MODELMODELBACKGROUNDLOADER_H
#define EMBEROGRE_MODELMODELBACKGROUNDLOADER_H

#include "components/ogre/EmberOgrePrerequisites.h"
#include "ModelDefinition.h"

#include <Eris/ActiveMarker.h>

#include <OgreResourceBackgroundQueue.h>

#include <memory>

namespace Eris {
class EventService;
}

namespace Ember {
class TimeFrame;


namespace OgreView::Model {

class Model;

class ModelBackgroundLoader;

class ModelDefinition;

/**
 @brief Responsible for loading the resources needed by a Model.
 If thread support is enabled it will be used. You must then call poll() each frame to see if the background thread has finished loading.

 @author Erik Ogenvik <erik@ogenvik.org>
 */
class ModelBackgroundLoader {
	friend class ModelBackgroundLoaderListener;

public:
	/**
	 * @brief The different loading states of the Model.
	 * Loading normally progresses through these states in order.
	 */
	enum LoadingState {
		/**
		 * @brief The loading hasn't yet begun.
		 */
		LS_UNINITIALIZED,
		/**
		 * @brief The Meshes are being prepared.
		 */
		LS_MESH_PREPARING,
		/**
		 * @brief The Meshes have been prepared.
		 */
		LS_MESH_PREPARED,
		/**
		 * @brief The Meshes are loading.
		 */
		LS_MESH_LOADING,
		/**
		 * @brief The Meshes have been loaded.
		 */
		LS_MESH_LOADED,
		/**
		 * @brief Particle systems are being prepared.
		 */
		LS_PARTICLE_SYSTEM_PREPARING,
		/**
		 * @brief The Materials are being prepared.
		 */
		LS_MATERIAL_PREPARING,
		/**
		 * @brief The Materials have been prepared.
		 */
		LS_MATERIAL_PREPARED,
		/**
		 * @brief Textures are being prepared.
		 */
		LS_TEXTURE_PREPARING,
		/**
		 * @brief Textures have been prepared.
		 */
		LS_TEXTURE_PREPARED,
		/**
		 * @brief The Materials are loading.
		 */
		LS_MATERIAL_LOADING,
		/**
		 * @brief Loading is done.
		 */
		LS_DONE
	};

	/**
	 * @brief Ctor.
	 * @param model The model which will be loaded.
	 */
	explicit ModelBackgroundLoader(ModelDefinition& modelDefinition);

	/**
	 * @brief Dtor.
	 */
	virtual ~ModelBackgroundLoader();

	/**
	 * @brief Starts the polling loop.
	 *
	 * If the model isn't yet loaded, we're in either of two states.
	 * We're either waiting for background loading tickets to complete. In this case nothing will be done. @see operationCompleted
	 * Or we don't have any background tickets currently, but bailed out early in the loading process in order to allow for interleaving with the event loop.
	 * In the latter case a new call to "poll" will be posted to the event loop.
	 *
	 * The result of this is that this method only should be called externally once.
	 *
	 * @return True if the loading is complete.
	 */
	bool poll();

	/**
	 * @brief Gets the current loading state.
	 * @return The current loading state.
	 */
	LoadingState getState() const;


protected:

	/**
	 * @brief The model definition which will be loaded.
	 */
	ModelDefinition& mModelDefinition;


	int mResourcesBeingLoadingInBackground;

	/**
	 * @brief The current loading state of the instance.
	 */
	LoadingState mState;

	/**
	 * @brief Keeps track of the currently processed submodel.
	 */
	size_t mSubModelLoadingIndex;

	/**
	 * @brief A set of textures that needs to be loaded.
	 */
	std::set<std::string> mTexturesToLoad;

	/**
	 * @brief A set of materials that needs to be loaded.
	 */
	std::set<Ogre::MaterialPtr> mMaterialsToLoad;

	Eris::ActiveMarker mIsActive;


	/**
	 * @brief Checks to see if there are any tickets left.
	 * @return True if there aren't any tickets left.
	 */
	bool areAllTicketsProcessed() const;

	/**
	 * @brief Polls the loading state (which might occur in a background thread).
	 * If the loading state has progressed it will be updated.
	 * @return True if the loading is complete.
	 */
	bool performLoading();

	void prepareMaterialInBackground(const std::string& materialName);

	void prepareMeshInBackground(const std::string& meshName);

	void prepareTextureInBackground(const std::string& textureName);

};

}


}

#endif
