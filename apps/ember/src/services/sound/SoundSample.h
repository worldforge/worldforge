/*
    Copyright (C) 2008 Romulo Fernandes Machado (nightz)

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

#ifndef SOUND_SAMPLE_H
#define SOUND_SAMPLE_H

#include "SoundGeneral.h"
#include "framework/IResourceProvider.h"
#include <vector>

#include "al.h"


namespace Ember {
class SoundSource;


/**
 * Sound Sample 
 *
 * Defines general properties of sound data
 */
class BaseSoundSample {
public:

	/**
	 * Dtor.
	 */
	virtual ~BaseSoundSample() = default;

	/**
	 * @return entity type
	 */
	SoundGeneral::SoundSampleType getType() const;

	/**
	 * @brief Returns the number of buffers stored for this sample.
	 * @return The number of buffers.
	 */
	virtual unsigned int getNumberOfBuffers() const = 0;

	/**
	 * @brief Returns a store of the sound data buffers stored by this sample.
	 * The buffers will be returned as ALuint which is the internal buffer reference within OpenAL.
	 * Any further operation on the buffer must therefore go through OpenAL (i.e. the values returned are _not_ memory pointers).
	 * @return A store of OpenAL buffer identifiers.
	 */
	virtual std::vector<ALuint> getBuffers() const = 0;

protected:

	/**
	 * @brief Ctor. This is protected to disallow direct creation of this class except by subclasses.
	 */
	explicit BaseSoundSample(SoundGeneral::SoundSampleType type) : mType(type) {
	}

	/**
	 * Type of the sample
	 */
	SoundGeneral::SoundSampleType mType;
};

/**
 * The class StaticSoundSample is responsible to keep track of samples that doesnt need often updates and only have one buffer
 */
class StaticSoundSample : public BaseSoundSample {
public:


	static std::unique_ptr<StaticSoundSample> create(const ResourceWrapper& resource);

	/**
	 * Ctor.
	 */
	explicit StaticSoundSample(ALuint buffer);

	/**
	 * Dtor.
	 */
	~StaticSoundSample() override;

	/**
	 * @return the unique buffer this sample has.
	 */
	ALuint getBuffer() const;

	/**
	 * Within this class, this is always 1.
	 */
	unsigned int getNumberOfBuffers() const override;

	/**
	 * @copydoc BaseSoundSample::getBuffers()
	 */
	std::vector<ALuint> getBuffers() const override;

private:
	/**
	 * Sample buffer
	 */
	ALuint mBuffer;

};


/**
 * The class StreamedSoundSample is responsible to keep track of samples that often need updates and requires more than a buffer to stream data.
 *
 * \todo not implemented
 */
// class StreamedSoundSample : public BaseSoundSample
// {
// 	private:
// 		/**
// 		 * Filename with full path to the data.
// 		 */
// 		std::string		mFilename;
// 
// 		/**
// 		 * A pointer to the file specified in mFilename
// 		 */
// 		FILE*				mFile;
// 
// 		/**
// 		 * VORBIS Internal Stream 
// 		 */
// 		OggVorbis_File mStream;
// 
// 		/**
// 		 * Front and back buffers for openAl
// 		 */
// 		ALuint			mBuffers[2];
// 
// 		/**
// 		 * Format of the stream (checked from ogg/vorbis)
// 		 */
// 		ALenum			mFormat;
// 
// 		/**
// 		 * Rate of the stream (checked from ogg/vorbis)
// 		 */
// 		ALuint			mRate;
// 
// 		/**
// 		 * If this stream is playing
// 		 */
// 		bool				mPlaying;
// 
// 		/**
// 		 * This function is responsible to fill
// 		 * buffers from stream data
// 		 *
// 		 * @param buffer The destination openAl buffer
// 		 * @return Status of the streaming
// 		 */
// 		bool stream(ALuint buffer);
// 
// 	public:
// 		StreamedSoundSample(const std::string& filename, bool playsLocal, float volume);
// 		~StreamedSoundSample();
// 
// 		/**
// 		 * Set the file to be used in stream proccess.
// 		 *
// 		 * @param ptr A pointer to the file.
// 		 * @param filename The file name with full path.
// 		 */
// 		void setFile(FILE* ptr, const std::string& filename);
// 
// 		/**
// 		 * Set stream format
// 		 */
// 		void setFormat(ALenum fmt);
// 
// 		/**
// 		 * Set stream rate
// 		 */
// 		void setRate(ALuint rate);
// 
// 		/**
// 		 * Set the stream status (if playing or not)
// 		 */
// 		void setPlaying(bool play);
// 
// 		/**
// 		 * Returns a pointer to the buffers array
// 		 */
// 		ALuint*				getBufferPtr();
// 
// 		/**
// 		 * Returns a pointer to the stream information (vorbis internals).
// 		 */
// 		OggVorbis_File*	getStreamPtr();
// 
// 		/**
// 		 * Return the state of the stream.
// 		 */
// 		bool					isPlaying();
// 
// 		/**
// 		 * Return the number of buffers in this stream. In this case this is 2.
// 		 */
// 		unsigned int		getNumberOfBuffers();
// 
// 		/**
// 		 * Return the full filename of the stream file.
// 		 */
// 		const std::string& getFilename();
// 
// 		// Common methods
// 		void play();	
// 		void stop();
// 		void cycle();				
// };

} // namespace Ember

#endif

