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

#ifndef SOUNDSERVICE_H
#define SOUNDSERVICE_H

#include "framework/Service.h"
#include "framework/ConsoleObject.h"
#include "framework/Singleton.h"
#include "SoundSource.h"


#include <wfmath/vector.h>
#include <wfmath/quaternion.h>
#include <wfmath/point.h>

#include <SDL_audio.h>

#include <list>
#include <unordered_map>
#include <filesystem>
#include <thread>
#include <optional>

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

namespace Ember {

class ConfigService;

struct IResourceProvider;

class StreamedSoundSample;

class BaseSoundSample;

/**
 * @brief A service responsible for playing and managing sounds.
 * In normal operations, the only way to play a sound is to first request a new instance of SoundInstance through createInstance(),
 * binding that instance to one or many sound samples and then asking the SoundInstance to start playing.
 * Once the SoundInstance is done playing it should be returned through destroyInstance().
 * Since it's expected that not too many sounds should be playing at one time it's not expected to be too many live instances of SoundInstance at any time.
 * Before you can start requesting sound instances and binding them to samples you must however set up the service.
 * The first thing that needs to be set up is a resource provider through the IResourceProvider interface.
 * The resource provider is responsible for providing any resource when so asked, and is the main interface into the actual sound data.
 * @author Romulo Fernandes Machado (nightz)
 * @author Erik Ogenvik <erik@ogenvik.org>
 */
class SoundService : public Service, public Singleton<SoundService> {
public:

	struct Sound {
		std::shared_ptr<BaseSoundSample> soundSample;
	};

	struct SoundGroup {
		std::vector<Sound> sounds;
		bool repeating;
		std::optional<float> gain;
		std::optional<float> rolloff;
		std::optional<float> reference;
	};

	struct SoundEntry {
		SoundGroup soundGroup;
		std::unique_ptr<SoundSource> source;
		int currentlyPlaying = -1;
	};

	struct SoundControl {
		std::function<void()> stop;
		std::function<void(WFMath::Point<3>)> setPosition;
		std::function<void(WFMath::Vector<3>)> setVelocity;
	};

	/**
	 * @brief Ctor.
	 */
	explicit SoundService(ConfigService& configService);

	~SoundService() override;

	std::shared_ptr<SoundControl> playSound(SoundGroup soundGroup);

	/**
	 * @brief Attempts to retrieve, or create if not already existing, the sound sample with the supplied identifier.
	 * Each sound sample is identified through the path to it, within the Ember resource system. This method will first look within the already allocated sound samples, and if the sought after sound sample is found there it will be returned.
	 * If not, it will try to create a new sound sample and return it. If no sound sample could be created (for example if no resource could be found) a null ref will be returned.
	 * @param soundPath The path to the sound data within the resource system.
	 * @return A sound sample, or null if none could be created.
	 */
	BaseSoundSample* createOrRetrieveSoundSample(const std::string& soundPath);


	/**
	 * @brief Update the position (in world coordinates) of the listener
	 * @param position The new listener position.
	 * @param direction The direction vector of the listener.
	 * @param up The up vector of the listener.
	 */
	void updateListenerPosition(const WFMath::Point<3>& pos, const WFMath::Vector<3>& direction, const WFMath::Vector<3>& up);

	/**
	 * @brief Call this each frame to update the sound samples.
	 * Through a call of this all registered and active SoundInstance instances will be asked to update themselves.
	 * Such an update could involve updating streaming buffers in the case of a streaming sound,
	 * or update the position of the sound if it's positioned within the 3d world.
	 */
	void cycle();

	/**
	 * @brief Gets the resource provider for this service.
	 * @return The resource provider registered for this service, or null if none has been registered.
	 */
	IResourceProvider* getResourceProvider();

	/**
	 * @brief Sets the resource provider which should be used for this service.
	 * It's through the resource provider that all sound data is loaded, so this must be called in order to have a properly functioning service.
	 * @param resourceProvider A pointer to the resource provider to use. Ownership will not be transferred to this service.
	 */
	void setResourceProvider(IResourceProvider* resourceProvider);

	/**
	 * @brief Returns true if the sound system is enabled.
	 * @return True if the sound is enabled.
	 */
	bool isEnabled() const;

private:

	/**
	 * @brief All the samples registered with the service are stored here.
	 * These are owned by the service and should be destroyed when the service is stopped.
	 */
	std::unordered_map<std::string, std::unique_ptr<BaseSoundSample>> mBaseSamples;

	/**
	 * @brief The main OpenAL context.
	 */
	ALCcontext* mContext;

	/**
	 * @brief The main OpenAL device.
	 */
	ALCdevice* mDevice;

	std::mutex mSoundEntriesMutex;

	std::vector<std::shared_ptr<SoundEntry>> mSoundEntries;

	/**
	 * @brief The resource provider used for loading resources.
	 * This is not owned by the service and won't be destroyed when the service shuts down.
	 */
	IResourceProvider* mResourceProvider;

	/**
	 * @brief True if the sound system is enabled.
	 * @see isEnabled()
	 */
	bool mEnabled;

	std::jthread mSoundProcessingThread;
}; //SoundService

} // namespace Ember

#endif
