#include <Eris/Lobby.h>
#include <Eris/Person.h>

#include <sigcperl/signal_wrap.h>

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

// Haven't wrapped Lobby::LoggedIn, Person::Sight, Person constructor

MODULE = WorldForge::Eris::Lobby		PACKAGE = WorldForge::Eris::Lobby		

Room*
Lobby::join(string roomID)
  PREINIT:
    const char *CLASS = "WorldForge::Eris::Room";

Room*
Lobby::getRoom(string id)
  PREINIT:
    const char *CLASS = "WorldForge::Eris::Room";

Person*
Lobby::getPerson(string id)
  PREINIT:
    const char *CLASS = "WorldForge::Eris::Person";

string
Lobby::getAccountID()

SignalBase*
Lobby::SightPerson()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->SightPerson);
  OUTPUT:
    RETVAL

SignalBase*
Lobby::PrivateTalk()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->PrivateTalk);
  OUTPUT:
    RETVAL

MODULE = WorldForge::Eris::Lobby		PACKAGE = WorldForge::Eris::Person		

void
PersonHandle::DESTROY()

void
Person::msg(string msg)

string
Person::getAccount()

string
Person::getName()
