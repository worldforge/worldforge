// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_FILL_SHADER_H
#define MERCATOR_FILL_SHADER_H

#include <Mercator/Shader.h>

namespace Mercator {

class FillShader : public Shader {
  public:
    explicit FillShader();
    explicit FillShader(const Parameters & params);
    virtual ~FillShader();

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

} // namespace Mercator

#endif // MERCATOR_FILL_SHADER_H
