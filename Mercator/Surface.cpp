// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Surface.h>

#include <Mercator/Shader.h>

namespace Mercator {

Surface::Surface(Segment & seg, Shader & sh) : Buffer(seg, 4), m_shader(sh)
{
}

Surface::~Surface()
{
}

void Surface::populate()
{
    m_shader.shade(*this);
}

} // namespace Mercator
