// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Dmitry Derevyanko

#ifndef ATLAS_FILTERS_GZIP_H
#define ATLAS_FILTERS_GZIP_H

#if defined(HAVE_ZLIB_H) && defined(HAVE_LIBZ)

#include <zlib.h>

#include "../Filter.h"

namespace Atlas { namespace Filters {

class Gzip : public Filter
{
    z_stream incoming;
    z_stream outgoing;
    unsigned char buf[4096];

    public:

    virtual void begin();
    virtual void end();
    
    virtual std::string encode(const std::string&);
    virtual std::string decode(const std::string&);
};

} } // namespace Atlas::Filters

#endif // HAVE_ZLIB_H && HAVE_LIBZ

#endif // ATLAS_FILTERS_GZIP_H
