// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_FILL_SHADER_H
#define MERCATOR_FILL_SHADER_H

#include <Mercator/Shader.h>

namespace Mercator {

class FillShader {
  public:
    explicit FillShader();
    virtual ~FillShader();

    virtual void shade(Surface &);
};

} // namespace Mercator

#endif // MERCATOR_FILL_SHADER_H
