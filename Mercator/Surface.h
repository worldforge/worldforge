// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_SURFACE_H
#define MERCATOR_SURFACE_H

#include <Mercator/Mercator.h>

namespace Mercator {

    class Segment;

    class Surface {
      public:
        typedef enum orient { SURFACE_XY, SURFACE_XZ, SURFACE_YZ } Orientation;
      private:
        Segment & m_segment;
        float * const m_colors;

      public:
        explicit Surface(Segment & segment);
        ~Surface();

        float * getColors() {
            return m_colors;
        }

    };

} // namespace Mercator

#endif // MERCATOR_SURFACE_H
