#include <Eris/Avatar.h>

#include <sigcperl/signal_wrap.h>

#include "../worldhandle.h"
#include "../perlentity.h"
#include "../conversion.h"

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
}

// try to convert a perl array to a Point<3> or Vector<3>
#define TRY_EXTRACT_TRIPLET(triplet, val) \
if(!SvTYPE(SvRV(val)) == SVt_PVAV) \
  XSRETURN_UNDEF; \
AV* array = (AV*) SvRV(val); \
for(int i = 0; i < 3; ++i) { \
  SV** component = av_fetch(array, i, 0); \
  triplet[i] = component ? SvNV(*component) : 0; \
}

using namespace Eris;
using std::string;
using SigCPerl::SignalBase;

MODULE = WorldForge::Eris::Avatar		PACKAGE = WorldForge::Eris::Avatar		

void
AvatarHandle::DESTROY()

World*
Avatar::getWorld()
  PREINIT:
    const char* CLASS = "WorldForge::Eris::World";

string
Avatar::getID()

Entity*
Avatar::getEntity()
  PREINIT:
    const char* CLASS = "WorldForge::Eris::Entity";

void
Avatar::drop(Entity* entity, ...)
  CODE:
    switch(items) {
      case 2:
        THIS->drop(entity);
        break;
      case 3:
      {
        WFMath::Vector<3> offset;
        TRY_EXTRACT_TRIPLET(offset, ST(2));
        THIS->drop(entity, offset);
        break;
      }
      case 4:
      {
        WFMath::Point<3> pos;
        TRY_EXTRACT_TRIPLET(pos, ST(2));
        THIS->drop(entity, pos, SigCPerl::get_string(ST(3)));
        break;
      }
      default:
        XSRETURN_UNDEF;
    }

Avatar*
Avatar::find(Connection* c, string s)
  PREINIT:
    const char* CLASS = "WorldForge::Eris::Avatar";

void
Avatar::getAvatars(Connection* c)
  PPCODE:
    std::vector<Avatar*> list = Avatar::getAvatars(c);
    std::vector<Avatar*>::iterator I;
    for(I = list.begin(); I != list.end(); ++I)
      XPUSHs(sv_2mortal(SigCPerl::GetSV(new AvatarHandle(*I))));

SignalBase*
Avatar::InvAdded()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->InvAdded);
  OUTPUT:
    RETVAL

SignalBase*
Avatar::InvRemoved()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->InvRemoved);
  OUTPUT:
    RETVAL
