// This file is distributed under the GNU Lesser General Public license.
// Copyright (C) 2000 Dmitry Derevyanko (See the file COPYING for details).

#ifndef ATLAS_NET_GZIP_FILTER_H
#define ATLAS_NET_GZIP_FILTER_H

#include "Filter.h"

#include <zlib.h>

namespace Atlas {

  class GzipFilter : public Filter {

    z_stream incoming;
    z_stream outgoing;
    unsigned char buf[4096];

  public:

    GzipFilter(int level=6);
        
    ~GzipFilter(void);
    
    std::string encode(const std::string& data);
    
    std::string decode(const std::string& data);
  };

};


#endif // ATLAS_NET_GZIP_FILTER_H
