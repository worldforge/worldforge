#ifndef MERCATOR_INTERSECT
#define MERCATOR_INTERSECT

#include <wfmath/axisbox.h>
#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <Mercator/Terrain.h>

namespace Mercator {

    bool Intersect(const Terrain &t, const WFMath::AxisBox<3> &bbox);
    bool Intersect(const Terrain &t, const WFMath::Point<3> &pt);

    //height over terrain
    float HOT(const Terrain &t, const WFMath::Point<3> &pt);

    bool Intersect(const Terrain &t, const WFMath::Point<3> &sPt, const WFMath::Vector<3>& dir,
            WFMath::Point<3> &intersection, WFMath::Vector<3> &normal, float &par);
} // namespace Mercator

#endif
