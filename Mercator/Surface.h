// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_SURFACE_H
#define MERCATOR_SURFACE_H

#include <Mercator/Buffer.h>

namespace Mercator {

class Surface : public Buffer {
  public:
    explicit Surface(Segment & segment);
    virtual ~Surface();

    // Do we need an accessor presenting the array in colour form?
};

} // namespace Mercator

#endif // MERCATOR_SURFACE_H
