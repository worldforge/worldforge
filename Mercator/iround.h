// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_IROUND_H
#define MERCATOR_IROUND_H

// std::lround is missing on Android: https://code.google.com/p/android/issues/detail?id=54418
#ifdef __ANDROID__
#include <cmath>
#define I_ROUND(_x) (lroundf(_x))
#else
#include <cmath>
#define I_ROUND(_x) (std::lround(_x))
#endif

#endif // MERCATOR_IROUND_H
