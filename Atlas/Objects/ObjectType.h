// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_OBJECTS_OBJECTTYPE_H
#define ATLAS_OBJECTS_OBJECTTYPE_H

namespace Atlas { namespace Objects {

enum ObjectType
{
    OBJTYPE_CLASS,
    OBJTYPE_INTERFACE,
    OBJTYPE_META,
    OBJTYPE_OP,
    OBJTYPE_DEFINITION,
    OBJTYPE_DATA_TYPE,
    OBJTYPE_TYPE,
    OBJTYPE_INSTANCE
};
    
} } // namespace Atlas::Objects

#endif
