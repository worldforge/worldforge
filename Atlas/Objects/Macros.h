// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#ifndef ATLAS_OBJECTS_MACROS_H
#define ATLAS_OBJECTS_MACROS_H

#define ATTRIBUTE(type, name) type name; bool sent_ ## name;

#define METHODS(type, name) \
virtual type Get_ ## name ## () const; \
virtual void Set_ ## name ## (const type ## &); \
virtual bool Has_ ## name ## () const;

#define INIT(name, value) name = value;

#define RESET(name) sent ## _ ## name = false;

#define GETATTR(n) if ( name == #n ) return Get_ ## n ##();


#define SETATTR(n, type) if ( name == #n ) { \
    Set_ ## n ## (object.As( Object:: ## type )); return ; \
}

#define HASATTR(n) if ( name == #n ) return Has_ ## n ## ();

#define TRANSMIT(n) if (!sent_ ## n ## ) { \
    e.MapItem( #n , n ); \
    sent_ ## n = true; \
}

#define IMPL_METHODS(cls, type, name) \
type cls ## :: ## Get_ ## name ## () const \
{ \
    return name##; \
} \
\
void cls ## :: ## Set_ ## name ## (const type ## & v) \
{ \
    name = v; \
    sent_##name = false; \
} \
\
bool cls ## :: ## Has_ ## name ## () const \
{ \
    return true; \
}

#endif
