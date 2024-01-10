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

class SoundSample;

class SoundSample;

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
		std::shared_ptr<SoundSample> soundSample;
	};

	struct SoundGroup {
		std::vector<Sound> sounds;
		bool repeating;
		std::optional<float> gain;
		std::optional<float> rolloff;
		std::optional<float> reference;
	};

	struct SoundEntry {
		~SoundEntry();

		SoundGroup soundGroup;
		std::unique_ptr<SoundSource> source;
		size_t currentlyPlaying;
		std::array<ALuint, 2> buffers;
	};

	/**
	 * Used by other components to control the sounds being played.
	 */
	struct SoundControl {
		/**
		 * Stop the sound. Once done the sound can't be interacted with anymore and will be cleaned up.
		 */
		std::function<void()> stop;
		/**
		 * Sets the position of the sound, in the world.
		 */
		std::function<void(WFMath::Point<3>)> setPosition;
		/**
		 * Sets the velocity of the sound, in the world.
		 */
		std::function<void(WFMath::Vector<3>)> setVelocity;
	};

	/**
	 * @brief Ctor.
	 */
	explicit SoundService(ConfigService& configService);

	~SoundService() override;

	std::shared_ptr<SoundControl> playSound(SoundGroup soundGroup);

	/**
	 * @brief Update the position (in world coordinates) of the listener
	 * @param position The new listener position.
	 * @param direction The direction vector of the listener.
	 * @param up The up vector of the listener.
	 */
	void updateListenerPosition(const WFMath::Point<3>& pos, const WFMath::Vector<3>& direction, const WFMath::Vector<3>& up) const;

	/**
	 * @brief Returns true if the sound system is enabled.
	 * @return True if the sound is enabled.
	 */
	bool isEnabled() const;

private:


	/**
	 * @brief The main OpenAL context.
	 */
	ALCcontext* mContext;

	/**
	 * @brief The main OpenAL device.
	 */
	ALCdevice* mDevice;

	std::mutex mSoundEntriesMutex;

	/**
	 * New sounds entries will be put in this vector, to be picked off by the background sound thread.
	 */
	std::vector<std::shared_ptr<SoundEntry>> mNewSoundEntries;


	/**
	 * @brief True if the sound system is enabled.
	 * @see isEnabled()
	 */
	bool mEnabled;

	std::jthread mSoundProcessingThread;
}; //SoundService

} // namespace Ember

#endif
