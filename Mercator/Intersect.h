#ifndef MERCATOR_INTERSECT
#define MERCATOR_INTERSECT

#include <wfmath/axisbox.h>
#include <Mercator/Terrain.h>

namespace Mercator {

    bool Intersect(const Terrain &t, const WFMath::AxisBox<3> &bbox);

} // namespace Mercator

#endif
