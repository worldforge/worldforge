// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_SURFACE_H
#define MERCATOR_SURFACE_H

#include <Mercator/Mercator.h>

namespace Mercator {

    class Surface {
      private:
        const int m_res;
        float * const m_colors;

      public:
        explicit Surface(unsigned int resolution = defaultResolution);
        ~Surface();

    };

} // namespace Mercator

#endif // MERCATOR_SURFACE_H
