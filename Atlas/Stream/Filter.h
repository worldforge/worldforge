// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_STREAM_FILTER_H
#define ATLAS_STREAM_FILTER_H

#include <string>

namespace Atlas
{
    namespace Stream
    {
	class Filter;
    }
}

class Atlas::Stream::Filter
{
    public:
    
    virtual ~Filter() { };
    
    virtual string Process(const std::string& data) = 0;
};

#endif
