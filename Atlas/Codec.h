// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_CODEC_H
#define ATLAS_CODEC_H

#include <iosfwd>
#include "Bridge.h"
#include "Task.h"
#include "Factory.h"

namespace Atlas {

/** Atlas stream codec

This class presents an interface for sending and receiving Atlas messages.
Each outgoing message is converted to a byte stream and piped through an
optional chain of filters for compression or other transformations, then
passed to a socket for transmission. Incoming messages are read from the
socket, piped through the filters in the opposite direction and passed to
a user specified Bridge callback class.

Codecs declare an instance of Codec::Factory in the module they are defined
in. This allows them to be automatically included in the negotiation process
that chooses which codecs and filters an Atlas connection will use. Each
codec has metrics that allow Negotiate to make informed decisions when more
than one codec is available for use. Currently these metrics consist of a
two integers to represent the cpu speed and bandwidth that the codec uses.
However, the scale of these values is not yet decided upon. FIXME

@see Bridge
@see Task
@see Factory
@see Negotiate
*/

template <class Stream>
class Codec : public Bridge, public Task
{
    public:

    virtual ~Codec() { }

    class Metrics
    {
	public:

	Metrics(int speed, int bandwidth) { }
    };

    struct Parameters
    {
        Stream& stream;
	Bridge* bridge;

        Parameters(Stream& stream, Bridge* bridge) 
           : stream(stream), bridge(bridge) { }
    };

    template <typename T>
    class Factory : public Atlas::Factory<Codec>
    {
	public:

	Factory(const std::string& name, const Metrics& metrics)
	    : Atlas::Factory<Codec>(name, metrics)
	{
	}

	virtual Codec* New(const Parameters& p)
	{
	    return new T(p);
	}

	virtual void Delete(Codec* codec)
	{
	    delete codec;
	}
    };
};

} // Atlas namespace

#endif
