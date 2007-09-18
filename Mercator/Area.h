// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef MERCATOR_AREA_H
#define MERCATOR_AREA_H

#include <wfmath/axisbox.h>
#include <wfmath/polygon.h>

namespace Mercator
{

class Segment;

/// \brief Region of terrain surface which is modified.
///
/// Objects of this class describe regions of the surface which have
/// been changed. Which layers of the shaded terrain is affected by
/// their layer number. For example, if they are below the snow layer
/// then the snow shader will cover them where the area affected by
/// snow intersects with the area, but the lower grass area will be
/// overriden. In order to get the best effect it is important to
/// select the layer carefully.
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
    
    const WFMath::AxisBox<2> & bbox() const
    {
        return m_box;
    }

    const WFMath::Polygon<2> & shape() const
    {
        return m_shape;
    }
    
    /**
    Test if a segment intersects this area
    */
    bool checkIntersects(const Segment& s) const;

    WFMath::Polygon<2> clipToSegment(const Segment& s) const;
private:

    int m_layer;
    bool m_hole;
    WFMath::Polygon<2> m_shape;
    WFMath::AxisBox<2> m_box;
};

}

#endif // of MERCATOR_AREA_H
