// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Plant.h>

#include <cassert>

int main()
{
    {
        Mercator::Plant a, b;

        const WFMath::Point<2> & p1 = a.getDisplacement();
        assert(!p1.isValid());
        WFMath::Point<2> p2 = a.getDisplacement();
        assert(!p2.isValid());

        const WFMath::Quaternion & q1 = b.getOrientation();
        assert(!q1.isValid());
        WFMath::Quaternion q2 = b.getOrientation();
        assert(!q2.isValid());

        Mercator::Plant * c = new Mercator::Plant();
        delete c;

        Mercator::Plant * d = new Mercator::Plant[10];
        delete [] d;
    }
}
