// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#include "Hack.h"

using namespace std;
using namespace Atlas;

Codec<iostream>* Atlas::UngodlyHack::GetPacked(iostream& stream, Bridge* bridge)
{
    list<Factory<Codec<iostream> >*>::iterator i;

    for (i = Factory<Codec<iostream> >::Factories().begin();
	 i != Factory<Codec<iostream> >::Factories().end(); ++i)
    {
	if ((*i)->GetName() == "Packed") break;
    }

    if (i == Factory<Codec<iostream> >::Factories().end()) abort();

    return (*i)->New(Codec<iostream>::Parameters(stream, bridge));
}

Codec<iostream>* Atlas::UngodlyHack::GetXML(iostream& stream, Bridge* bridge)
{
    list<Factory<Codec<iostream> >*>::iterator i;

    for (i = Factory<Codec<iostream> >::Factories().begin();
	 i != Factory<Codec<iostream> >::Factories().end(); ++i)
    {
	if ((*i)->GetName() == "XML") break;
    }

    if (i == Factory<Codec<iostream> >::Factories().end()) abort();

    return (*i)->New(Codec<iostream>::Parameters(stream, bridge));
}
