// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Alistair Riddoch

#include <Mercator/Buffer.h>
#include <Mercator/Segment.h>

int main()
{
    Mercator::Segment s(0,0,64);
    Mercator::Buffer<float> b(s);

    return 0;
}
