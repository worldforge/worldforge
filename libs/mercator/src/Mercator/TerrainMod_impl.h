// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Damien McGinnes, Alistair Riddoch

#ifndef MERCATOR_TERRAIN_MOD_IMPL_H
#define MERCATOR_TERRAIN_MOD_IMPL_H

#include "TerrainMod.h"

#include "Segment.h"

namespace Mercator {

template<template<int> class Shape>
ShapeTerrainMod<Shape>::ShapeTerrainMod(const Shape<2>& s) : m_shape(s) {
	m_box = m_shape.boundingBox();
}


template<template<int> class Shape>
ShapeTerrainMod<Shape>::~ShapeTerrainMod() = default;

template<template<int> class Shape>
bool ShapeTerrainMod<Shape>::checkIntersects(const Segment& s) const {
	return WFMath::Intersect(m_shape, s.getRect(), false) ||
		   WFMath::Contains(s.getRect(), m_shape.getCorner(0), false);
}

template<template<int> class Shape>
void ShapeTerrainMod<Shape>::setShape(const Shape<2>& s) {
	m_shape = s;
	m_box = m_shape.boundingBox();
}

template<template<int> class Shape>
LevelTerrainMod<Shape>::~LevelTerrainMod() = default;

template<template<int> class Shape>
void LevelTerrainMod<Shape>::apply(float& point, int x, int z) const {
	if (Contains(this->m_shape, WFMath::Point<2>(x, z), true)) {
		point = this->m_function(point, m_level);
	}
}

template<template<int> class Shape>
void LevelTerrainMod<Shape>::setShape(float level, const Shape<2>& s) {
	ShapeTerrainMod<Shape>::setShape(s);
	m_level = level;
}

template<template<int> class Shape>
AdjustTerrainMod<Shape>::~AdjustTerrainMod() = default;

template<template<int> class Shape>
void AdjustTerrainMod<Shape>::apply(float& point, int x, int z) const {
	if (Contains(this->m_shape, WFMath::Point<2>(x, z), true)) {
		point += m_dist;
	}
}

template<template<int> class Shape>
void AdjustTerrainMod<Shape>::setShape(float dist, const Shape<2>& s) {
	ShapeTerrainMod<Shape>::setShape(s);
	m_dist = dist;
}

template<template<int> class Shape>
SlopeTerrainMod<Shape>::~SlopeTerrainMod() = default;

template<template<int> class Shape>
void SlopeTerrainMod<Shape>::apply(float& point, int x, int z) const {
	if (Contains(this->m_shape, WFMath::Point<2>(x, z), true)) {
		float level = m_level + (this->m_shape.getCenter()[0] - x) * m_dx
					  + (this->m_shape.getCenter()[1] - z) * m_dz;
		point = this->m_function(point, level);
	}
}

template<template<int> class Shape>
void SlopeTerrainMod<Shape>::setShape(float level, float dx, float dz, const Shape<2>& s) {
	ShapeTerrainMod<Shape>::setShape(s);
	m_level = level;
	m_dx = dx;
	m_dz = dz;
}


template<template<int> class Shape>
CraterTerrainMod<Shape>::~CraterTerrainMod() = default;

template<template<int> class Shape>
void CraterTerrainMod<Shape>::apply(float& point, int x, int z) const {
	if (Contains(this->m_shape, WFMath::Point<2>(x, z), true)) {
		point += m_level;
	}
}

template<template<int> class Shape>
void CraterTerrainMod<Shape>::setShape(float level, const Shape<2>& s) {
	ShapeTerrainMod<Shape>::setShape(s);
	m_level = level;
}


} //namespace Mercator

#endif // MERCATOR_TERRAIN_MOD_IMPL_H
