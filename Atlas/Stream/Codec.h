// This file may be redistributed and modified under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_STREAM_CODEC_H
#define ATLAS_STREAM_CODEC_H

#include "../Object/Object.h"
#include "../Net/Socket.h"
#include "Filter.h"

#include <list>
#include <algorithm>

namespace Atlas { namespace Stream {

/** Atlas stream codec

This class presents an interface for sending and receiving Atlas messages.
Each outgoing message is converted to a byte stream and piped through an
optional chain of filters for compression or other transformations, then
passed to a socket for transmission. Incoming messages are read from the
socket, piped through the filters in the opposite direction and passed to
user specified callback functions FIXME this doesn't happen yet FIXME

Codecs should declare an instance of Codec::Factory in the module they are
defined in. This will allow them to be automatically included in the
negotiation process that chooses which codecs and filters an Atlas connection
will use.

@see Filter
@see Socket
@see Factory
@see Negotiate

*/

class Codec
{
    public:

    Codec(Socket*, Filter* = 0);
    virtual ~Codec();

    // Interface for top level context

    virtual void MessageBegin() = 0;
    virtual void MessageEnd() = 0;
    
    // Interface for map context
    
    virtual void ListBegin(const std::string& name) = 0;
    virtual void MapBegin(const std::string& name) = 0;
    virtual void Item(const std::string& name, int) = 0;
    virtual void Item(const std::string& name, float) = 0;
    virtual void Item(const std::string& name, const std::string&) = 0;
    virtual void Item(const std::string& name, const Atlas::Object&) = 0;
    virtual void ListEnd() = 0;
    
    // Interface for list context
    
    virtual void ListBegin() = 0;
    virtual void MapBegin() = 0;
    virtual void Item(int) = 0;
    virtual void Item(float) = 0;
    virtual void Item(const std::string&) = 0;
    virtual void Item(const Atlas::Object&) = 0;
    virtual void MapEnd() = 0;

    class Metrics
    {
	public:

	Metrics(int speed, int bandwidth) { }
    };
	
    static std::list<Atlas::Stream::Factory<Codec>*> factories;

    template <typename T>
    class Factory : public Atlas::Stream::Factory<Codec>
    {
	public:

	Factory(const std::string& name, const Metrics& metrics)
	    : name(name), metrics(metrics)
	{
	    factories.push_back(this);
	}
	    
	virtual ~Factory()
	{
	    std::list<Atlas::Stream::Factory<Codec>*>::iterator i;
	    i = std::find(factories.begin(), factories.end(), this);
	    factories.erase(i);
	}

	virtual Codec* New()
	{
	    return new T;
	}

	virtual void Delete(Codec* codec)
	{
	    delete codec;
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

    Socket *socket;
    Filter *filter;
};

} } // Atlas::Stream

#endif
