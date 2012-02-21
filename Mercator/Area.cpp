// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Mercator/Area.h>
#include "Mercator/Segment.h"

#include <wfmath/intersect.h>
#include <iostream>
#include <cmath>

#include <cassert>

using WFMath::CoordType;

namespace Mercator
{

typedef WFMath::Point<2> Point2;
typedef WFMath::Vector<2> Vector2;

#ifndef NDEBUG
static bool isZero(CoordType d)
{
    return (std::fabs(d) < WFMATH_EPSILON);
}
#endif

/// \brief Helper to clip points to a given range.
class TopClip
{
public:
    /// Constructor
    ///
    /// @param t top of y range
    TopClip(CoordType t) : topY(t) { }
    
    /// \brief Check a point is outside this clip.
    ///
    /// @param p point to be checked.
    /// @return true if p is outside the clip.
    bool inside(const Point2& p) const
    {
        return p.y() >= topY;
    }

    /// \brief Determine the point where a line crosses this clip.
    ///
    /// @param u one of of a line that crosses this clip
    /// @param v one of of a line that crosses this clip
    /// @return a point where the line cross this clip.
    Point2 clip(const Point2& u, const Point2& v) const
    {
        CoordType dy = v.y() - u.y();
        CoordType dx = v.x() - u.x();
        
        // shouldn't every happen - if dy iz zero, the line is horizontal,
        // so either both points should be inside, or both points should be
        // outside. In either case, we should not call clip()
        assert(!isZero(dy));
        
        CoordType t = (topY - u.y()) / dy;
        return Point2(u.x() + t * dx, topY);
    }
private:
    /// \brief Top of y range.
    CoordType topY;
};

/// \brief Helper to clip points to a given range.
class BottomClip
{
public:
    /// Constructor
    ///
    /// @param t bottom of y range
    BottomClip(CoordType t) : bottomY(t) { }
    
    /// \brief Check a point is outside this clip.
    ///
    /// @param p point to be checked.
    /// @return true if p is outside the clip.
    bool inside(const Point2& p) const
    {
        return p.y() < bottomY;
    }

    /// \brief Determine the point where a line crosses this clip.
    ///
    /// @param u one of of a line that crosses this clip
    /// @param v one of of a line that crosses this clip
    /// @return a point where the line cross this clip.
    Point2 clip(const Point2& u, const Point2& v) const
    {
        CoordType dy = v.y() - u.y();
        CoordType dx = v.x() - u.x();
        assert(!isZero(dy));
        
        CoordType t = (u.y() - bottomY) / -dy;
        return Point2(u.x() + t * dx, bottomY);
    }
private:
    /// \brief Bottom of y range.
    CoordType bottomY;
};

/// \brief Helper to clip points to a given range.
class LeftClip
{
public:
    /// Constructor
    ///
    /// @param t left of x range.
    LeftClip(CoordType t) : leftX(t) { }
    
    /// \brief Check a point is outside this clip.
    ///
    /// @param p point to be checked.
    /// @return true if p is outside the clip.
    bool inside(const Point2& p) const
    {
        return p.x() >= leftX;
    }

    /// \brief Determine the point where a line crosses this clip.
    ///
    /// @param u one of of a line that crosses this clip
    /// @param v one of of a line that crosses this clip
    /// @return a point where the line cross this clip.
    Point2 clip(const Point2& u, const Point2& v) const
    {
        CoordType dy = v.y() - u.y();
        CoordType dx = v.x() - u.x();
        
        // shouldn't every happen
        assert(!isZero(dx));
        
        CoordType t = (leftX - u.x()) / dx;
        return Point2(leftX, u.y() + t * dy);
    }
private:
    /// \brief Left of x range.
    CoordType leftX;
};

/// \brief Helper to clip points to a given range.
class RightClip
{
public:
    /// Constructor
    ///
    /// @param t right of x range.
    RightClip(CoordType t) : rightX(t) { }
    
    /// \brief Check a point is outside this clip.
    ///
    /// @param p point to be checked.
    /// @return true if p is outside the clip.
    bool inside(const Point2& p) const
    {
        return p.x() < rightX;
    }

