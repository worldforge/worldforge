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

A filter transforms a byte stream into another byte stream. The transform
must be invertible; that is to say, encoding a string and then decoding it
must result in the original string. Filters can be used for compression,
encryption or performing checksums and other forms of transmission error
detection. A compound filter can be created that acts like a single filter,
allowing various filters to be chained together in useful ways such as
compressing and then encrypting.

FIXME talk about filter factories and metrics FIXME

Filters are used by Codec to transform the byte stream before transmission.
A filter chain is created during negotiation to ensure that both sides of
the Atlas connection are using the same filters in the same order.

@see Codec
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

    static std::list<Atlas::Stream::Factory<Filter>*> factories;

    template <typename T>
    class Factory : public Atlas::Stream::Factory<Filter>
    {
	public:

	Factory(const std::string& name, const Metrics &metrics)
	    : name(name), metrics(metrics)
	{
	    factories.push_back(this);
	}
	    
	virtual ~Factory()
	{
	    std::list<Atlas::Stream::Factory<Filter>*>::iterator i;
	    i = std::find(factories.begin(), factories.end(), this);
	    factories.erase(i);
	}

	virtual Filter* New()
	{
	    return new T;
	}

	virtual void Delete(Filter* filter)
	{
	    delete filter;
	}

	virtual std::string GetName()
	{
	    return name;
	}

	virtual Metrics GetMetrics()
	{
	    return metrics;
	}

	private:

	std::string name;
	Metrics metrics;
    };

    protected:

    Filter* next;
};

} } // Atlas::Stream

#endif
