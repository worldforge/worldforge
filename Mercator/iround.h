// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_IROUND_H
#define MERCATOR_IROUND_H

#ifndef PACKAGE
#error iround.h must be included after config.h
#endif

#ifdef HAVE_LRINTF
    #define I_ROUND(x) (::lrintf(x)) 
#elif defined(HAVE_RINTF)
    #define I_ROUND(x) ((int)::rintf(x)) 
#elif defined(HAVE_RINT)
    #define I_ROUND(x) ((int)::rint(x)) 
#else
    #define I_ROUND(x) ((int)(x)) 
#endif

#endif // MERCATOR_IROUND_H
