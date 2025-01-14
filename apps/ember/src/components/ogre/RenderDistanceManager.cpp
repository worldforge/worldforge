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

#include "RenderDistanceManager.h"

#include "components/ogre/environment/IEnvironmentProvider.h"
#include <services/config/ConfigListenerContainer.h>

#include "OgreCamera.h"


namespace Ember::OgreView {

RenderDistanceManager::RenderDistanceManager(GraphicalChangeAdapter& graphicalChangeAdapter, Environment::IFog& fog, Ogre::Camera& mainCamera) :
		mDefaultFarRenderDistance(1000),
		mFarRenderDistance(1000),
		mFarRenderDistanceFactor(1.0f),
		mMaxFarRenderDistanceFactor(1.5f),
		mMinFarRenderDistanceFactor(0.7f),
		mRenderDistanceThreshold(5.0f),
		mDefaultRenderDistanceStep(0.3f),
		mFog(fog),
		mGraphicalChangeAdapter(graphicalChangeAdapter),
		mMainCamera(mainCamera),
		mConfigListenerContainer(std::make_unique<ConfigListenerContainer>()) {
//	if (!mChangeRequiredConnection) {
//		mChangeRequiredConnection = mGraphicalChangeAdapter.EventChangeRequired.connect(sigc::mem_fun(*this, &RenderDistanceManager::changeLevel));
//	}
//	mConfigListenerContainer->registerConfigListener("graphics", "renderdistance", sigc::mem_fun(*this, &RenderDistanceManager::Config_FarRenderDistance));
}

RenderDistanceManager::~RenderDistanceManager() {
	if (mChangeRequiredConnection) {
		mChangeRequiredConnection.disconnect();
	}
}

void RenderDistanceManager::setFarRenderDistance(float factor) {
	if (factor < 0.0f) {
		factor = 0.0f;
	}
	mFarRenderDistance = factor * mDefaultFarRenderDistance;
	mMainCamera.setFarClipDistance(mFarRenderDistance);
}

void RenderDistanceManager::setCompensatedFarRenderDistance(float factor) {
	mFarRenderDistance = factor * mDefaultFarRenderDistance;
	mMainCamera.setFarClipDistance(mFarRenderDistance);
	float fogDensity = mMaxFarRenderDistanceFactor - factor;
	if (factor > 0.3f) {
		fogDensity = 0.3f;
	}
	mFog.setDensity(fogDensity);
}

bool RenderDistanceManager::changeLevel(float level) {
	//Disable for now. The render distance should be decided by the server.
	//What we can do is alter detail levels; but it's not clear if this should be done from here.
	//For now we'll keep the class, but keep it inactive.

	return false;

//	// If the fps change required is less than any threshold, the changeMade boolean will remain false, else it indicates if a step up or down was made.
//	bool changeMade = false;
//
//	if (std::abs(level) >= mRenderDistanceThreshold) {
//		if (level > 0.0f) {
//			changeMade |= stepDownFarRenderDistance(mDefaultRenderDistanceStep);
//		} else {
//			changeMade |= stepUpFarRenderDistance(mDefaultRenderDistanceStep);
//		}
//	}
//	return changeMade;
}

bool RenderDistanceManager::stepDownFarRenderDistance(float step) {
	bool stepPossible = false;
	if (mFarRenderDistanceFactor > step) { //step down only if existing render distance factor is greater than step
		mFarRenderDistanceFactor -= step;
		setFarRenderDistance(mFarRenderDistanceFactor);
		stepPossible = true;
	} else if (mFarRenderDistanceFactor < step &&
			   mFarRenderDistanceFactor > mMinFarRenderDistanceFactor) { //if there is still some positive distance left which is smaller than step, set it to the minimum distance
		mFarRenderDistanceFactor = mMinFarRenderDistanceFactor;
		setFarRenderDistance(mFarRenderDistanceFactor);
		stepPossible = true;
	} else { //step down not possible
		stepPossible = false;
	}
	return stepPossible;
}

bool RenderDistanceManager::stepUpFarRenderDistance(float step) {
	bool stepPossible = false;
	if (mFarRenderDistanceFactor + step <= mMaxFarRenderDistanceFactor) { //step up only if the step doesn't cause render far distance to go over maximum render far distance.
		mFarRenderDistanceFactor += step;
		setFarRenderDistance(mFarRenderDistanceFactor);
		stepPossible = true;
	} else if (mFarRenderDistanceFactor < mMaxFarRenderDistanceFactor) { //if the render far distance is still below maximum render far distance but a default step causes it to go over it.
		mFarRenderDistanceFactor = mMaxFarRenderDistanceFactor;
		setFarRenderDistance(mFarRenderDistanceFactor);
		stepPossible = true;
	} else {
		stepPossible = false; //step up not possible
	}
	return stepPossible;
}

void RenderDistanceManager::Config_FarRenderDistance(const std::string& section, const std::string& key, varconf::Variable& variable) {
	if (variable.is_double()) {
		auto renderDistance = static_cast<double>(variable);
		setFarRenderDistance((float) renderDistance / 100.0f);
	}
}

void RenderDistanceManager::pause() {
	mChangeRequiredConnection.block();
}

void RenderDistanceManager::unpause() {
	mChangeRequiredConnection.unblock();
}

}

