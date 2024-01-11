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

#include "SoundSample.h"

#include "SoundGeneral.h"
#include "framework/Log.h"
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <array>

namespace Ember {


std::unique_ptr<SoundSample> SoundSample::create(const std::filesystem::path& filename) {
	auto vorbisFile = std::make_unique<OggVorbis_File>();
	auto status = ov_fopen(filename.generic_string().c_str(), vorbisFile.get());
	if (status != 0) {
		logger->warn("Error when trying to open sound file '{}': {}", filename.string(), status);
		return {};
	}

	return std::make_unique<SoundSample>(std::move(vorbisFile));
}


SoundSample::SoundSample(std::unique_ptr<OggVorbis_File> vorbisFile) :
		mVorbisFile(std::move(vorbisFile)),
		mPosition(0),
		mCurrentOffset(0) {

	vorbis_info* oggInfo = ov_info(mVorbisFile.get(), -1);
	if (oggInfo->channels == 1) {
		mFormat = AL_FORMAT_MONO16;
	} else {
		mFormat = AL_FORMAT_STEREO16;
	}

	mRate = oggInfo->rate;
	//Data is always 16 bits, so multiply by two.
	mTotalBytes = ov_pcm_total(mVorbisFile.get(), -1) * oggInfo->channels * 2;
}

SoundSample::~SoundSample() {
	ov_clear(mVorbisFile.get());
}

void SoundSample::reset() {
	ov_raw_seek(mVorbisFile.get(), 0);
	mCurrentOffset = 0;
}

SoundSample::BufferFillStatus SoundSample::fillBuffer(ALuint buffer) {
#ifndef LITTLE_ENDIAN
	auto endian = 1;
#else
	auto endian = 0;
#endif
	std::array<char, 65'536> dataBuffer{};

	int section;

	size_t offset = 0;
	while (offset < dataBuffer.size()) {
		auto result = ov_read(mVorbisFile.get(), dataBuffer.data() + offset, (int) std::min(4096UL, dataBuffer.size() - (size_t) offset), endian, 2, 1, &section);
		if (result > 0) {
			offset += result;
		} else {
			if (result < 0) {
				logger->error("Failed to read from ogg stream.");
				return BufferFillStatus::ERROR;
			} else break;
		}
	}

	mCurrentOffset += offset;

	alBufferData(buffer, mFormat, (void*) dataBuffer.data(), (ALsizei) offset, (ALint) mRate);
	if (!SoundGeneral::checkAlError("Generated buffer for static sample from OGG file.")) {
		return SoundSample::BufferFillStatus::ERROR;
	}

	if (mCurrentOffset < mTotalBytes) {
		return SoundSample::BufferFillStatus::HAS_MORE_DATA;
	} else {
		return SoundSample::BufferFillStatus::NO_MORE_DATA;
	}

}

}

