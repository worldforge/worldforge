// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Shader.h>

#include <Mercator/Segment.h>

namespace Mercator {

Shader::Shader(Segment & segment, unsigned int channels) : m_segment(segment)
{
}

Shader::~Shader()
{
}

} // namespace Mercator
