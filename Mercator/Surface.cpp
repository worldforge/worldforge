// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Surface.h>

#include <Mercator/Shader.h>

namespace Mercator {

Surface::Surface(Segment & seg, const Shader & sh, bool color, bool alpha) :
    Buffer<ColorT>(seg, (color ? 3 : 0) + (alpha ? 1 : 0)), m_shader(sh)
{
}

Surface::~Surface()
{
}

void Surface::populate()
{
    if (m_shader.checkIntersect(*this)) {
        m_shader.shade(*this);
    }
}

} // namespace Mercator
