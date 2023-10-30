// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2002 Michael Koch, 2003 Alistair Riddoch

// $Id$

#ifndef ATLAS_DEBUG_H
#define ATLAS_DEBUG_H

#ifdef DEBUG
#define ATLAS_DEBUG(a) { if (debug_flag) { a; } }
#else
#define ATLAS_DEBUG(a)
#endif

#endif // ATLAS_DEBUG_H
