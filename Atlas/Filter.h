// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_FILTER_H
#define ATLAS_FILTER_H

#include "Factory.h"

namespace Atlas {

/** Atlas stream filter

Filters are used by Codec to transform the byte stream before transmission.
The transform must be invertible; that is to say, encoding a string and then
decoding it must result in the original string. Filters can be used for
compression, encryption or performing checksums and other forms of
transmission error detection. A compound filter can be created that acts like
a single filter, allowing various filters to be chained together in useful
ways such as compressing and then encrypting.

Filters declare an instance of Filter::Factory in the module they are defined
in. This allows them to be automatically included in the negotiation process
that chooses which codecs and filters an Atlas connection will use. Each
filter has metrics that allow Negotiate to make informed decisions when more
than one filter is available for use. Currently these metrics consist of the
type of the filter, whether it is for compression, encryption or check
summing.

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

    struct Parameters
    {
    };

    template <typename T>
    class Factory : public Atlas::Factory<Filter>
    {
	public:

	Factory(const std::string& name, const Metrics &metrics)
	    : Atlas::Factory<Filter>(name, metrics)
	{
	}
	    
	virtual Filter* New(const Parameters&)
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

} // Atlas namespace

#endif
