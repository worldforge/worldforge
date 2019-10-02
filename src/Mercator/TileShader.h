// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef MERCATOR_TILE_SHADER_H
#define MERCATOR_TILE_SHADER_H

#include "Shader.h"

#include <map>

namespace Mercator {

/// \brief Shader agregating surface data.
///
/// Some applications require the terrain surface data be stored in a
/// single buffer so specific locations can be queried to determine the type.
/// Typically this is used on a server, where lots of surface data optimised
/// for rendering is not required, but fast cheap queries about the surface
/// at a specific point are required.
class TileShader : public Shader {
  public:
    /// \brief STL map to store sparse array of Shader pointers.
    typedef std::map<int, Shader *> Shaderstore;
  private:
    /// \brief Store of shaders which are agregated by this shader.
    Shaderstore m_subShaders;
  public:
    explicit TileShader();

    ~TileShader() override;

    /// \brief Add a shader to those agregated by the tile shader.
    void addShader(Shader * t, int id) {
        m_subShaders[id] = t;
    }

    bool checkIntersect(const Segment &) const override;

    void shade(Surface &) const override;
};

} // namespace Mercator

#endif // MERCATOR_TILE_SHADER_H
