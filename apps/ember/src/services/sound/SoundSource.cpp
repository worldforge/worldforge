//
// C++ Implementation: SoundSource
//
// Description: 
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2008
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

#include "SoundSource.h"

#include "SoundGeneral.h"

#include "framework/Log.h"
#include "framework/Exception.h"
#include <cassert>

namespace Ember {

SoundSource::SoundSource()
		: mALSource(0) {
	alGetError();
	// Bind the buffer with the source.
	alGenSources(1, &mALSource);
	if (!SoundGeneral::checkAlError("Creating new sound source.")) {
		alDeleteSources(1, &mALSource);
		throw Exception("Failed to generate a new sound source.");
	}
}

SoundSource::~SoundSource() {
	if (alIsSource(mALSource)) {
		alDeleteSources(1, &mALSource);
		SoundGeneral::checkAlError("Deleting sound source.");
	}
}

void SoundSource::setPosition(const WFMath::Point<3>& pos) const {
	assert(pos.isValid());
	alSource3f(mALSource, AL_POSITION, (ALfloat) pos.x(), (ALfloat) pos.y(), (ALfloat) pos.z());
	SoundGeneral::checkAlError("Setting sound source position.");
}

void SoundSource::setVelocity(const WFMath::Vector<3>& vel) const {
	assert(vel.isValid());
	alSource3f(mALSource, AL_VELOCITY, (ALfloat) vel.x(), (ALfloat) vel.y(), (ALfloat) vel.z());
	SoundGeneral::checkAlError("Setting sound source velocity.");
}

void SoundSource::setGain(float gain) const {
	alSourcef(mALSource, AL_GAIN, gain);
	SoundGeneral::checkAlError("Setting sound source gain.");
}

void SoundSource::setRolloff(float rolloff) const {
	alSourcef(mALSource, AL_ROLLOFF_FACTOR, rolloff);
	SoundGeneral::checkAlError("Setting rolloff.");
}

void SoundSource::setReference(float reference) const {
	alSourcef(mALSource, AL_REFERENCE_DISTANCE, reference);
	SoundGeneral::checkAlError("Setting reference.");
}


}
