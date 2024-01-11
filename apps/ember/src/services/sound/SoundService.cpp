/*
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
#include "SoundGeneral.h"
#include "SoundSample.h"

#include "services/config/ConfigService.h"
#include "framework/Log.h"

#include "al.h"
#include "alc.h"

#include <thread>

using namespace std::literals::chrono_literals;

namespace Ember {
SoundService::SoundService(ConfigService& configService)
		: Service("Sound"),
		  mContext(nullptr),
		  mDevice(nullptr),
		  mEnabled(false) {
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
		std::vector<std::shared_ptr<SoundEntry>> soundEntries;

		while (!stopToken.stop_requested()) {
			//Start collecting any new sound entries and adding them to the active list.
			std::unique_lock l(mSoundEntriesMutex);
			soundEntries.insert(soundEntries.end(), mNewSoundEntries.begin(), mNewSoundEntries.end());
			mNewSoundEntries.clear();
			l.unlock();


			for (auto I = soundEntries.begin(); I != soundEntries.end();) {
				auto soundEntry = *I;
				auto& soundGroup = soundEntry->soundGroup;
				auto alSource = soundEntry->source->getALSource();

				std::array<ALuint, 2> buffers{}; //At most two buffers.
				size_t numberOfBuffersToQueue = 0;
				//Check if we've completed playing and if so remove the entry.
				ALint alNewState;
				alGetSourcei(alSource, AL_SOURCE_STATE, &alNewState);
				SoundGeneral::checkAlError("Checking source state.");
				if (alNewState == AL_INITIAL) {
					numberOfBuffersToQueue = soundEntry->buffers.size();
					buffers = soundEntry->buffers;
				} else {
					ALsizei processed = 0;
					alGetSourcei(alSource, AL_BUFFERS_PROCESSED, &processed);
					SoundGeneral::checkAlError("Checking buffers processed.");
					if (processed > 0) {
						alSourceUnqueueBuffers(alSource, processed, buffers.data());
						numberOfBuffersToQueue = (size_t) processed;
					}
				}

				if (alNewState != AL_STOPPED) {
					//Check if we need to cycle buffers
					for (size_t i = 0; i < numberOfBuffersToQueue; ++i) {
						if (soundEntry->currentlyPlaying < soundGroup.sounds.size()) {
							auto result = soundGroup.sounds[soundEntry->currentlyPlaying].soundSample->fillBuffer(buffers[i]);
							if (result != SoundSample::BufferFillStatus::HAD_ERROR) {
								alSourceQueueBuffers(alSource, 1, &buffers[i]);
								SoundGeneral::checkAlError("Queuing buffers.");
								if (result == SoundSample::BufferFillStatus::NO_MORE_DATA) {
									soundEntry->currentlyPlaying++;
									if (soundEntry->currentlyPlaying > soundGroup.sounds.size() && soundGroup.repeating) {
										soundEntry->currentlyPlaying = 0;
									}
									if (soundEntry->currentlyPlaying < soundGroup.sounds.size()) {
										soundEntry->soundGroup.sounds[soundEntry->currentlyPlaying].soundSample->reset();
									}
								}
							}
						}
					}
				}

				if (alNewState == AL_INITIAL) {
					alSourcePlay(alSource);
					SoundGeneral::checkAlError("Playing sound instance.");
				}
				if (alNewState == AL_STOPPED) {
					I = soundEntries.erase(I);
				} else {
					I++;
				}

			}


			std::this_thread::sleep_for(1ms);
		}
	}
	};

}

SoundService::~SoundService() {
	mSoundProcessingThread.request_stop();
	mSoundProcessingThread.join();
	if (isEnabled()) {
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(mContext);
		alcCloseDevice(mDevice);
	}
}


bool SoundService::isEnabled() const {
	return mEnabled;
}

void SoundService::updateListenerPosition(const WFMath::Point<3>& pos, const WFMath::Vector<3>& direction, const WFMath::Vector<3>& up) const {
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

std::shared_ptr<SoundService::SoundControl> SoundService::playSound(SoundService::SoundGroup soundGroup) {
	std::array<ALuint, 2> buffers{};
	alGenBuffers(2, buffers.data());

	auto soundEntry = std::make_shared<SoundEntry>(soundGroup, std::make_unique<SoundSource>(), 0, buffers);

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
		mNewSoundEntries.emplace_back(soundEntry);
	}
	auto weak = std::weak_ptr(soundEntry);

	auto control = std::make_shared<SoundService::SoundControl>([weak]() {
		auto shared = weak.lock();
		if (shared) {
			//Just stop it; the background thread will take care of cleaning up.
			alSourceStop(shared->source->getALSource());
			SoundGeneral::checkAlError("Stopping sound instance.");
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

SoundService::SoundEntry::~SoundEntry() {
	alDeleteBuffers(2, buffers.data());
	SoundGeneral::checkAlError("Deleting sound buffers.");
}

} // namespace Ember

