// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "TileShader.h"

#include "Segment.h"
#include "Surface.h"

#include <cassert>

namespace Mercator {

TileShader::TileShader() = default;

TileShader::~TileShader() = default;

bool TileShader::checkIntersect(const Segment& s) const {
	return true;
}

void TileShader::shade(Surface& surface) const {
	ColorT* sdata = surface.getData();
	auto sdata_len = surface.getSize() * surface.getSize();

	auto I = m_subShaders.begin();
	auto Iend = m_subShaders.end();
	for (; I != Iend; ++I) {
		if (!I->second->checkIntersect(surface.getSegment())) {
			continue;
		}
		auto subs = I->second->newSurface(surface.getSegment());
		assert(subs);
		subs->populate();
		ColorT* subsdata = subs->getData();
		auto channels = subs->getChannels();

		for (unsigned int i = 0; i < sdata_len; ++i) {
			if (subsdata[i * channels + channels - 1] > 127) {
				sdata[i] = I->first;
			}
		}
	}
}

} // namespace Mercator
