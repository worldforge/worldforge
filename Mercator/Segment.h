// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_SEGMENT_H
#define MERCATOR_SEGMENT_H

#include <Mercator/Matrix.h>
#include <wfmath/vector.h>
#include <wfmath/axisbox.h>
#include <list>

namespace Mercator {

class TerrainMod;
typedef std::list<TerrainMod *> ModList;

// This class will need to be reference counted if we want the code to
// be able to hold onto it, as currently they get deleted internally
// whenever height points are asserted.
class Segment {
  private:
    int m_res;
    float * const m_points;
    float * m_normals;
    float m_max;
    float m_min;
  public:
    explicit Segment(int res = 64);
    ~Segment();

    const int getSize() const {
        return m_res;
    }
    
    const float * getPoints() const {
        return m_points;
    }

    float * getPoints() {
        return m_points;
    }

    const float * getNormals() const {
        return m_normals;
    }

    float * getNormals() {
        return m_normals;
    }

    float get(int x, int y) const {
        return m_points[y * (m_res + 1) + x];
    }

    void getHeightAndNormal(float x, float y, float &h, 
                    WFMath::Vector<3> &normal) const;

    void populate(const Matrix<2,2> &);
    void populateNormals();

    float getMax() const { return m_max; }
    float getMin() const { return m_min; }

    void addMod(TerrainMod *t);
    void clearMods();
    
  private:
    void checkMaxMin(float h) { 
        if (h<m_min) {
            m_min=h;
        }
        if (h>m_max) {
            m_max=h;
        }
    } 

    void fill1d(int size, float falloff, float roughness, 
                float l, float h, float *array) const;

    void fill2d(int size, float falloff, float roughness,
                float p1, float p2, float p3, float p4);
        
    float qRMD(float nn, float fn, float ff, float nf, 
               float roughness, float falloff, int depth) const;

    bool clipToSegment(const WFMath::AxisBox<2> &bbox, int &lx, int &hx, int &ly, int &hy);

    void applyMod(TerrainMod *t);

    ModList m_modList;
};

} // namespace Mercator

#endif // MERCATOR_SEGMENT_H
