// This file may be redistributed and modified under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_STREAM_FILTER_H
#define ATLAS_STREAM_FILTER_H

#include <string>

#include <sigc++/signal_system.h>

namespace Atlas { namespace Stream {

class Filter
{
    public:
    
    virtual ~Filter() { };

    SigC::Signal1<void, std::string> output;
    
    virtual void Process(const std::string& data) = 0;
};

} } // Atlas::Stream

#endif
