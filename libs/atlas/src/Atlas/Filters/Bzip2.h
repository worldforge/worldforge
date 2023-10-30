// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit, Dmitry Derevyanko

// $Id$

#ifndef ATLAS_FILTERS_BZIP2_H
#define ATLAS_FILTERS_BZIP2_H

#include <Atlas/Filter.h>

// my version of bzlib.h does not have extern "C" in the header file,
// like it should
// dmitryd 05/08/200
extern "C" {
#include <bzlib.h>
}

namespace Atlas { namespace Filters {

class Bzip2 : public Filter
{
    bz_stream incoming;
    bz_stream outgoing;
    char buf[4096];

    public:

    void begin() override;
    void end() override;
    
    std::string encode(const std::string&) override;
    std::string decode(const std::string&) override;
};

} } // namespace Atlas::Filters

#endif // ATLAS_FILTERS_BZIP2_H
