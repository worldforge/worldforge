// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Id$

#ifndef ATLAS_FACTORY_H
#define ATLAS_FACTORY_H

#include <string>
#include <list>
#include <algorithm>

namespace Atlas {

/** Class factory

Factory is a template class for automatic registration, construction and
destruction of particular classes. It is used by creating a static instance
for each class that requires it. Each registered class specifies a name and
Metrics (can be any class) that is used when enumerating the classes for the
purposes of negotiation. Each class also declares a Parameters structure,
which is passed to the constructor of the class at creation time by the
factory.

Both Codec and Filter specialise Factory and use it for class registration.

@see Codec
@see Filter
@see Negotiate
*/

template <typename T>
class Factory
{
    public:

    Factory(const std::string& name, const typename T::Metrics& metrics)
     : m_name(name), m_metrics(metrics)
    {
	factories()->push_back(this);
    }
    
    virtual ~Factory()
    {
	std::list<Factory*>::iterator i;
	i = std::find(factories()->begin(), factories()->end(), this);
	factories()->erase(i);
    }
    
    virtual T* New(const typename T::Parameters&) = 0;
    virtual void Delete(T*) = 0;

    std::string getName()
    {
	return m_name;
    }
    
    typename T::Metrics getMetrics()
    {
	return m_metrics;
    }
   
    static std::list<Factory*> * factories()
    {
	static std::list<Factory*> * m_factories = nullptr;
        if (m_factories == nullptr) {
            m_factories = new std::list<Factory*>;
            getFactories();
        }
	return m_factories;
    }

    protected:

    std::string m_name;
    typename T::Metrics m_metrics;

    private:

    static void getFactories();
};

} // Atlas namespace

#endif
