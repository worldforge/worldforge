// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2009 Alistair Riddoch

#include <Mercator/Terrain.h>
#include <Mercator/Area.h>
#include <Mercator/AreaShader.h>
#include <Mercator/Segment.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <cassert>

typedef WFMath::Point<2> Point2;

void testAreaShader() {
	auto a1 = std::make_unique<Mercator::Area>(1, false);
	auto a1_ptr = a1.get();

	WFMath::Polygon<2> p;
	p.addCorner(p.numCorners(), Point2(3, 4));
	p.addCorner(p.numCorners(), Point2(10, 10));
	p.addCorner(p.numCorners(), Point2(14, 6));
	p.addCorner(p.numCorners(), Point2(18, 4));
	p.addCorner(p.numCorners(), Point2(17, 19));
	p.addCorner(p.numCorners(), Point2(6, 20));
	p.addCorner(p.numCorners(), Point2(-1, 18));
	p.addCorner(p.numCorners(), Point2(-8, 11));

	a1->setShape(p);

	Mercator::Terrain terrain(Mercator::Terrain::SHADED, 16);

	Mercator::AreaShader ashade(1);
	terrain.addShader(&ashade, 0);

	terrain.setBasePoint(0, 0, -1);
	terrain.setBasePoint(0, 1, 8);
	terrain.setBasePoint(1, 0, 2);
	terrain.setBasePoint(1, 1, 11);

	terrain.updateArea(1, std::move(a1));

	Mercator::Segment* seg = terrain.getSegmentAtIndex(0, 0);
	assert(a1_ptr->checkIntersects(*seg));

	seg->populateSurfaces();
}

int main() {
	testAreaShader();

	return 0;
}
