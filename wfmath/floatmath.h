// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003-2011 Alistair Riddoch

#ifndef WFMATH_IROUND_H
#define WFMATH_IROUND_H

#ifndef PACKAGE
#error iround.h must be included after config.h
#endif

#ifdef HAVE_FABSF
    #define F_ABS(_x) (::fabsf(_x))
#else
    #define F_ABS(_x) ((float)::fabs(_x))
#endif

#endif // WFMATH_IROUND_H
