// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Log$
// Revision 1.4  2000-02-23 06:49:34  mike
// Big changes. Descriptive, huh.
//

#ifndef ATLAS_STREAM_CODEC_H
#define ATLAS_STREAM_CODEC_H

#include "../Object/Object.h"

#include <sigc++/signal_system.h>

namespace Atlas
{
    namespace Stream
    {
	class Codec;
    }
}

class Atlas::Stream::Codec
{
    public:

    virtual ~Codec() { }

    SigC::Signal1<void, std::string> output;

    virtual void BeginMap() = 0;
    virtual void BeginMap(const std::string& name) = 0;
    virtual void EndMap() = 0;
    
    virtual void BeginList() = 0;
    virtual void BeginList(const std::string& name) = 0;
    virtual void EndList() = 0;

    virtual void Send(int) = 0;
    virtual void Send(float) = 0;
    virtual void Send(const std::string&) = 0;
    virtual void Send(const Atlas::Object&) = 0;
	
    virtual void Send(const std::string& name, int) = 0;
    virtual void Send(const std::string& name, float) = 0;
    virtual void Send(const std::string& name, const std::string&) = 0;
    virtual void Send(const std::string& name, const Atlas::Object&) = 0;
};

#endif
