// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Surface.h>

#include <Mercator/Shader.h>
#include <cassert>

namespace Mercator {

/// \brief Constructor
///
/// @param seg the terrain height segment this surface maps on to.
/// @param sh the shader used to generate the surface data.
/// @param color true if this shader contains color data.
/// @param alpha true if this shader contains alpha data.
Surface::Surface(const Segment & seg, const Shader & sh, bool color, bool alpha)
  : Buffer<ColorT>(seg, (color ? 3 : 0) + (alpha ? 1 : 0)), m_shader(sh)
{
}

Surface::~Surface()
{
}

/// \brief Populate the data buffer using the correct shader.
///
/// Call the shader to full this surface buffer with surface data.
void Surface::populate()
{
    if (!isValid()) {
        allocate();
    }

    m_shader.shade(*this);
}

} // namespace Mercator
