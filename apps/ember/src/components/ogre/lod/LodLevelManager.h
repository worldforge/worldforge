/*
 * Copyright (C) 2012 Arjun Kumar <arjun1991@gmail.com>
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
#ifndef LODLEVELMANAGER_H
#define LODLEVELMANAGER_H

#include <sigc++/connection.h>
#include <string>
#include "components/ogre/OgreIncludes.h"

namespace varconf {
class Variable;
}

namespace Ember {

class ConfigListenerContainer;
namespace OgreView {

class ShaderManager;

class GraphicalChangeAdapter;

namespace Lod {
/**
 * @brief Handles level of detail changes for the main camera.
 * Acts as a sub-component of the automatic handling of graphics system.
 */
class LodLevelManager {
public:
	/**
	 * @brief Constructor.
	 */
	LodLevelManager(GraphicalChangeAdapter& graphicalChangeAdapter, Ogre::Camera& mainCamera);

	/**
	 * @brief Destructor.
	 */
	~LodLevelManager();

	/**
	 * @brief Sets the lod bias factor.
	 * 
	 * Using this function it is possible to influence the lod behavior of all materials and meshes. It works by adjusting the 
	 * lod bias of the main camera.
	 * 
	 * @param factor Proportional factor to apply to the distance at which LOD is changed. Higher values increase the distance at which higher LODs are used (eg. 2.0 is twice the normal distance, 0.5 is half).
	 * 
	 * @note factor cannot be negative or zero. It is still safe to attempt to set these values as the function will default to a factor of 0.1
	 */
	bool setLodBiasAll(Ogre::Real factor);

	/**
	 * @brief This can be used to stop this component responding to change requests.
	 */
	void pause();

	/**
	 * @brief This can be used to restart this component responding to change requests.
	 */
	void unpause();

protected:
	/**
	 * The threshold fps change after which the lod level is changed to respond to performance change requested.
	 */
	float mLodThresholdLevel;

	/**
	 * The minimum material lod bias that the lod bias factor can be set to.
	 */
	Ogre::Real mMinLodFactor;

	/**
	 * The maximum material lod bias that the lod bias factor can be set to.
	 */
	Ogre::Real mMaxLodFactor;

	/**
	 * The default step that is taken when adjusting the factors of this component.
	 */
	float mDefaultStep;

	/**
	 * Holds the reference to the connection to the changeRequired signal. Used to disconnect the signal on destruction of this class or to pause the functioning of this component.
	 */
	sigc::connection mChangeRequiredConnection;

	/**
	 * Reference to graphical adapter through which this manager receives graphics detail change requests.
	 */
	GraphicalChangeAdapter& mGraphicalChangeAdapter;

	/**
	 * Reference to the main camera object
	 */
	Ogre::Camera& mMainCamera;

	/**
	 * @brief Used to listen for configuration changes.
	 */
	std::unique_ptr<ConfigListenerContainer> mConfigListenerContainer;

	/**
	 * @brief Checks level against this component's threshold value to determine change in material lod.
	 * This function is used to listen to the changes required by the automatic graphics detail system.
	 */
	bool changeLevel(float level);

	/**
	 * @brief Steps up the material lod bias by provided step.
	 * @param step The value to step the bias up by.
	 */
	bool stepUpLodBias(float step);

	/**
	 * @brief Steps down the material lod bias by provided step.
	 * @param step The value to step the bias down by.
	 */
	bool stepDownLodBias(float step);

	/**
	 * @brief Connected to the config service to listen for lod bias settings.
	 */
	void Config_LodBias(const std::string& section, const std::string& key, varconf::Variable& variable);

};

}
}
}
#endif // ifndef LODLEVELMANAGER_H

