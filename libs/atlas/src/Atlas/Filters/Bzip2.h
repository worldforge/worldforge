// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit, Dmitry Derevyanko

// $Id$

#ifndef ATLAS_FILTERS_BZIP2_H
#define ATLAS_FILTERS_BZIP2_H

#include <Atlas/Filter.h>

extern "C" {
#include <bzlib.h>
}
#include <array>


namespace Atlas::Filters {

class Bzip2 : public Filter {
	bz_stream incoming;
	bz_stream outgoing;
	std::array<char, 4096> buf;

public:

	Bzip2();

	void begin() override;

	void end() override;

	std::string encode(const std::string&) override;

	std::string decode(const std::string&) override;
};

}
// namespace Atlas::Filters

#endif // ATLAS_FILTERS_BZIP2_H
