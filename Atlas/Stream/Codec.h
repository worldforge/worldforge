// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_STREAM_CODEC_H
#define ATLAS_STREAM_CODEC_H

#include "Bridge.h"
#include "Filter.h"

namespace Atlas { namespace Stream {

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
@see Filter
@see Factory
@see Negotiate
*/

template <class Stream>
class Codec : public Bridge
{
    public:

    virtual ~Codec() { }

    virtual void Poll() = 0;

    class Metrics
    {
	public:

	Metrics(int speed, int bandwidth) { }
    };

    struct Parameters
    {
        Stream& stream;
	Filter* filter;
	Bridge* bridge;

        Parameters(Stream& stream, Filter* filter, Bridge* bridge) 
           : stream(stream), filter(filter), bridge(bridge) { }
    };

    template <typename T>
    class Factory : public Atlas::Stream::Factory<Codec>
    {
	public:

	Factory(const std::string& name, const Metrics& metrics)
	    : Atlas::Stream::Factory<Codec>(name, metrics)
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

} } // Atlas::Stream

#endif
