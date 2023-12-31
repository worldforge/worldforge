// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/TerrainMod.h>

#include <iostream>
#include <cassert>
#include <memory>

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif


int terrain_mod_context_test(Mercator::Terrain& terrain) {
	const WFMath::Ball<2> circ2(WFMath::Point<2>(0.0, 0.0), 12.0);
	auto mp = std::make_unique<Mercator::LevelTerrainMod<WFMath::Ball>>(10.0f, circ2);

	mp->setContext(std::make_unique<Mercator::TerrainMod::Context>());
	mp->context()->m_id = "foo";
	terrain.updateMod(1, std::move(mp));

	terrain.updateMod(1, nullptr);


	return 0;
}

int main() {
	Mercator::Terrain terrain(Mercator::Terrain::SHADED);

	terrain.setBasePoint(0, 0, 2.8);
	terrain.setBasePoint(1, 0, 7.1);
	terrain.setBasePoint(2, 0, 7.1);
	terrain.setBasePoint(0, 1, 0.2);
	terrain.setBasePoint(1, 1, 0.2);
	terrain.setBasePoint(2, 1, 0.2);
	terrain.setBasePoint(0, 2, 14.7);
	terrain.setBasePoint(1, 2, 14.7);
	terrain.setBasePoint(2, 2, 14.7);

	const WFMath::Ball<2> circ2(WFMath::Point<2>(0.0, 0.0), 12.0);
	auto mp1 = std::make_unique<Mercator::LevelTerrainMod<WFMath::Ball>>(10.0f, circ2);
	terrain.updateMod(1, std::move(mp1));

	Mercator::Segment* segment = terrain.getSegmentAtIndex(0, 0);

	assert(segment->getMods().size() == 1);

	auto mp1_1 = std::make_unique<Mercator::LevelTerrainMod<WFMath::Ball>>(10.0f, circ2);
	auto ptr = mp1_1.get();
	terrain.updateMod(1, std::move(mp1_1));
	assert(segment->getMods().size() == 1);
	assert(segment->getMods().find(1)->second == ptr);

	const WFMath::RotBox<2> rot(
			WFMath::Point<2>(-80., -130.),
			WFMath::Vector<2>(150.0, 120.0),
			WFMath::RotMatrix<2>().rotation(WFMath::numeric_constants<WFMath::CoordType>::pi() / 4));
	auto mp2 = std::make_unique<Mercator::LevelTerrainMod<WFMath::RotBox>>(10.0f, rot);
	terrain.updateMod(2, std::move(mp2));

	const WFMath::Ball<2> ball(WFMath::Point<2>(80, 80), 10);
	auto mp3 = std::make_unique<Mercator::CraterTerrainMod<WFMath::Ball>>(-5.f, ball);
	terrain.updateMod(3, std::move(mp3));

	segment->populate();

	segment = terrain.getSegmentAtIndex(1, 1);

	if (segment == nullptr) {
		std::cerr << "Segment not created by addition of required basepoints"
				  << std::endl;
		return 1;
	}

	segment->populate();

	assert(segment->isValid());

	auto mp3_rect1 = terrain.updateMod(3, std::make_unique<Mercator::CraterTerrainMod<WFMath::Ball>>(-5.f, ball));

	assert(!segment->isValid());

	//Check that the stored bbox is correctly updated when calling updateMod().
	auto new_mp3 = std::make_unique<Mercator::CraterTerrainMod<WFMath::Ball>>(-5.f, ball);
	new_mp3->setShape(-5.f, WFMath::Ball<2>(WFMath::Point<2>(-80, 80), 10));
	auto mp3_rect2 = new_mp3->bbox();
	WFMath::AxisBox<2> mp3_rect3 = terrain.updateMod(3, std::move(new_mp3));
	assert(mp3_rect1 == mp3_rect3);
	auto another_new_mp3 = std::make_unique<Mercator::CraterTerrainMod<WFMath::Ball>>(-5.f, ball);
	another_new_mp3->setShape(-5.f, WFMath::Ball<2>(WFMath::Point<2>(-80, -80), 10));
	WFMath::AxisBox<2> mp3_rect4 = terrain.updateMod(3, std::move(another_new_mp3));
	assert(mp3_rect2 == mp3_rect4);

	terrain.updateMod(1, nullptr);

	terrain.updateMod(2, nullptr);

	terrain.updateMod(3, nullptr);

	return terrain_mod_context_test(terrain);
}
