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

#include "SoundService.h"

#include "services/config/ConfigService.h"
#include "framework/Log.h"

#include "SoundSample.h"
#include "al.h"
#include "alc.h"

#include <algorithm>
#include <thread>
#include <future>

using namespace std::literals::chrono_literals;

namespace Ember {
SoundService::SoundService(ConfigService& configService)
		: Service("Sound"), mContext(nullptr), mDevice(nullptr), mResourceProvider(nullptr), mEnabled(false) {
	logger->info("Sound Service starting");

	if (configService.hasItem("audio", "enabled")
		&& !static_cast<bool>(configService.getValue("audio", "enabled"))) {
		logger->info("Sound disabled.");
	} else {

		if (isEnabled()) {
			logger->error("Can't start the sound system if it's already started.");
		} else {

			mDevice = alcOpenDevice(nullptr);

			if (!mDevice) {
				mEnabled = false;
				logger->error("Sound Service failed to start, sound device not found");
			} else {

				mContext = alcCreateContext(mDevice, nullptr);
				if (!mContext) {
					mEnabled = false;
					logger->error("Sound Service failed to start, sound device not found");
				} else {
					mEnabled = alcMakeContextCurrent(mContext) == ALC_TRUE;
				}
			}


			SoundGeneral::checkAlError();
		}
	}

	//Create a background thread which loops through any active sound entries and updates them.
	mSoundProcessingThread = std::jthread{[this](std::stop_token stopToken) {
		while (!stopToken.stop_requested()) {
			std::unique_lock l(mSoundEntriesMutex);
			auto soundEntriesCopy = mSoundEntries;
			l.unlock();

			std::vector<std::shared_ptr<SoundEntry>> completedEntries;

			for (auto& soundEntry: soundEntriesCopy) {
				auto& soundGroup = soundEntry->soundGroup;
				auto alSource = soundEntry->source->getALSource();
				if (soundEntry->currentlyPlaying == -1) {
					//Need to set up everything
					if (soundGroup.sounds.size() == 1) {
						//Only need to handle one buffer
						auto& firstSound = soundGroup.sounds.front();
						auto buffer = firstSound.soundSample->getBuffers()[0];
						alSourcei(alSource, AL_BUFFER, (ALint) buffer);
						SoundGeneral::checkAlError("Binding sound source to static sound buffer.");
						soundEntry->currentlyPlaying = 0;
					} else {
						//Need to queue buffers. Start with the first two.
						std::array<ALuint, 2> buffers{soundGroup.sounds[0].soundSample->getBuffers()[0], soundGroup.sounds[1].soundSample->getBuffers()[0]};
						alSourceQueueBuffers(alSource, 2, buffers.data());
						SoundGeneral::checkAlError("Queuing buffers.");
					}
					soundEntry->currentlyPlaying = 0;
					alSourcePlay(alSource);
					SoundGeneral::checkAlError("Playing sound instance.");
				} else {
					//Check status.
					if (!soundGroup.repeating) {
						//Check if we've completed playing and if so remove the entry.
						ALint alNewState;
						alGetSourcei(alSource, AL_SOURCE_STATE, &alNewState);
						SoundGeneral::checkAlError("Checking source state.");
						if (alNewState == AL_STOPPED) {
							completedEntries.emplace_back(soundEntry);
						} else {
							if (soundGroup.sounds.size() > 1) {
								//Check if we need to cycle buffers
								ALint processed;
								alGetSourcei(alSource, AL_BUFFERS_PROCESSED, &processed);
								SoundGeneral::checkAlError("Checking buffers processed.");
								if (processed > 0) {
									ALuint buffer;
									alSourceUnqueueBuffers(alSource, processed, &buffer);
									soundEntry->currentlyPlaying += processed;
									//We now need to queue new buffers. This is done differently depending on whether we're repeating or not.
									if (soundGroup.sounds.size() > (size_t) soundEntry->currentlyPlaying) {
										std::array<ALuint, 1> buffers{soundGroup.sounds[soundEntry->currentlyPlaying + 1].soundSample->getBuffers()[0]};
										alSourceQueueBuffers(alSource, 1, buffers.data());
									} else {
										if (soundGroup.repeating) {
											//Go back to the first sound
											std::array<ALuint, 1> buffers{soundGroup.sounds[0].soundSample->getBuffers()[0]};
											alSourceQueueBuffers(alSource, 1, buffers.data());
										}
									}
								}
							}
						}
					}
				}
			}

			if (!completedEntries.empty()) {
				{
					l.lock();
					for (auto& completedEntry: completedEntries) {
						auto I = std::find(mSoundEntries.begin(), mSoundEntries.end(), completedEntry);
						if (I != mSoundEntries.end()) {
							mSoundEntries.erase(I);
						}
					}
				}
			}

			std::this_thread::sleep_for(1ms);
		}
	}
	};

}

SoundService::~SoundService() {

	mBaseSamples.clear();

	if (isEnabled()) {
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(mContext);
		alcCloseDevice(mDevice);
	}
}


bool SoundService::isEnabled() const {
	return mEnabled;
}

void SoundService::updateListenerPosition(const WFMath::Point<3>& pos, const WFMath::Vector<3>& direction, const WFMath::Vector<3>& up) {
	if (!isEnabled()) {
		return;
	}

	alListener3f(AL_POSITION, (ALfloat) pos.x(), (ALfloat) pos.y(), (ALfloat) pos.z());
	SoundGeneral::checkAlError("Setting the listener position.");

	//Set the direction of the listener.

	std::array<ALfloat, 6> aluVectors{(ALfloat) direction.x(), (ALfloat) direction.y(), (ALfloat) direction.z(), (ALfloat) up.x(), (ALfloat) up.y(), (ALfloat) up.z()};

	alListenerfv(AL_ORIENTATION, aluVectors.data());
	SoundGeneral::checkAlError("Setting the listener orientation.");
}

void SoundService::cycle() {

}

BaseSoundSample* SoundService::createOrRetrieveSoundSample(const std::string& soundPath) {
	auto I = mBaseSamples.find(soundPath);
	if (I != mBaseSamples.end()) {
		return I->second.get();
	}
	if (mResourceProvider) {
		ResourceWrapper resWrapper = mResourceProvider->getResource(soundPath);
		if (resWrapper.hasData()) {
			auto sample = StaticSoundSample::create(resWrapper);
			auto result = mBaseSamples.emplace(soundPath, std::move(sample));
			if (result.second) {
				return result.first->second.get();
			}
		}
	}
	return nullptr;
}


IResourceProvider* SoundService::getResourceProvider() {
	return mResourceProvider;
}

void SoundService::setResourceProvider(IResourceProvider* resourceProvider) {
	mResourceProvider = resourceProvider;
}

std::shared_ptr<SoundService::SoundControl> SoundService::playSound(SoundService::SoundGroup soundGroup) {
	auto soundEntry = std::make_shared<SoundEntry>(soundGroup, std::make_unique<SoundSource>());
	if (soundGroup.gain) {
		soundEntry->source->setGain(*soundGroup.gain);
	}
	if (soundGroup.reference) {
		soundEntry->source->setReference(*soundGroup.reference);
	}
	if (soundGroup.rolloff) {
		soundEntry->source->setRolloff(*soundGroup.rolloff);
	}

	if (soundGroup.repeating) {
		alSourcei(soundEntry->source->getALSource(), AL_LOOPING, true);
		SoundGeneral::checkAlError("Setting sound source looping.");
	}


	{
		std::unique_lock l(mSoundEntriesMutex);
		mSoundEntries.emplace_back(soundEntry);
	}
	auto weak = std::weak_ptr(soundEntry);

	auto control = std::make_shared<SoundService::SoundControl>([this, weak]() {
		auto shared = weak.lock();
		if (shared) {
			std::unique_lock l(mSoundEntriesMutex);
			auto I = std::find(mSoundEntries.begin(), mSoundEntries.end(), shared);
			if (I != mSoundEntries.end()) {
				mSoundEntries.erase(I);
			}
		}
	}, [weak](WFMath::Point<3> pos) {
		auto shared = weak.lock();
		if (shared) {
			shared->source->setPosition(pos);
		}

	}, [weak](WFMath::Vector<3> velocity) {
		auto shared = weak.lock();
		if (shared) {
			shared->source->setVelocity(velocity);
		}

	});

	return control;
}

} // namespace Ember

