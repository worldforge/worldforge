// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Log$
// Revision 1.2  2000-02-22 08:32:24  mike
// Sketched out new codec interfaces.
//
// Revision 1.1  2000/02/22 08:29:04  mike
// Migrated all Encoder and Decoder files into unified Codec files.
//
// Revision 1.3  2000/02/22 05:31:22  mike
// Fixed a quick namespace bug relating to use of unqualified Atlas::Object.
//
// Revision 1.2  2000/02/22 04:19:11  mike
// Rewrote PackedEncoder interface as per the new Encoder design.
//

#ifndef ATLAS_STREAM_PACKEDCODEC_H
#define ATLAS_STREAM_PACKEDCODEC_H

#include "Encoder.h"

namespace Atlas
{
    namespace Stream
    {
	class PackedEncoder;
	class PackedDecoder;
    }
}

class Atlas::Stream::PackedEncoder : public Atlas::Stream::Encoder
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

