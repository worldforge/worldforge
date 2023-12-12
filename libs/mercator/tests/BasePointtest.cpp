// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Alistair Riddoch

#include <Mercator/BasePoint.h>
#include <cassert>

int main() {
	Mercator::BasePoint bp1;

	bp1.falloff() = 2.0;
	bp1.height() = 3.0;
	bp1.roughness() = 4.0;

	assert(bp1 == Mercator::BasePoint(3.0, 4.0, 2.0));
	assert(bp1 != Mercator::BasePoint(3.0, 4.0, 3.0));

	assert(bp1.seed() == Mercator::BasePoint(3.0, 4.0, 3.0).seed());
	assert(bp1.seed() != Mercator::BasePoint(4.0, 4.0, 3.0).seed());

	return 0;
}
