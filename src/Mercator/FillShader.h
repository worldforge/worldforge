// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_FILL_SHADER_H
#define MERCATOR_FILL_SHADER_H

#include "Shader.h"

namespace Mercator {

/// \brief Shader that fills the surface.
///
/// This shader is typically only used for the lowest possible layer,
/// which is usually bed rock.
class FillShader : public Shader {
  public:
    /// \brief Constructor
    explicit FillShader();
    /// \brief Constructor
    ///
    /// @param params a map of parameters for the shader.
    explicit FillShader(const Parameters & params);
    virtual ~FillShader();

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

} // namespace Mercator

#endif // MERCATOR_FILL_SHADER_H
