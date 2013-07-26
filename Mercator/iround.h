// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_IROUND_H
#define MERCATOR_IROUND_H

//MSVC 11.0 doesn't support std::lround so we'll use boost. When MSVC gains support for std::lround this could be removed.
#ifdef _MSC_VER
#include <boost/math/special_functions/round.hpp>
#define I_ROUND(_x) (boost::math::lround(_x))
#else
#include <cmath>
#define I_ROUND(_x) (std::lround(_x))
#endif

#endif // MERCATOR_IROUND_H
