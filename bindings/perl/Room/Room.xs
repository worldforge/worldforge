#include <Eris/Room.h>

#include <sigcperl/signal_wrap.h>
#include <sigcperl/signal_array.h>

#include "../roomhandle.h"
#include "../conversion.h"

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
}

using namespace Eris;
using std::string;
using SigCPerl::SignalBase;

#define PUSH_STRING_LIST(list) \
for(StringList::iterator i = list.begin(); i != list.end(); ++i) \
  XPUSHs(sv_2mortal(newSVpv(i->c_str(), i->size())));

// Room::Destroy, actually deletes the RoomHandle, not the Room

MODULE = WorldForge::Eris::Room		PACKAGE = WorldForge::Eris::Room		

void
RoomHandle::DESTROY()

void
Room::say(string tk)

void
Room::emote(string em)

void
Room::leave()

Room*
Room::createRoom(string name)
  PREINIT:
    const char *CLASS = "WorldForge::Eris::Room";

string
Room::getName()

void
Room::getPeople()
  PPCODE:
    StringList list = THIS->getPeople();
    PUSH_STRING_LIST(list);

void
Room::getRooms()
  PPCODE:
    StringList list = THIS->getRooms();
    PUSH_STRING_LIST(list);

string
Room::getID()

SignalBase*
Room::Entered()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Entered);
  OUTPUT:
    RETVAL

SignalBase*
Room::Talk()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Talk);
  OUTPUT:
    RETVAL

SignalBase*
Room::Emote()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Emote);
  OUTPUT:
    RETVAL

SignalBase*
Room::Appearance()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Appearance);
  OUTPUT:
    RETVAL

SignalBase*
Room::Disappearance()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Disappearance);
  OUTPUT:
    RETVAL

SignalBase*
Room::Changed()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapArraySignal(THIS->Changed);
  OUTPUT:
    RETVAL
