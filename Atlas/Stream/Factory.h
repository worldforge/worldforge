// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_STREAM_FACTORY_H
#define ATLAS_STREAM_FACTORY_H

#include <string>
#include <list>
#include <algorithm>

namespace Atlas { namespace Stream {

/** Class factory

Factory is a template class for automatic registration, construction and
destruction of particular classes. It can be used by creating a static
instance for each class that requires it. Both Codec and Filter specialise
Factory and use it for class registration.

FIXME talk about name and metrics and parameters FIXME

@see Codec
@see Filter

*/

template <typename T>
class Factory
{
    public:

    typedef typename T::Metrics Metrics;
    typedef typename T::Parameters Parameters;

    Factory(const std::string& name, const Metrics& metrics)
     : name(name), metrics(metrics)
    {
	factories.push_back(this);
    }
    
    virtual ~Factory()
    {
	std::list<Factory*>::iterator i;
	i = std::find(factories.begin(), factories.end(), this);
	factories.erase(i);
    }
    
    virtual T* New(const Parameters&) = 0;
    virtual void Delete(T*) = 0;

    std::string GetName()
    {
	return name;
    }
    
    Metrics GetMetrics()
    {
	return metrics;
    }
    
    static std::list<Factory*> factories;

    protected:

    std::string name;
    Metrics metrics;
};

} } // Atlas::Stream

#endif
