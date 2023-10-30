// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Dmitry Derevyanko

// $Id$

#ifndef ATLAS_FILTERS_GZIP_H
#define ATLAS_FILTERS_GZIP_H

#include <Atlas/Filter.h>

#include <zlib.h>

namespace Atlas { namespace Filters {

class Gzip : public Filter
{
    z_stream incoming;
    z_stream outgoing;
    unsigned char buf[4096];

    public:

    void begin() override;
    void end() override;
    
    std::string encode(const std::string&) override;
    std::string decode(const std::string&) override;
};

} } // namespace Atlas::Filters

#endif // ATLAS_FILTERS_GZIP_H
