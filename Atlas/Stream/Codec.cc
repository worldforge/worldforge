// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#include "Codec.h"

#include <list>

std::list<Atlas::Factory<Codec>*> factories;

template <typename T>
Atlas::Codec::Factory<T>::Factory(
    const std::string& name,
    const std::string& version)
{
    factories.push_back(this);
}

template <typename T>
Atlas::Codec::Factory<T>::~Factory()
{
}
