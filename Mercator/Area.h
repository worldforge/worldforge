#ifndef MERCATOR_AREA_H
#define MERCATOR_AREA_H

#include <wfmath/axisbox.h>
#include <wfmath/polygon.h>

namespace Mercator
{

class Segment;

class Area
{
public:
    Area(int l, bool hole);
    
    void setLayer(int l, bool hole);

    void setShape(const WFMath::Polygon<2>& p);

    bool contains(double x, double y) const;

    int getLayer() const
    {
        return m_layer;
    }
    
    bool isHole() const
    {
        return m_hole;
    }
    
    WFMath::AxisBox<2> bbox() const
    {
        return m_box;
    }
    
    /**
    Test if a segment intersects this area
    */
    bool checkIntersects(const Segment& s) const;

    WFMath::Polygon<2> clipToSegment(Segment& s) const;
private:

    int m_layer;
    bool m_hole;
    WFMath::Polygon<2> m_shape;
    WFMath::AxisBox<2> m_box;
};

}

#endif // of MERCATOR_AREA_H
