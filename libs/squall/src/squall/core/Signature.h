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

#ifndef SQUALL_SIGNATURE_H
#define SQUALL_SIGNATURE_H

#include <array>
#include <string_view>
#include <string>
#include <iostream>

namespace Squall {

/**
 * A signature, which is a Blake3 digest of a file's content.
 * We'll use this instead of std::string to speed things up, since we know that most
 * digests are 64 characters and we want to make it easier for the compiler to allocate memory.
 */
struct Signature {
	static constexpr size_t maxDigestLength = 64;
	static constexpr size_t minDigestLength = 3;

	/**
	 * The digest characters, not null terminated.
	 */
	std::array<char, maxDigestLength> digest;
	/**
	 * Actual length of the digest since it might be less than maxDigestLength, and we don't use a null terminator.
	 */
	size_t digestLength;

	Signature() = default;

	explicit Signature(std::array<char, maxDigestLength> data, size_t length);

	/**
	 * Implicit ctor by design.
	 * @param data A null terminated string. We'll extract everything up to the null character as the digest (max maxDigestLength though).
	 */
	Signature(const char* data);

	/**
	 * Implicit ctor by design.
	 * @param data A string. We'll copy the content into the digest, up to minDigestLength.
	 */
	Signature(const std::string& data);

	/**
	 * Implicit ctor by design.
	 * @param data A string view. We'll copy the content into the digest, up to minDigestLength.
	 */
	Signature(std::string_view data);

	Signature(const Signature& rhs) = default;

	Signature(Signature&& rhs) = default;

	Signature& operator=(const Signature& rhs) = default;

	Signature& operator=(Signature&& rhs) noexcept = default;

	std::string_view str_view() const;

	std::string str() const;

	explicit operator std::string_view() const {
		return str_view();
	}

	bool operator==(const Signature& rhs) const noexcept;

	bool operator!=(const Signature& rhs) const noexcept;

	/**
	 * Checks that the length of the digest fits our requirements.
	 * @return
	 */
	bool isValid() const;
};
}

inline std::ostream& operator<<(std::ostream& out, const Squall::Signature& signature) {
	//Needs to be inlined here in order for Clang to pick up the symbols, for some reason.
	out << signature.str_view();
	return out;
}

#endif //SQUALL_SIGNATURE_H
