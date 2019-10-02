// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Aloril and Al Riddoch

// $Id$

#ifndef ATLAS_LOAD_DEFAULTS_H
#define ATLAS_LOAD_DEFAULTS_H

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Exception.h>

namespace Atlas {
namespace Objects {

class DefaultLoadingException : public Atlas::Exception {
public:
	explicit DefaultLoadingException(const std::string& a_msg) noexcept :
			Atlas::Exception(a_msg) {}
};

void loadDefaults(const std::string& filename, const Factories& factories);

}
} // namespace Atlas::Objects
#endif
