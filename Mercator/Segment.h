// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch, Damien McGinnes

#ifndef MERCATOR_SEGMENT_H
#define MERCATOR_SEGMENT_H

#include <Mercator/Mercator.h>
#include <Mercator/Matrix.h>
#include <Mercator/BasePoint.h>

#include <wfmath/vector.h>
#include <wfmath/axisbox.h>

#include <list>

namespace Mercator {

class Terrain;
class Surface;
class TerrainMod;
typedef std::list<TerrainMod *> ModList;

// This class will need to be reference counted if we want the code to
// be able to hold onto it, as currently they get deleted internally
// whenever height points are asserted.
class Segment {
  public:
    typedef std::list<Surface *> Surfacestore;
  private:
    /// Distance between segments
    const int m_res;
    /// Size of segment, m_res + 1
    const int m_size;
    /// Global x reference of this segment
    int m_xRef;
    /// Global y reference of this segment
    int m_yRef;
    /// 2x2 matrix of points which control this segment
    Matrix<2, 2, BasePoint> m_controlPoints;
    /// Pointer to buffer containing height points
    float * m_points;
    /// Pointer to buffer containing normals for height points
    float * m_normals;
    /// Pointer to buffer containing vertices
    float * m_vertices;
    /// Maximum height of any point in this segment
    float m_max;
    /// Minimum height of any point in this segment
    float m_min;
    /// Flag indicating whether the contains of this segment is valid
    bool m_validPt;
    /// Flag indicating whether the normals of this segment is valid
    bool m_validNorm;
    /// Flag indicating whether the vertices of this segment are valid
    bool m_validVert;
    /// Flag indicating whether the surfaces of this segment are valid
    bool m_validSurf;

    /// Store of surfaces which can be rendered on this terrain
    Surfacestore m_surfaces;

    void invalidate() {
        m_validPt = false;
        m_validNorm = false;
        m_validVert = false;
        m_validSurf = false;
    }
  public:
    explicit Segment(unsigned int resolution = defaultResolution);
    ~Segment();

    const int getResolution() const {
        return m_res;
    }

    const int getSize() const {
        return m_size;
    }

    const bool isValid() {
        return m_validPt;
    }

    const bool isVertexCacheValid() {
        return m_validVert;
    }

    void setVertexCacheValid(bool f = true) {
        m_validVert = true;
    }

    void setRef(int x, int y) {
        m_xRef=x*m_res;
        m_yRef=y*m_res;
    }

    void setCornerPoint(unsigned int x, unsigned int y, const BasePoint & bp) {
        m_controlPoints(x, y) = bp;
        invalidate();
    }
    
    const Matrix<2, 2, BasePoint> & getControlPoints() const {
        return m_controlPoints;
    }

    Matrix<2, 2, BasePoint> & getControlPoints() {
        return m_controlPoints;
    }

    const Surfacestore & getSurfaces() const {
        return m_surfaces;
    }

    Surfacestore & getSurfaces() {
        return m_surfaces;
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

    const float * getVertexCache() const {
        return m_vertices;
    }

    float * getVertexCache() {
        return m_vertices;
    }

    float * setVertexCache(float * v) {
        return m_vertices = v;
    }

    float get(int x, int y) const {
        return m_points[y * (m_res + 1) + x];
    }

    void getHeightAndNormal(float x, float y, float &h, 
                    WFMath::Vector<3> &normal) const;

    void populate();
    void populateNormals();
    void populateSurfaces();

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

    void fill1d(const BasePoint& l, const BasePoint &h, float *array) const;

    void fill2d(const BasePoint& p1, const BasePoint& p2, 
                const BasePoint& p3, const BasePoint& p4);

    float qRMD(float nn, float fn, float ff, float nf, 
               float roughness, float falloff, int depth) const;

    bool clipToSegment(const WFMath::AxisBox<2> &bbox, int &lx, int &hx, int &ly, int &hy);

    void applyMod(TerrainMod *t);

    ModList m_modList;

};

} // namespace Mercator

#endif // MERCATOR_SEGMENT_H
