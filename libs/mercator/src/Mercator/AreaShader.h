// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef MERCATOR_AREASHADER_H
#define MERCATOR_AREASHADER_H

#include "Shader.h"

namespace Mercator {

class Area;

/// \brief Shader for handling areas.
class AreaShader : public Shader {
public:
	/// \brief Constructor
	///
	/// @param layer layer number.
	explicit AreaShader(int layer);

	void shade(Surface& s) const override;

	bool checkIntersect(const Segment&) const override;

private:
	/// helper to shader a single area into the surface
	static void shadeArea(Surface& s, const Area& ar) ;

	/// The layer number.
	int m_layer;
};

}

#endif // of MERCATOR_AREASHADER_H
