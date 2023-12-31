// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2010 Alistair Riddoch

#include <Mercator/Terrain.h>

#include <Mercator/Area.h>
#include <Mercator/Segment.h>
#include <Mercator/Shader.h>
#include <Mercator/Surface.h>

#include <wfmath/point.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <cassert>

// This is a relatively specific tests which ensures Terrain::addArea
// behaves as defined with respect to invalidating surfaces, to verify
// a refactoring. Previous cases where std::map::count() was used followed
// by [] are inefficient, and I wanted to verify consistent bahviour before
// checking to use std::map::find()

class TestShader : public Mercator::Shader {
public:
	virtual bool checkIntersect(const Mercator::Segment&) const {
		return true;
	}

	/// \brief Populate a Surface with data.
	virtual void shade(Mercator::Surface&) const {}
};

int main() {
	Mercator::Terrain t;

	// Add a null test shader to the terrain
	TestShader shader{};
	t.addShader(&shader, 0);

	// Create a test area with a shape which intersects
	// the Segment at 0,0
	auto a1 = std::make_unique<Mercator::Area>(0, false);

	WFMath::Polygon<2> p;
	p.addCorner(p.numCorners(), WFMath::Point<2>(3, 4));
	p.addCorner(p.numCorners(), WFMath::Point<2>(10, 10));
	p.addCorner(p.numCorners(), WFMath::Point<2>(14, 6));
	p.addCorner(p.numCorners(), WFMath::Point<2>(18, 4));
	p.addCorner(p.numCorners(), WFMath::Point<2>(17, 19));
	p.addCorner(p.numCorners(), WFMath::Point<2>(6, 20));
	p.addCorner(p.numCorners(), WFMath::Point<2>(-1, 18));
	p.addCorner(p.numCorners(), WFMath::Point<2>(-8, 11));

	a1->setShape(p);

	// Add enough base points to force the creation of the Segment at 0,0
	t.setBasePoint(0, 0, -1);
	t.setBasePoint(0, 1, 8);
	t.setBasePoint(1, 0, 2);
	t.setBasePoint(1, 1, 11);

	// Get the Segment at 0,0
	Mercator::Segment* seg = t.getSegmentAtIndex(0, 0);

	assert(seg);

	// Get the surfaces, and add one corresponding to the shader we added.
	// We need to do this as the functions that would normally make it happen
	// have been stubbed out.
	Mercator::Segment::Surfacestore& sss = seg->getSurfaces();

	auto* sfce = new Mercator::Surface(*seg, shader);

	// Force allocation of the surface buffer so we can check later that it
	// gets destroyed when the area is added to the terrain.
	sfce->allocate();
	assert(sfce->isValid());

	// Add the surface to the store for this segment
	sss[0].reset(sfce);

	// Add the area which should cause relevant surface date to be invalidated
	t.updateArea(1, std::move(a1));

	// We assert this to ensure that the buffer has been de-allocated
	// by a call to Surface::invalidate caused by adding the Area.
	assert(!sfce->isValid());

	// force the surface to re-allocate
	sfce->allocate();
	assert(sfce->isValid());

	// Modify the areas shape
	p.addCorner(p.numCorners(), WFMath::Point<2>(-9, 12));
	auto a1_2 = std::make_unique<Mercator::Area>(0, false);
	a1_2->setShape(p);

	// and cause an area update
	t.updateArea(1, std::move(a1_2));

	// Check the surface has been invalidated again
	assert(!sfce->isValid());

	// force the surface to re-allocate
	sfce->allocate();
	assert(sfce->isValid());

	t.updateArea(1, nullptr);

	// Check the surface has been invalidated again
	assert(!sfce->isValid());
}

// stubs

#include <Mercator/Area.h>
#include <Mercator/Buffer_impl.h>
#include <Mercator/Shader.h>
#include <Mercator/TerrainMod.h>

