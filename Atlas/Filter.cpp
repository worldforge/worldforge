// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#include "Filter.h"

Atlas::Filter::Filter(Atlas::Filter* next) : next(next)
{
}

Atlas::Filter::~Filter()
{
    delete next;
}
