// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_SHADER_H
#define MERCATOR_SHADER_H

namespace Mercator {

class Segment;

// FIXME - PLACEHOLDER
// This class itereates over the buffer in a segment using the data it contains
// to populate a RGBA colour buffer based on the terrain height data. ie it is
// used to define the texture blending for a given surface
class Shader {
  private:
    Segment & m_segment;

  public:
    explicit Shader(Segment & segment, unsigned int channels = 4);
    virtual ~Shader();
};

} // namespace Mercator

#endif // MERCATOR_SHADER_H
