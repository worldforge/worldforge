// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Damien McGinnes

#ifndef MERCATOR_POINT_H
#define MERCATOR_POINT_H

#include <iostream>

namespace Mercator { 

class BasePoint {
	
  public:
    const static float ROUGHNESS = 1.15;
    const static float FALLOFF = 0.15;
    const static float HEIGHT = 8.0;

    float height;
    float roughness;
    float falloff;
    BasePoint(float h, float r, float f) : height(h), roughness(r), falloff(f) {}
    BasePoint(float h, float r) : height(h), roughness(r), falloff(FALLOFF) {}
    BasePoint(float h) : height(h), roughness(ROUGHNESS), falloff(FALLOFF) {}
    BasePoint() : height(HEIGHT), roughness(ROUGHNESS), falloff(FALLOFF) {}

    int seed() const { return (int)(height * 10.0);}
};

} //namespace Mercator

#endif // MERCATOR_POINT_H
