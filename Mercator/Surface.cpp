// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Surface.h>

#include <Mercator/Segment.h>

namespace Mercator {

Surface::Surface(Segment & segment) : m_segment(segment),
                 m_colors(new float[(segment.getResolution() + 1) *
                                    (segment.getResolution() + 1) * 4])
{
}

} // namespace Mercator
