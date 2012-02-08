// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_IROUND_H
#define MERCATOR_IROUND_H

#ifndef PACKAGE_NAME
#error iround.h must be included after config.h
#endif

// FIXME tr1 contains std::tr1::lrint which is type overloaded
// In c++0x this becomes std::lrint

#ifdef HAVE_LRINTF
    #define I_ROUND(_x) (::lrintf(_x)) 
#elif defined(HAVE_RINTF)
    #define I_ROUND(_x) ((int)::rintf(_x)) 
#elif defined(HAVE_RINT)
    #define I_ROUND(_x) ((int)::rint(_x)) 
#else
    #define I_ROUND(_x) ((int)(_x)) 
#endif

#endif // MERCATOR_IROUND_H
