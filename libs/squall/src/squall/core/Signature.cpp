/*
 Copyright (C) 2022 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Signature.h"
#include <stdexcept>
#include <cstring>

namespace Squall {
Signature::Signature(std::array<char, maxDigestLength> data, size_t length)
		: digest(data),
		  digestLength(length) {
	if (digestLength < minDigestLength) {
		throw std::runtime_error("Signature data must be at least 3 characters.");
	}
	if (digestLength > maxDigestLength) {
		throw std::runtime_error("Signature data must not exceed 64 characters.");
	}
}

Signature::Signature(const char* data)
		: digest{},
		  digestLength(std::strlen(data)) {
	if (digestLength < minDigestLength) {
		throw std::runtime_error("Signature data must be at least 3 characters.");
	}
	if (digestLength > maxDigestLength) {
		throw std::runtime_error("Signature data must not exceed 64 characters.");
	}

	std::strncpy(digest.data(), data, maxDigestLength);
}

Signature::Signature(const std::string& data)
		: digest{},
		  digestLength(data.length()) {
	if (digestLength < minDigestLength) {
		throw std::runtime_error("Signature data must be at least 3 characters.");
	}
	if (digestLength > maxDigestLength) {
		throw std::runtime_error("Signature data must not exceed 64 characters.");
	}
	digest.fill('\0');

	std::copy(data.begin(), data.end(), digest.begin());
	digestLength = data.length();
}

Signature::Signature(std::string_view data)
		: digest{},
		  digestLength(data.length()) {
	if (digestLength < minDigestLength) {
		throw std::runtime_error("Signature data must be at least 3 characters.");
	}
	if (digestLength > maxDigestLength) {
		throw std::runtime_error("Signature data must not exceed 64 characters.");
	}
	digest.fill('\0');

	std::copy(data.begin(), data.end(), digest.begin());
}

std::string_view Signature::str_view() const {
	return {digest.data(), digestLength};
}

std::string Signature::str() const {
	return std::string(str_view());
}

bool Signature::operator==(const Signature& rhs) const noexcept {
	return digest == rhs.digest;
}

bool Signature::operator!=(const Signature& rhs) const noexcept {
	return digest != rhs.digest;
}

bool Signature::isValid() const {
	return digestLength <= maxDigestLength && digestLength > minDigestLength;
}
}
