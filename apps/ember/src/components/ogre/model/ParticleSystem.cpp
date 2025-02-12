//
// C++ Implementation: ParticleSystem
//
// Description:
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2005
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
#include <cassert>

#include "ParticleSystem.h"
#include "ParticleSystemBinding.h"
#include "ModelDefinition.h"
#include <OgreParticleSystem.h>
#include <OgreParticleEmitter.h>
#include <OgreNode.h>


namespace Ember::OgreView::Model {

Ogre::ParticleSystem* ParticleSystem::getOgreParticleSystem() {
	return mOgreParticleSystem;
}

ParticleSystem::ParticleSystem(Ogre::ParticleSystem* ogreParticleSystem, const Ogre::Vector3& direction) :
		mOgreParticleSystem(ogreParticleSystem), mDirection(direction) {
	assert(ogreParticleSystem);
	//If there's a direction, make sure that we adjust all emitters for each frame so that they are in world space.
	if (!mDirection.isNaN()) {
		ogreParticleSystem->setListener(this);
	}
}

//The Ogre ParticleSystem isn't owned by this instance and won't be destroyed here.
ParticleSystem::~ParticleSystem() = default;


ParticleSystemBinding ParticleSystem::addBinding(ModelDefinition::ParticleSystemSetting emitterVal, const std::string& variableName) {
	if (emitterVal == ModelDefinition::ParticleSystemSetting::EMISSION_RATE) {
		Ogre::ParticleEmitter* emitter = mOgreParticleSystem->getEmitter(0);
		if (emitter) {
			return {emitterVal, variableName, this, emitter->getEmissionRate()};
		}
	} else if (emitterVal == ModelDefinition::ParticleSystemSetting::TIME_TO_LIVE) {
		Ogre::ParticleEmitter* emitter = mOgreParticleSystem->getEmitter(0);
		if (emitter) {
			return {emitterVal, variableName, this, emitter->getTimeToLive()};
		}
	}
	return {};
}

void ParticleSystem::setVisible(bool visibility) {
	if (mOgreParticleSystem) {
		mOgreParticleSystem->setVisible(visibility);
	}
}

const Ogre::Vector3& ParticleSystem::getDirection() const {
	return mDirection;
}

bool ParticleSystem::objectRendering(const Ogre::MovableObject* movable, const Ogre::Camera*) {
	//Adjust all emitters so that mDirection is applied in world space
	Ogre::Quaternion rotation = movable->getParentNode()->convertWorldToLocalOrientation(Ogre::Quaternion(Ogre::Degree(0), mDirection));
	for (unsigned short i = 0; i < mOgreParticleSystem->getNumEmitters(); ++i) {
		Ogre::ParticleEmitter* emitter = mOgreParticleSystem->getEmitter(i);
		emitter->setDirection(rotation * mDirection);
	}
	return true;
}


}


