//
// C++ Interface: ShrubberyFoliage
//
// Description: 
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2008
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.//
//
#ifndef EMBEROGRE_ENVIRONMENTSHRUBBERYFOLIAGE_H
#define EMBEROGRE_ENVIRONMENTSHRUBBERYFOLIAGE_H

#include "FoliageBase.h"

namespace Forests {
class PagedGeometry;
}



namespace Ember::OgreView {

namespace Terrain {
struct TerrainFoliageDefinition;

struct TerrainLayerDefinition;

class TerrainArea;
}

namespace Environment {

class FoliageLoader;

/**
	@author Erik Ogenvik <erik@ogenvik.org>
*/
class ShrubberyFoliage : public FoliageBase {
public:
	ShrubberyFoliage(Terrain::TerrainManager& terrainManager,
					 Terrain::TerrainLayer terrainLayer,
					 Terrain::TerrainFoliageDefinition foliageDefinition);

	~ShrubberyFoliage() override;

	void frameStarted() override;

	void initialize() override;

	void setDensity(float newGrassDensity) override;

	void setFarDistance(float factor) override;

protected:
	std::unique_ptr<FoliageLoader> mLoader;
};
}

}



#endif
