// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_STREAM_DECODER_H
#define ATLAS_STREAM_DECODER_H

#include "Filter.h"
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


