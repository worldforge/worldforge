// This file may be redistributed and modified under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_STREAM_FILTER_H
#define ATLAS_STREAM_FILTER_H

#include "Factory.h"

#include <string>
#include <list>
#include <algorithm>

namespace Atlas { namespace Stream {

/** Atlas stream filter

Filters are used by Codec to transform the byte stream before transmission.
The transform must be invertible; that is to say, encoding a string and then
decoding it must result in the original string. Filters can be used for
compression, encryption or performing checksums and other forms of
transmission error detection. A compound filter can be created that acts like
a single filter, allowing various filters to be chained together in useful
ways such as compressing and then encrypting.

Filters should declare an instance of Filter::Factory in the module they are
defined in. This will allow them to be automatically included in the
negotiation process that chooses which codecs and filters an Atlas connection
will use. FIXME talk about filter metrics FIXME

@see Codec
@see Factory
@see Negotiate

*/

class Filter
{
    public:
   
    Filter(Filter* = 0);
    virtual ~Filter();

    virtual void Begin() = 0;
    virtual void End() = 0;

    virtual std::string Encode(const std::string&) = 0;
    virtual std::string Decode(const std::string&) = 0;

    enum Type
    {
	CHECKSUM,
	COMPRESSION,
	ENCRYPTION,
    };

    class Metrics
    {
	public:

	Metrics(Type) { }
    };

    template <typename T>
    class Factory : public Atlas::Stream::Factory<Filter>
    {
	public:

	Factory(const std::string& name, const Metrics &metrics)
	    : Atlas::Stream::Factory<Filter>(name, metrics)
	{
	}
	    
	virtual Filter* New()
	{
	    return new T;
	}

	virtual void Delete(Filter* filter)
	{
	    delete filter;
	}
    };

    protected:

    Filter* next;
};

} } // Atlas::Stream

#endif
