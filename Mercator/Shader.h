// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_SHADER_H
#define MERCATOR_SHADER_H

namespace Mercator {

class Surface;
class Segment;

// FIXME - PLACEHOLDER
// This class itereates over the buffer in a segment using the data it contains
// to populate a RGBA colour buffer based on the terrain height data. ie it is
// used to define the texture blending for a given surface
class Shader {
  private:
    bool m_color;
    bool m_alpha;
  protected:
    explicit Shader(bool color = false, bool alpha = true);
  public:
    virtual ~Shader();

    bool getColor() const {
        return m_color;
    }

    bool getAlpha() const {
        return m_alpha;
    }

    Surface * newSurface(Segment &) const;

    virtual bool checkIntersect(Surface &) const = 0;
    virtual void shade(Surface &) const = 0;
};

} // namespace Mercator

#endif // MERCATOR_SHADER_H
