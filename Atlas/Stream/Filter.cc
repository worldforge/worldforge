// This file may be redistributed and modified under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#include "Filter.h"

using namespace std;
using namespace Atlas::Stream;

list<Factory<Filter>*> Factory<Filter>::factories;

Atlas::Stream::Filter::Filter(Filter* next) : next(next)
{
}

Atlas::Stream::Filter::~Filter()
{
    delete next;
}
