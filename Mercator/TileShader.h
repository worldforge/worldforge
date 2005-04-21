// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef MERCATOR_TILE_SHADER_H
#define MERCATOR_TILE_SHADER_H

#include <Mercator/Shader.h>

#include <map>

namespace Mercator {

class TileShader : public Shader {
  public:
    /// \brief STL map to store sparse array of Shader pointers.
    typedef std::map<int, Shader *> Shaderstore;
  private:
    Shaderstore m_subShaders;
  public:
    explicit TileShader();
    virtual ~TileShader();

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

} // namespace Mercator

#endif // MERCATOR_TILE_SHADER_H
