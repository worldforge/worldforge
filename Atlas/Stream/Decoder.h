// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Log$
// Revision 1.4  2000-02-22 05:34:03  mike
// Removed inclusion of Filter into Decoder and Encoder.
// A arbitrary chain of filters can now be fairly easily attached to these
// by use of signals and slots.
//
// Revision 1.3  2000/02/21 11:49:48  mike
// Added change log
//

#ifndef ATLAS_STREAM_DECODER_H
#define ATLAS_STREAM_DECODER_H

#include "../Object/Object.h"

namespace Atlas
{
    namespace Stream
    {
	class Decoder;
    }
}

class Atlas::Stream::Decoder
{
    public:

    virtual ~Decoder() { }
};

#endif