    /// \brief Determine the point where a line crosses this clip.
    ///
    /// @param u one of of a line that crosses this clip
    /// @param v one of of a line that crosses this clip
    /// @return a point where the line cross this clip.
    Point2 clip(const Point2& u, const Point2& v) const
    {
        CoordType dy = v.y() - u.y();
        CoordType dx = v.x() - u.x();
        
        // shouldn't every happen
        assert(!isZero(dx));
        
        CoordType t = (u.x() - rightX) / -dx;
        return Point2(rightX, u.y() + t * dy);
    }
private:
    /// \brief Right of x range.
    CoordType rightX;
};

template <class Clip>
WFMath::Polygon<2> sutherlandHodgmanKernel(const WFMath::Polygon<2>& inpoly, Clip clipper)
{
    WFMath::Polygon<2> outpoly;
    
    if (!inpoly.isValid()) return inpoly;
    std::size_t points = inpoly.numCorners();
    if (points < 3) return outpoly; // i.e an invalid result
    
    Point2 lastPt = inpoly.getCorner(points - 1);
    bool lastInside = clipper.inside(lastPt);
    
    for (std::size_t p = 0; p < points; ++p) {
    
        Point2 curPt = inpoly.getCorner(p);
        bool inside = clipper.inside(curPt);
        
        if (lastInside) {
            if (inside) {
                // emit curPt
                outpoly.addCorner(outpoly.numCorners(), curPt);
            } else {
                // emit intersection of edge with clip line
                outpoly.addCorner(outpoly.numCorners(), clipper.clip(lastPt, curPt));
            }
        } else {
            if (inside) {
                // emit both
                outpoly.addCorner(outpoly.numCorners(), clipper.clip(lastPt, curPt));
                outpoly.addCorner(outpoly.numCorners(), curPt);
            } else {
                // don't emit anything
            }
        } // last was outside
        
        lastPt = curPt;
        lastInside = inside;
    }
    
    return outpoly;
}

Area::Area(int layer, bool hole) :
    m_layer(layer),
    m_hole(hole),
    m_shader(0)
{
}

void Area::setShape(const WFMath::Polygon<2>& p)
{
    assert(p.isValid());
    m_shape = p;
    m_box = p.boundingBox();
}

void Area::setShader(const Shader * shader) const
{
    m_shader = shader;
}

bool Area::contains(CoordType x, CoordType y) const
{
    if (!WFMath::Contains(m_box, Point2(x,y), false)) return false;
    
    return WFMath::Contains(m_shape, Point2(x,y), false);
}

int Area::addToSegment(Segment & s) const
{
    if (!checkIntersects(s)) {
        return -1;
    }
    return s.addArea(this);
}

void Area::updateToSegment(Segment & s) const
{
    if (!checkIntersects(s)) {
        s.removeArea(this);
        return;
    }
    if (s.updateArea(this) != 0) {
        s.addArea(this);
    }
}

void Area::removeFromSegment(Segment & s) const
{
    if (checkIntersects(s)) {
        s.removeArea(this);
    }
}

WFMath::Polygon<2> Area::clipToSegment(const Segment& s) const
{
    // box reject
    if (!checkIntersects(s)) return WFMath::Polygon<2>();
    
    WFMath::AxisBox<2> segBox(s.getRect());
    WFMath::Polygon<2> clipped = sutherlandHodgmanKernel(m_shape, TopClip(segBox.lowCorner().y()));
    
    clipped = sutherlandHodgmanKernel(clipped, BottomClip(segBox.highCorner().y()));
    clipped = sutherlandHodgmanKernel(clipped, LeftClip(segBox.lowCorner().x()));
    clipped = sutherlandHodgmanKernel(clipped, RightClip(segBox.highCorner().x()));
    
    return clipped;
}

bool Area::checkIntersects(const Segment& s) const
{
    return WFMath::Intersect(m_shape, s.getRect(), false) ||
        WFMath::Contains(s.getRect(), m_shape.getCorner(0), false);
}

} // of namespace
