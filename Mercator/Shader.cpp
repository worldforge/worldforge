// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Shader.h>

#include <Mercator/Segment.h>
#include <Mercator/Surface.h>

namespace Mercator {

Shader::Shader(bool color, bool alpha) : m_color(color), m_alpha(alpha)
{
}

Shader::~Shader()
{
}

Surface * Shader::newSurface(Segment & segment) const
{
    return new Surface(segment, *this, m_color, m_alpha);
}

} // namespace Mercator
