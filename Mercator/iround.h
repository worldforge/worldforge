// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_IROUND_H
#define MERCATOR_IROUND_H

#ifndef PACKAGE
#error iround.h must be included after config.h
#endif

#ifdef HAVE_LRINTF
    #define I_ROUND(_x) (::lrintf(_x)) 
#elif defined(HAVE_RINTF)
    #define I_ROUND(_x) ((int)::rintf(_x)) 
#elif defined(HAVE_RINT)
    #define I_ROUND(_x) ((int)::rint(_x)) 
#else
    #define I_ROUND(_x) ((int)(_x)) 
#endif

#ifdef HAVE_FABSF
    #define F_ABS(_x) (::fabsf(_x))
#else
    #define F_ABS(_x) (::fabs(_x))
#endif

#ifdef HAVE_POWF
    #define F_POW(_x, _y) (::powf(_x, _y))
#else
    #define F_POW(_x, _y) (::pow(_x, _y))
#endif

#ifdef HAVE_FLOORF
    #define F_FLOOR(_x) (::floorf(_x))
#else
    #define F_FLOOR(_x) (::floor(_x))
#endif

#ifdef HAVE_CEILF
    #define F_CEIL(_x) (::ceilf(_x))
#else
    #define F_CEIL(_x) (::ceil(_x))
#endif

#endif // MERCATOR_IROUND_H
