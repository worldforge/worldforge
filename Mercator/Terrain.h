// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch, Damien McGinnes

#ifndef MERCATOR_TERRAIN_H
#define MERCATOR_TERRAIN_H

#include <Mercator/Mercator.h>
#include <Mercator/BasePoint.h>

#include <map>
#include <list>
#include <cmath>

namespace Mercator {

class Segment;
class Shader;
class TerrainMod;

class Terrain {
  public:
    typedef std::map<int, BasePoint> Pointcolumn;
    typedef std::map<int, Segment *> Segmentcolumn;

    typedef std::map<int, Pointcolumn > Pointstore;
    typedef std::map<int, Segmentcolumn > Segmentstore;
    typedef std::list<Shader *> Shaderstore;
  private:
    const int m_res;

    Pointstore m_basePoints;
    Segmentstore m_segments;
    Shaderstore m_shaders;
  public:
    static const float defaultLevel = 8;
    explicit Terrain(unsigned int resolution = defaultResolution);
    ~Terrain();

    float get(float x, float y) const;

    bool getBasePoint(int x, int y, BasePoint& z) const;
    void setBasePoint(int x, int y, const BasePoint& z);

    void setBasePoint(int x, int y, float z) {
        BasePoint bp(z);
        setBasePoint(x, y, bp);
    }

    Segment * getSegment(float x, float y) const {
        int ix = (int)floor(x / m_res);
        int iy = (int)floor(y / m_res);
        return getSegment(ix, iy);
    }

    Segment * getSegment(int x, int y) const;

    const int getResolution() const {
        return m_res;
    }

    const Segmentstore & getTerrain() const {
        return m_segments;
    }

    const Pointstore & getPoints() const {
        return m_basePoints;
    }
    
    const Shaderstore & getShaders() const {
        return m_shaders;
    }

    void addShader(Shader * t) {
        m_shaders.push_back(t);
    }

    void addMod(const TerrainMod &t);
};

} // namespace Mercator

#endif // MERCATOR_TERRAIN_H
