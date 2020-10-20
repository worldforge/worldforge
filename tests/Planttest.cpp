// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include <Mercator/Plant.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <cassert>

int main()
{
    {
        Mercator::Plant a, b;

        const WFMath::Point<2> & p1 = a.m_displacement;
        assert(!p1.isValid());
        WFMath::Point<2> p2 = a.m_displacement;
        assert(!p2.isValid());
        a.m_displacement = WFMath::Point<2>(2.5f, 3.f);
        const WFMath::Point<2> & p3 = a.m_displacement;
        assert(p3.isValid());

        const WFMath::Quaternion & q1 = b.m_orientation;
        assert(!q1.isValid());
        WFMath::Quaternion q2 = b.m_orientation;
        assert(!q2.isValid());
        b.m_orientation = WFMath::Quaternion(2, 2.124f);
        const WFMath::Quaternion & q3 = b.m_orientation;
        assert(q3.isValid());

        Mercator::Plant * c = new Mercator::Plant();
        c->m_height = 5.5f;
        delete c;

        Mercator::Plant * d = new Mercator::Plant[10];
        d->m_height = 15.5f;
        delete [] d;
    }
}
