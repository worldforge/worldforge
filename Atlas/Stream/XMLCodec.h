// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Log$
// Revision 1.2  2000-02-22 08:32:24  mike
// Sketched out new codec interfaces.
//

#ifndef ATLAS_STREAM_XMLCODEC_H
#define ATLAS_STREAM_XMLCODEC_H

#include "Encoder.h"

namespace Atlas
{
    namespace Stream
    {
	class XMLEncoder;
	class XMLDecoder;
    }
}

class Atlas::Stream::XMLEncoder : public Atlas::Stream::Encoder
{
    public:
    
    virtual void Begin(Container);
    virtual void Begin(const std::string& name, Container);
    
    virtual void Send(int);
    virtual void Send(float);
    virtual void Send(const std::string&);
    virtual void Send(const Atlas::Object&);
	
    virtual void Send(const std::string& name, int);
    virtual void Send(const std::string& name, float);
    virtual void Send(const std::string& name, const std::string&);
    virtual void Send(const std::string& name, const Atlas::Object&);
    
    virtual void End();
};

#endif

