// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Log$
// Revision 1.5  2000-03-04 23:44:08  mike
// Begun work on codec/filter negotiation
//
// Revision 1.4  2000/02/22 03:55:45  mike
// Added output signal to Filter.
//
// Revision 1.3  2000/02/21 11:49:49  mike
// Added change log
//

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
