// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Al Riddoch

// $Id$

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Atlas/Factory.h>

#include <Atlas/Codecs/XML.h>
#include <Atlas/Codecs/Packed.h>

#include <Atlas/Filters/Gzip.h>
#include <Atlas/Filters/Bzip2.h>

namespace Atlas {

void Factory<Codec<std::iostream> >::getFactories()
{
    new Codec::Factory<Codecs::XML> ("XML", Codec::Metrics(1,2));
    new Codec::Factory<Codecs::Packed> ("Packed", Codec::Metrics(1,2));
    new Codec::Factory<Codecs::Bach> ("Bach", Codec::Metrics(1,2));
}

void Factory<Filter>::getFactories()
{
#if defined(HAVE_ZLIB_H) && defined(HAVE_LIBZ)
    new Filter::Factory<Gzip> ("GZIP", Filter::Metrics(Filter::COMPRESSION));
#endif

#if defined(HAVE_BZLIB_H) && defined(HAVE_LIBBZ2)
    new Filter::Factory<Bzip2> ("BZIP2", Filter::Metrics(Filter::COMPRESSION));
#endif
}

} // namespace Atlas
