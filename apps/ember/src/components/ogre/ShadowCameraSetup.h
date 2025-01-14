//
// C++ Interface: ShadowCameraSetup
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
#ifndef EMBEROGRESHADOWCAMERASETUP_H
#define EMBEROGRESHADOWCAMERASETUP_H

#include "services/config/ConfigListenerContainer.h"
#include "OgreIncludes.h"
#include <OgreSharedPtr.h>


namespace Ember::OgreView {

class GraphicalChangeAdapter;

class ShadowDetailManager;

/**
 *
 * @brief This takes care of the setup of the pssm shadow camera.
 *
 * Apart from setting up the camera it also listens for changes to the varconf settings dealing with the shadows and update the shadow settings accordingly.
 *
 * @author Erik Ogenvik <erik@ogenvik.org>
 */
class ShadowCameraSetup : ConfigListenerContainer {
public:
	/**
	 * @brief Ctor.
	 * @param sceneMgr The scene manager to which we want to apply our shadow camera.
	 */
	ShadowCameraSetup(Ogre::SceneManager& sceneMgr, GraphicalChangeAdapter& graphicalChangeAdapter);

	/**
	 * @brief Dtor.
	 * This will actually not reset the shadows.
	 */
	~ShadowCameraSetup() override;

protected:

	/**
	 * @brief The scene manager which we want to apply our shadow camera to.
	 */
	Ogre::SceneManager& mSceneMgr;

	/**
	 * @brief The pssm shadow camera setup.
	 */
	Ogre::PSSMShadowCameraSetup* mPssmSetup;

	/**
	 * @brief A reference to the shared pointer, which guarantees that the mPssmSetup field isn't deleted inside Ogre.
	 */
	Ogre::ShadowCameraSetupPtr mSharedCameraPtr;

	/**
	 * @brief Object that handles shadow detail automatically.
	 */
	std::unique_ptr<ShadowDetailManager> mShadowDetailManager;

private:

	/**
	 * @brief Sets the shadow texture size.
	 * @param section
	 * @param key
	 * @param variable
	 */
	void Config_ShadowTextureSize(const std::string& section, const std::string& key, varconf::Variable& variable);

	/**
	 * @brief Sets the shadow split points (as a space separated series of floats, for example "1 15 50 500").
	 * @param section
	 * @param key
	 * @param variable
	 */
	void Config_ShadowSplitPoints(const std::string& section, const std::string& key, varconf::Variable& variable);

	/**
	 * @brief Sets the shadow split padding, a float.
	 * @param section
	 * @param key
	 * @param variable
	 */
	void Config_ShadowSplitPadding(const std::string& section, const std::string& key, varconf::Variable& variable);

	/**
	 * @brief Sets the shadow optimal adjust factors (as a space separated series of floats, for example "1 1 1").
	 * @param section
	 * @param key
	 * @param variable
	 */
	void Config_ShadowOptimalAdjustFactors(const std::string& section, const std::string& key, varconf::Variable& variable);

	/**
	 * @brief Sets the shadow use aggressive focus region, as a boolean.
	 * @param section
	 * @param key
	 * @param variable
	 */
	void Config_ShadowUseAggressiveFocusRegion(const std::string& section, const std::string& key, varconf::Variable& variable);

	/**
	 * @brief Sets the shadow far distance, as a float.
	 * @param section
	 * @param key
	 * @param variable
	 */
	void Config_ShadowFarDistance(const std::string& section, const std::string& key, varconf::Variable& variable);

	/**
	 * @brief Sets whether back faces should be rendered, as a boolean.
	 * @param section
	 * @param key
	 * @param variable
	 */
	void Config_ShadowRenderBackfaces(const std::string& section, const std::string& key, varconf::Variable& variable);

};

}



#endif
