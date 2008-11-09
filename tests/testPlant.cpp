// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include <Mercator/Plant.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#define DEBUG

#include <cassert>

int main()
{
    {
        Mercator::Plant a, b;

        const WFMath::Point<2> & p1 = a.getDisplacement();
        assert(!p1.isValid());
        WFMath::Point<2> p2 = a.getDisplacement();
        assert(!p2.isValid());
        a.setDisplacement(WFMath::Point<2>(2.5f, 3.f));
        const WFMath::Point<2> & p3 = a.getDisplacement();
        assert(p3.isValid());

        const WFMath::Quaternion & q1 = b.getOrientation();
        assert(!q1.isValid());
        WFMath::Quaternion q2 = b.getOrientation();
        assert(!q2.isValid());
        b.setOrientation(WFMath::Quaternion(2, 2.124f));
        const WFMath::Quaternion & q3 = b.getOrientation();
        assert(q3.isValid());

        Mercator::Plant * c = new Mercator::Plant();
        c->setHeight(5.5f);
        delete c;

        Mercator::Plant * d = new Mercator::Plant[10];
        d->setHeight(15.5f);
        delete [] d;
    }
}
