// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Surface.h>

namespace Mercator {

Surface::Surface(unsigned int resolution) : m_res(resolution),
                 m_colors(new float[(m_res + 1) * (m_res + 1) * 4])
{
}

} // namespace Mercator
