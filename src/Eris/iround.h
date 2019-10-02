// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef ERIS_IROUND_H
#define ERIS_IROUND_H

// std::lround is missing on Android: https://code.google.com/p/android/issues/detail?id=54418
#ifdef __ANDROID__
#include <cmath>
#define I_ROUND(_x) (lroundf(_x))
#define L_ROUND(_x) (llround(_x))
#else
#include <cmath>
#define I_ROUND(_x) (std::lround(_x))
#define L_ROUND(_x) (std::llround(_x))
#endif

#endif // ERIS_IROUND_H
