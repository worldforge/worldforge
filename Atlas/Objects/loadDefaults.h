// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Aloril

#ifndef ATLAS_LOAD_DEFAULTS_H
#define ATLAS_LOAD_DEFAULTS_H

#include "Decoder.h"

namespace Atlas {

class DefaultLoadingException
{
public:
    DefaultLoadingException(const std::string& a_msg) : msg(a_msg) {}
    std::string msg;
};

void loadDefaults(const string& filename);

} // namespace Atlas
#endif
