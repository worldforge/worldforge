// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_FILL_GRASS_SHADER_H
#define MERCATOR_FILL_GRASS_SHADER_H

#include <Mercator/Shader.h>
#include <Mercator/Surface.h>

/* alpha ^
 *       |
 *       |_______________________ cutoff
 *       |                       \ 
 *       |                        \ 
 *       |                         \ 
 *       |                          \ 
 *       |                           \ 
 *       |                            \ 
 *       |                             \ 
 *       |                              \ 
 *       |                               \ 
 *       |                                \  intercept
 *       |_________________________________\_________________________> slope
 */

namespace Mercator {

class GrassShader : public Shader {
  private:
    float m_lowThreshold;
    float m_highThreshold;
    float m_cutoff;
    float m_intercept;

    ColorT slopeToAlpha(float height, float slope) const;
  public:
    explicit GrassShader(float lowThreshold = 1.f, float highThreshold = 20.f,
                         float cutoff = 1.f, float intercept = 2.f);
    virtual ~GrassShader();

    virtual bool checkIntersect(const Segment &) const;
    virtual void shade(Surface &) const;
};

} // namespace Mercator

#endif // MERCATOR_FILL_GRASS_SHADER_H
