// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "Shader.h"

#include "Segment.h"
#include "Surface.h"

namespace Mercator {

/// \brief Protected constructor for classes which inherit from this
/// one.
///
/// Permanently sets the color and alpha flags.
Shader::Shader(bool color, bool alpha) : m_color(color), m_alpha(alpha) {
}

/// \brief Destructor does nothing interesting.
Shader::~Shader() = default;

/// \brief Create a new Surface which matches the requirements of this shader.
///
/// Called by terrain when creating a new Segment to add the necessary
/// Surface objects to that Segment.
std::unique_ptr<Surface> Shader::newSurface(const Segment& segment) const {
	return std::make_unique<Surface>(segment, *this, m_color, m_alpha);
}

} // namespace Mercator
