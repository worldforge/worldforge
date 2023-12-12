// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Damien McGinnes, Alistair Riddoch

#include "BasePoint.h"
#include "iround.h"

namespace Mercator {

unsigned int BasePoint::seed() const {
	return I_ROUND(m_height * 1000.0);
}

bool BasePoint::operator==(const BasePoint& rhs) const noexcept {
	return rhs.m_falloff == m_falloff && rhs.m_height == m_height && rhs.m_roughness == m_roughness;
}

bool BasePoint::operator!=(const BasePoint& rhs) const noexcept {
	return !(*this == rhs);
}

} //namespace Mercator
