/*
 Copyright (C) 2009 Erik Ogenvik <erik@ogenvik.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef TERRAINSHADERUPDATETASK_H_
#define TERRAINSHADERUPDATETASK_H_

#include "framework/tasks/TemplateNamedTask.h"
#include "Types.h"
#include <vector>
#include <sigc++/signal.h>

namespace WFMath {
template<int>
class AxisBox;
}


namespace Ember::OgreView::Terrain {

struct TerrainShader;

class TerrainPage;

class TerrainPageSurfaceCompilationInstance;

/**
 * @brief Updates a terrain shader, i.e. the mercator surfaces.
 * This will also recompile the terrain page material once the surface has been updated.
 * @author Erik Ogenvik <erik@ogenvik.org>
 */
class TerrainShaderUpdateTask : public Tasks::TemplateNamedTask<TerrainShaderUpdateTask> {
public:

	/**
	 * @brief Ctor.
	 * @param geometry The geometry which needs the surfaces updated.
	 * @param shaders The shaders which for each page will be be applied.
	 * @param areas Any areas which define the area to update. This will only be applied if updateAll is set to false.
	 * @param signal A signal which will be emitted in the main thread once all surfaces have been updated.
	 * @param signalMaterialRecompiled A signal which will be passed on and emitted once a material for a terrain page has been recompiled.
	 * @param lightDirection The main light direction.
	 */
	TerrainShaderUpdateTask(GeometryPtrVector geometry,
							std::vector<TerrainShader> shaders,
							AreaStore areas,
							sigc::signal<void(const TerrainShader&, const AreaStore&)>& signal,
							sigc::signal<void(TerrainPage&)>& signalMaterialRecompiled);

	~TerrainShaderUpdateTask() override;

	void executeTaskInBackgroundThread(Tasks::TaskExecutionContext& context) override;

	bool executeTaskInMainThread() override;

private:

	/**
	 * @brief The pages which will be updated.
	 */
	GeometryPtrVector mGeometry;

	/**
	 * @brief The shader which will be applied.
	 */
	std::vector<TerrainShader> mShaders;

	/**
	 * @brief Only the pages affected by the areas will be updated.
	 */
	const AreaStore mAreas;

	/**
	 * @brief A signal to emit once the update is done.
	 */
	sigc::signal<void(const TerrainShader&, const AreaStore&)>& mSignal;

	/**
	 * @brief A signal to pass on to the material recompilation task;
	 */
	sigc::signal<void(TerrainPage&)>& mSignalMaterialRecompiled;

};

}


#endif /* TERRAINSHADERUPDATETASK_H_ */
