// This file is distributed under the GNU Lesser General Public license.
// Copyright (C) 2000 Dmitry Derevyanko (See the file COPYING for details).

#ifndef ATLAS_NET_GZIP_FILTER_H
#define ATLAS_NET_GZIP_FILTER_H

#include <zlib.h>

#include "Filter.h"

namespace Atlas {

  class GzipFilter : public Filter {

    z_stream incoming;
    z_stream outgoing;
    unsigned char buf[4096];

  public:

    GzipFilter(int level=6);
        
    ~GzipFilter(void);
    
    string encode(const string& data);
    
    string decode(const string& data);
  };

};


#endif // ATLAS_NET_GZIP_FILTER_H
