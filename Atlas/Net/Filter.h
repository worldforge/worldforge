// This file is distributed under the GNU Lesser General Public license.
// Copyright (C) 2000 Michael Day (See the file COPYING for details).

#ifndef ATLAS_NET_FILTER_H
#define ATLAS_NET_FILTER_H

#include <string>
using std::string;

namespace Atlas
{
    class Filter;
}

/** Interface for Atlas protocol filter
*/

class Atlas::Filter
{
    public:
    
    virtual ~Filter() { }
    
    virtual string encode(const string& data) = 0;
    virtual string decode(const string& data) = 0;
};

#endif
