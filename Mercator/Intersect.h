#ifndef MERCATOR_INTERSECT
#define MERCATOR_INTERSECT

#include <wfmath/axisbox.h>
#include <Mercator/Terrain.h>

namespace Mercator {

    bool Intersect(const Terrain &t, const WFMath::AxisBox<3> &bbox);
    bool Intersect(const Terrain &t, const WFMath::Point<3> &pt);

    //height over terrain
    float HOT(const Terrain &t, const WFMath::Point<3> &pt);

} // namespace Mercator

#endif
