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
