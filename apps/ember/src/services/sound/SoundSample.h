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

#include <vorbis/vorbisfile.h>

#include "al.h"
#include <vector>
#include <filesystem>
#include <memory>
namespace Ember {

/**
 * A Sound Sample, read from OGG data.
 */
class SoundSample {
public:
	enum class BufferFillStatus {
		HAS_MORE_DATA,
		NO_MORE_DATA,
		ERROR
	};
	static std::unique_ptr<SoundSample> create(const std::filesystem::path& filename);

	explicit SoundSample(std::unique_ptr<OggVorbis_File> vorbisFile);

	~SoundSample() ;

	BufferFillStatus fillBuffer(ALuint buffer) ;

	void reset() ;


private:
	std::unique_ptr<OggVorbis_File> mVorbisFile;
	size_t mPosition;
	/**
	 * Format of the stream (checked from ogg/vorbis)
	 */
	ALenum mFormat;

	/**
	 * Rate of the stream (checked from ogg/vorbis)
	 */
	ALuint mRate;

	size_t mTotalBytes;
	size_t mCurrentOffset;
};

} // namespace Ember

#endif

