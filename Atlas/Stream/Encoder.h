// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Log$
// Revision 1.12  2000-02-22 05:34:03  mike
// Removed inclusion of Filter into Decoder and Encoder.
// A arbitrary chain of filters can now be fairly easily attached to these
// by use of signals and slots.
//
// Revision 1.11  2000/02/22 05:31:22  mike
// Fixed a quick namespace bug relating to use of unqualified Atlas::Object.
//
// Revision 1.10  2000/02/22 03:52:38  mike
// Added output signal to Encoder, the first (but definitely not the last)
// usage of libsigc++ within the library.
//
// Revision 1.9  2000/02/21 11:49:49  mike
// Added change log
//

#ifndef ATLAS_STREAM_ENCODER_H
#define ATLAS_STREAM_ENCODER_H

#include "../Object/Object.h"

#include <sigc++/signal_system.h>

namespace Atlas
{
    namespace Stream
    {
	class Encoder;
    }
}

class Atlas::Stream::Encoder
{
    public:

    virtual ~Encoder() { }

    SigC::Signal1<void, std::string> output;

    enum Container
    {
	LIST,
	MAP,
    };
    
    virtual void Begin(Container) = 0;
    virtual void Begin(const std::string& name, Container) = 0;
    
    virtual void Send(int) = 0;
    virtual void Send(float) = 0;
    virtual void Send(const std::string&) = 0;
    virtual void Send(const Atlas::Object&) = 0;
	
    virtual void Send(const std::string& name, int) = 0;
    virtual void Send(const std::string& name, float) = 0;
    virtual void Send(const std::string& name, const std::string&) = 0;
    virtual void Send(const std::string& name, const Atlas::Object&) = 0;
    
    virtual void End() = 0;
};

#endif
