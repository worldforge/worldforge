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

#include "core/Repository.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <utility>
#include <algorithm>

using namespace Squall;
using namespace Catch::Matchers;

TEST_CASE("Signatures can be created", "[signature]") {
	setupEncodings();

	SECTION("valid signatures should be created") {
		REQUIRE(Signature("2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae").isValid());
		REQUIRE(Signature(std::string("2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae")).isValid());
		std::string digest("2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae");
		REQUIRE(Signature(std::string_view(digest)).isValid());
		std::array<char, 64> digestArray{};
		digestArray.fill('a');
		REQUIRE(Signature(digestArray, 64).isValid());
	};

	SECTION("signatures are copied") {
		Signature signature("2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae");
		auto signatureCopy = signature;
		REQUIRE(signature == signatureCopy);
		REQUIRE(signature.digest == signatureCopy.digest);
		REQUIRE(signature.digestLength == signatureCopy.digestLength);
		REQUIRE(signatureCopy.isValid());
	};

	SECTION("invalid signatures should not be allowed") {
		REQUIRE_THROWS(Signature(""));
		REQUIRE_THROWS(Signature("1"));
		REQUIRE_THROWS(Signature("12"));
		REQUIRE_THROWS(Signature("2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae"));
		Signature("123");
	};
}