// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef ERIS_IROUND_H
#define ERIS_IROUND_H

//MSVC 11.0 doesn't support std::lround so we'll use boost. When MSVC gains support for std::lround this could be removed.
#ifdef _MSC_VER
#include <boost/math/special_functions/round.hpp>
#define I_ROUND(_x) (boost::math::lround(_x))
#define L_ROUND(_x) (boost::math::llround(_x))
#else
#include <cmath>
#define I_ROUND(_x) (std::lround(_x))
#define L_ROUND(_x) (std::llround(_x))
#endif

#endif // ERIS_IROUND_H
