// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Mercator/AreaShader.h"
#include "Mercator/Area.h"
#include "Mercator/iround.h"
#include "Mercator/Segment.h"
#include "Mercator/Surface.h"

#include <list>
#include <set>
#include <iostream>
#include <algorithm>
#include <cmath>

#include <cassert>

namespace Mercator
{

typedef WFMath::Point<2> Point2;
typedef WFMath::Vector<2> Vector2;

const double ROW_HEIGHT = 1 / 4.0; // 4x over-sample

/// \brief The edge of an area.
class Edge
{
public: 
    /// \brief Constructor
    ///
    /// @param a one end of the line defining the edge.
    /// @param b one end of the line defining the edge.
    Edge(const Point2& a, const Point2& b)
    {
        // horizontal segments should be discarded earlier
        assert(a.y() != b.y());
        
        if (a.y() < b.y()) {
            m_start = a;
            m_seg = b - a;
        } else {
            m_start = b;
            m_seg = a - b;
        }
        
        // normal gradient is y/x, here we use x/y. seg.y() will be != 0,
        // as we already asserted above.
        m_inverseGradient = m_seg.x() / m_seg.y();
    }
    
    /// Accessor for the point describing the start of the edge.
    Point2 start() const { return m_start; }
    /// Determine the point describing the end of the edge.
    Point2 end() const { return m_start + m_seg; }
    
    /// \brief Determine the x coordinate at a given y coordinate.
    ///
    /// Calculate the x coordinate on the edge line where the y coordinate
    /// is the value specified.
    /// @param y the y coordinate where the calculation is required.
    double xValueAtY(double y) const
    {
        double x = m_start.x() + ((y - m_start.y()) * m_inverseGradient);
     //   std::cout << "edge (" << m_start << ", " << m_start + m_seg << ") at y=" << y << " has x=" << x << std::endl; 
        return x;
    }
    
    /// \brief Compare the y coordinate of the start with another edge.
    ///
    /// This operator ensures that edges can be sorted, compares the y
    /// y coordinate of the start of the edges.
    bool operator<(const Edge& other) const
    {
        return m_start.y() < other.m_start.y();
    }
private:
    /// The point describing the start of the edge.
    Point2 m_start;
    /// The vector describing the edge from its start.
    Vector2 m_seg;
    /// The inverse of the gradient of the line.
    double m_inverseGradient;
};

/// \brief The edge of an area parallel to the x axis.
class EdgeAtY
{
public:
    /// Constructor
    ///
    /// @param y coordinate on the y axis of the edge.
    EdgeAtY(double y) : m_y(y) {}
    
    /// Determine which edge crosses this edge at a lower x coordinate.
    bool operator()(const Edge& u, const Edge& v) const
    {
        return u.xValueAtY(m_y) < v.xValueAtY(m_y);
    }
private:
    /// The coordinate on the y axis of the edge.
    double m_y;
};

void contribute(Surface& s, unsigned int x, unsigned int y, double amount)
{    
    unsigned int sz = s.getSize() - 1;
    if ((x == 0) || (x == sz))
        amount *= 2;
        
    if ((y == 0) || (y == sz))
        amount *= 2;
        
    s(x, y, 0) = std::min( static_cast<ColorT>(I_ROUND(amount * 255)) + s(x,y,0), 255);
}

void span(Surface& s, double y, double xStart, double xEnd)
{
    assert(xStart <= xEnd); 

    // quantize and accumulate into the buffer data
    unsigned int row = I_ROUND(y),
        ixStart = I_ROUND(xStart),
        ixEnd = I_ROUND(xEnd);
 
    //std::cout << "span @ y=" << row << ", " << ixStart << " -> " << ixEnd << std::endl;
    
    if (ixStart == ixEnd) {
        contribute(s, ixStart, row, ROW_HEIGHT * (xEnd - xStart));
    } else {
        contribute(s, ixStart, row, ROW_HEIGHT * (ixStart - xStart + 0.5));
        
        for (unsigned int i=ixStart+1; i < ixEnd; ++i)
            contribute(s, i, row, ROW_HEIGHT);
        
        contribute(s, ixEnd, row, ROW_HEIGHT * (xEnd - ixEnd + 0.5));
    }
}

void scanConvert(const WFMath::Polygon<2>& inPoly, Surface& sf)
{
    if (!inPoly.isValid()) return;
    
    std::list<Edge> pending;
    std::vector<Edge> active;

    Point2 lastPt = inPoly.getCorner(inPoly.numCorners() - 1);
    for (int p=0; p < inPoly.numCorners(); ++p) {
        Point2 curPt = inPoly.getCorner(p);
        
        // skip horizontal edges
        if (curPt.y() != lastPt.y())
            pending.push_back(Edge(lastPt, curPt));
        
        lastPt = curPt;
    }
    
    if (pending.empty()) return;
    
    // sort edges by starting (lowest) y value
    pending.sort();
    active.push_back(pending.front());
    pending.pop_front();
    
    // advance to the row of the first y value, and ensure y sits in the
    // middle of sample rows - we do this by offseting by 1/2 a row height
    // if you don't do this, you'll find alternating rows are over/under
    // sampled, producing a charming striped effect.
    double y = std::floor(active.front().start().y()) + ROW_HEIGHT * 0.5;
    
    for (; !pending.empty() || !active.empty();  y += ROW_HEIGHT)
    {
        while (!pending.empty() && (pending.front().start().y() <= y)) {
            active.push_back(pending.front());
            pending.pop_front();
        }
        
        // sort by x value - note active will be close to sorted anyway
        std::sort(active.begin(), active.end(), EdgeAtY(y));
        
        // delete finished edges
        for (unsigned int i=0; i< active.size(); ) {
            if (active[i].end().y() <= y)
                active.erase(active.begin() + i);
            else
                ++i;
        }
        
        // draw pairs of active edges
        for (unsigned int i=1; i < active.size(); i += 2)
            span(sf, y, active[i-1].xValueAtY(y), active[i].xValueAtY(y));
    } // of active edges loop
}

AreaShader::AreaShader(int layer) :
    Shader(false /* no color */, true),
    m_layer(layer)
{

}

bool AreaShader::checkIntersect(const Segment& s) const
{
    const Segment::Areastore& areas(s.getAreas());
    return (areas.count(m_layer) > 0);
}

void AreaShader::shade(Surface &s) const
{
    ColorT * data = s.getData();
    unsigned int size = s.getSegment().getSize();

    unsigned int buflen = size * size;
    for (unsigned int i = 0; i < buflen; ++i) data[i] = 0;

    const Segment::Areastore& areas(s.m_segment.getAreas());
    Segment::Areastore::const_iterator it = areas.lower_bound(m_layer);
    Segment::Areastore::const_iterator itend = areas.upper_bound(m_layer);
    
    for (;it != itend; ++it) {
        // apply to surface in turn
        if (it->second->isHole()) {
            // shadeHole
        } else
            shadeArea(s, it->second);
    } // of areas in layer
}

void AreaShader::shadeArea(Surface& s, const Area* const ar) const
{
    WFMath::Polygon<2> clipped = ar->clipToSegment(s.m_segment);
    assert(clipped.isValid());
    
    if (clipped.numCorners() == 0) return;
 
    Point2 segOrigin = s.m_segment.getRect().lowCorner();
    clipped.shift(Point2(0,0) - segOrigin);
    scanConvert(clipped, s);
}

} // of namespace
