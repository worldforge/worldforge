#include <Mercator/Area.h>
#include "Mercator/Segment.h"

#include <wfmath/intersect.h>
#include <iostream>

namespace Mercator
{

typedef WFMath::Point<2> Point2;
typedef WFMath::Vector<2> Vector2;

bool isZero(double d)
{
    return (fabs(d) < WFMATH_EPSILON);
}

class TopClip
{
public:
    TopClip(double t) : topY(t) { }
    
    bool inside(const Point2& p) const
    {
        return p.y() >= topY;
    }

    Point2 clip(const Point2& u, const Point2& v) const
    {
        double dy = v.y() - u.y();
        double dx = v.x() - u.x();
        
        // shouldn't every happen - if dy iz zero, the line is horizontal,
        // so either both points should be inside, or both points should be
        // outside. In either case, we should not call clip()
        assert(!isZero(dy));
        
        double t = (topY - u.y()) / dy;
        return Point2(u.x() + t * dx, topY);
    }
private:
    double topY;
};

class BottomClip
{
public:
    BottomClip(double t) : bottomY(t) { }
    
    bool inside(const Point2& p) const
    {
        return p.y() < bottomY;
    }

    Point2 clip(const Point2& u, const Point2& v) const
    {
        double dy = v.y() - u.y();
        double dx = v.x() - u.x();
        assert(!isZero(dy));
        
        double t = (u.y() - bottomY) / -dy;
        return Point2(u.x() + t * dx, bottomY);
    }
private:
    double bottomY;
};

class LeftClip
{
public:
    LeftClip(double t) : leftX(t) { }
    
    bool inside(const Point2& p) const
    {
        return p.x() >= leftX;
    }

    Point2 clip(const Point2& u, const Point2& v) const
    {
        double dy = v.y() - u.y();
        double dx = v.x() - u.x();
        
        // shouldn't every happen
        assert(!isZero(dx));
        
        double t = (leftX - u.x()) / dx;
        return Point2(leftX, u.y() + t * dy);
    }
private:
    double leftX;
};

class RightClip
{
public:
    RightClip(double t) : rightX(t) { }
    
    bool inside(const Point2& p) const
    {
        return p.x() < rightX;
    }

    Point2 clip(const Point2& u, const Point2& v) const
    {
        double dy = v.y() - u.y();
        double dx = v.x() - u.x();
        
        // shouldn't every happen
        assert(!isZero(dx));
        
        double t = (u.x() - rightX) / -dx;
        return Point2(rightX, u.y() + t * dy);
    }
private:
    double rightX;
};

template <class Clip>
WFMath::Polygon<2> sutherlandHodgmanKernel(const WFMath::Polygon<2>& inpoly, Clip clipper)
{
    WFMath::Polygon<2> outpoly;
    
    if (!inpoly.isValid()) return inpoly;
    int points = inpoly.numCorners();
    if (points < 3) return outpoly; // i.e an invalid result
    
    Point2 lastPt = inpoly.getCorner(points - 1);
    bool lastInside = clipper.inside(lastPt);
    
    for (int p=0; p < points; ++p) {
    
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

#pragma mark -

Area::Area(int l, bool hole) :
    m_layer(l),
    m_hole(hole)
{
}

void Area::setShape(const WFMath::Polygon<2>& p)
{
    assert(p.isValid());
    m_shape = p;
    m_box = p.boundingBox();
}

bool Area::contains(double x, double y) const
{
    if (!WFMath::Contains(m_box, Point2(x,y), false)) return false;
    
    return WFMath::Contains(m_shape, Point2(x,y), false);
}

WFMath::Polygon<2> Area::clipToSegment(Segment& s) const
{
    // box reject
    if (!checkIntersects(s)) return WFMath::Polygon<2>();
    
    WFMath::AxisBox<2> segBox(s.getBox());
    WFMath::Polygon<2> clipped = sutherlandHodgmanKernel(m_shape, TopClip(segBox.lowCorner().y()));
    
 //   std::cout << "top clipped: y=" << segBox.lowCorner().y() << std::endl;
 //   for (unsigned int i=0; i<clipped.numCorners(); ++i) {
  //      std::cout << '\t' << clipped[i] << std::endl;
 //   }
  
    clipped = sutherlandHodgmanKernel(clipped, BottomClip(segBox.highCorner().y()));
    
 //   std::cout << "bottom clipped: y=" << segBox.highCorner().y() << std::endl;
//    for (unsigned int i=0; i<clipped.numCorners(); ++i) {
 //       std::cout << '\t' << clipped[i] << std::endl;
  //  }
  
    clipped = sutherlandHodgmanKernel(clipped, LeftClip(segBox.lowCorner().x()));
    
  //  std::cout << "left clipped: x=" << segBox.lowCorner().x() << std::endl;
  //  for (unsigned int i=0; i<clipped.numCorners(); ++i) {
   //     std::cout << '\t' << clipped[i] << std::endl;
  //  }
    
    clipped = sutherlandHodgmanKernel(clipped, RightClip(segBox.highCorner().x()));
    
 //   std::cout << "right clipped: x=" << segBox.highCorner().x() << std::endl;
 //   for (unsigned int i=0; i<clipped.numCorners(); ++i) {
 //       std::cout << '\t' << clipped[i] << std::endl;
  //  }
    
    return clipped;
}

bool Area::checkIntersects(const Segment& s) const
{
    return WFMath::Intersect(m_shape, s.getBox(), false);
}


} // of namespace
