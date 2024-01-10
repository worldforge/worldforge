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

#include "SoundGeneral.h"
#include "framework/Log.h"

#include "al.h"

namespace Ember {
bool SoundGeneral::checkAlError(const std::string& description) {
	ALenum error = alGetError();
	if (error == AL_NO_ERROR) {
		return true;
	} else {
		std::string errorString = std::to_string(error);
		switch (error) {
			case AL_INVALID_NAME:
				errorString = "Invalid name";
				break;
			case AL_INVALID_ENUM:
				errorString = "Invalid enum";
				break;
			case AL_INVALID_VALUE  :
				errorString = "Invalid value";
				break;

			case AL_INVALID_OPERATION    :
				errorString = "Invalid operation";
				break;
			default:
				break;
		}

		if (description.empty()) {
			logger->error("OpenAl error: {}", errorString);
		} else {
			logger->error("OpenAl error when doing '{}': {}", description, errorString);
		}
		return false;
	}
}
}
