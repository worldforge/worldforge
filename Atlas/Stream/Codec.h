// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

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
    
    // Interface for top level context

    virtual void MessageBegin() = 0;
    virtual void MessageEnd() = 0;
    
    // Interface for map context
    
    virtual void ListBegin(const std::string& name) = 0;
    virtual void MapBegin(const std::string& name) = 0;
    virtual void Item(const std::string& name, int) = 0;
    virtual void Item(const std::string& name, float) = 0;
    virtual void Item(const std::string& name, const std::string&) = 0;
    virtual void Item(const std::string& name, const Atlas::Object&) = 0;
    virtual void ListEnd() = 0;
    
    // Interface for list context
    
    virtual void ListBegin() = 0;
    virtual void MapBegin() = 0;
    virtual void Item(int) = 0;
    virtual void Item(float) = 0;
    virtual void Item(const std::string&) = 0;
    virtual void Item(const Atlas::Object&) = 0;
    virtual void MapEnd() = 0;
};

#endif
