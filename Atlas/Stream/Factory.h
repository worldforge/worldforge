// This file may be redistributed and modified under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_STREAM_FACTORY_H
#define ATLAS_STREAM_FACTORY_H

#include <string>

namespace Atlas { namespace Stream {

/** Class factory

Factory is a template class for automatic registration, construction and
destruction of particular classes. It can be used by creating a static
instance for each class that requires it. Both Codec and Filter specialise
Factory and use it for class registration.

@see Codec
@see Filter

*/

template <typename T>
class Factory
{
    public:

    virtual ~Factory() { }

    virtual T* New() = 0;
    virtual void Delete(T*) = 0;

    virtual std::string GetName() = 0;
    virtual typename T::Metrics GetMetrics() = 0;
};

} } // Atlas::Stream

#endif
