// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_SURFACE_H
#define MERCATOR_SURFACE_H

#include "Buffer.h"
#include "Segment.h"

#include <climits>

namespace Mercator {

class Shader;

typedef unsigned char ColorT;

static const ColorT colorMax = UCHAR_MAX;
static const ColorT colorMin = 0;

/// \brief Data store for terrain surface data.
class Surface : public Buffer<ColorT> {
  public:
    /// \brief The shader that populates this surface.
    const Shader & m_shader;
    /// The terrain height segment this buffer is associated with.
    const Segment & m_segment;

    explicit Surface(const Segment & segment, const Shader & shader,
                     bool colors = true, bool alpha = true);
    virtual ~Surface() = default;

    void populate();

    /// Accessor for the terrain height segment this surface is associated with.
    const Segment & getSegment() const {
        return m_segment;
    }
    // Do we need an accessor presenting the array in colour form?
};

} // namespace Mercator

#endif // MERCATOR_SURFACE_H
