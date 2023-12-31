/*
    Copyright (C) 2008 Romulo Fernandes Machado (nightz)
    Copyright (C) 2008 Erik Ogenvik <erik@ogenvik.org>

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
#include "SoundAction.h"
#include "SoundEntity.h"
#include "SoundGroup.h"
#include "SoundDefinition.h"
#include "SoundDefinitionManager.h"

#include "services/EmberServices.h"

#include "services/sound/SoundService.h"
#include "services/sound/SoundSource.h"

using namespace Ember;


namespace Ember::OgreView {
SoundAction::SoundAction(SoundEntity& soundEntity)
		: mSoundEntity(soundEntity),
		  mInstance(nullptr),
		  mIsLooping(false) {
}

SoundAction::~SoundAction() {
	if (mInstance) {
		SoundService::getSingleton().destroyInstance(mInstance);
	}
}

SoundGroup* SoundAction::getGroup() {
	return mGroup.get();
}

SoundGroup* SoundAction::setGroup(const std::string& name) {
	if (mGroup) {
		return nullptr;
	}

	auto groupModel = SoundDefinitionManager::getSingleton().getSoundGroupDefinition(name);

	if (!groupModel) {
		logger->error("A template to the group {} could not be found.", name);
		return nullptr;
	}

	mGroup = std::make_unique<SoundGroup>();

	auto& soundDefinitions = groupModel->mSamples;
	for (const auto& soundDefinition: soundDefinitions) {
		// Register Individual samples
		mGroup->addSound(soundDefinition);
	}

	return mGroup.get();
}

void SoundAction::play() {
	if (mGroup) {
		if (!mInstance) {
			mInstance = SoundService::getSingleton().createInstance();
			if (!mInstance) {
				//The sound system seems to be disabled (could be for a valid reason) so just return without any fuss.
				return;
			}
			mInstance->setMotionProvider(this);
			mInstance->setIsLooping(mIsLooping);
			//If the sound is set not to loop, we need to listen for when it's done playing and remove the instance once it's done (to save on sound resources).
			if (!mIsLooping) {
				mInstance->EventPlayComplete.connect(sigc::mem_fun(*this, &SoundAction::SoundInstance_PlayComplete));
			}

			mGroup->bindToInstance(mInstance);
		}
		mInstance->play();
	}
}

void SoundAction::stop() {
	if (mInstance) {
		SoundService::getSingleton().destroyInstance(mInstance);
		mInstance = nullptr;
	}
}


void SoundAction::SoundInstance_PlayComplete() {
	if (mInstance) {
		if (SoundService::getSingleton().destroyInstance(mInstance)) {
			mInstance = nullptr;
		}
	} else {
		logger->warn("Got a play complete signal while there's no sound instance registered. For some reason the sound instance must have already been removed.");
	}
}

SoundInstance* SoundAction::getInstance() const {
	return mInstance;
}

void SoundAction::update(SoundSource& soundSource) {
	auto pos = mSoundEntity.getPosition();
	if (pos.isValid()) {
		soundSource.setPosition(pos);
	}
	auto velocity = mSoundEntity.getVelocity();
	if (velocity.isValid()) {
		soundSource.setVelocity(velocity);
	}
}

void SoundAction::setIsLooping(bool isLooping) {
	mIsLooping = isLooping;
}

}


