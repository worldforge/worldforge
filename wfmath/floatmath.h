// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003-2011 Alistair Riddoch

#ifndef WFMATH_IROUND_H
#define WFMATH_IROUND_H

#ifndef PACKAGE
#error floatmath.h must be included after config.h
#endif

#ifdef HAVE_FABSF
    #define F_ABS(_x) (::fabsf(_x))
#else
    #define F_ABS(_x) ((float)::fabs(_x))
#endif

#ifdef HAVE_SQRTF
    #define F_SQRT(_x) (::sqrtf(_x))
#else
    #define F_SQRT(_x) ((float)::sqrt(_x))
#endif

#ifdef HAVE_SINF
    #define F_SIN(_x) (::sinf(_x))
#else
    #define F_SIN(_x) ((float)::sin(_x))
#endif

#ifdef HAVE_COSF
    #define F_COS(_x) (::cosf(_x))
#else
    #define F_COS(_x) ((float)::cos(_x))
#endif

#endif // WFMATH_IROUND_H
