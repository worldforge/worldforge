// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_SURFACE_H
#define MERCATOR_SURFACE_H

#include <Mercator/Buffer.h>

namespace Mercator {

class Shader;

typedef unsigned char ColorT;

static const ColorT colorMax = 255;
static const ColorT colorMin = 0;

class Surface : public Buffer<ColorT> {
  private:
    const Shader & m_shader;
  public:
    explicit Surface(Segment & segment, const Shader & shader,
                     bool colors = true, bool alpha = true);
    virtual ~Surface();

    void populate();
    // Do we need an accessor presenting the array in colour form?
};

} // namespace Mercator

#endif // MERCATOR_SURFACE_H
