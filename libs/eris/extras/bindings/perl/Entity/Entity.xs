#include <Eris/Entity.h>

#include <sigcperl/signal_wrap.h>
#include <sigcperl/signal_array.h>

#include "../perlentity.h"
#include "../atlas_convert.h"
#include "../typeinfohandle.h"
#include "../worldhandle.h"
#include "../conversion.h"

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
}

typedef const Atlas::Message::Object AtlasConstObject;

using namespace Eris;
using std::string;
using SigCPerl::SignalBase;

#undef scalar

#define PUSH_STRING_SET(list) \
for(StringSet::iterator i = list.begin(); i != list.end(); ++i) \
  XPUSHs(sv_2mortal(newSVpv(i->c_str(), i->size())));

// creates an SV* from either a Point<3> or a Vector<3>
template<class C>
static SV* triplet_to_sv(const C& c)
{
  AV* array = newAV();
  for(int i = 0; i < 3; ++i)
    av_push(array, newSVnv(c[i]));
  return newRV_noinc((SV*) array);
}

namespace SigCPerl {
inline SV* GetSV(const WFMath::Point<3>& p) {return triplet_to_sv(p);}

template<> struct ArgBox<const WFMath::Point<3>&>
{
  ArgBox(SV *sv) throw(SigCPerl::BadConvertVal)
  {
    if(SvTYPE(SvRV(sv)) != SVt_PVAV)
      throw SigCPerl::BadConvertVal();

    AV* array = (AV*) SvRV(sv);
    for(int i = 0; i < 3; ++i) {
      SV** component = av_fetch(array, i, 0);
      m_val[i] = component ? SvNV(*component) : 0;
      m_val.setValid(); // since we initialized it
    }
  }
  const WFMath::Point<3>& val() {return m_val;}
  const WFMath::Point<3>& ret_val() {return m_val;}

  WFMath::Point<3> m_val;
};
}

MODULE = WorldForge::Eris::Entity		PACKAGE = WorldForge::Eris::Entity		

Entity*
Entity::getContainer()
  PREINIT:
    const char* CLASS = "WorldForge::Eris::Entity";

unsigned
Entity::getNumMembers()


Entity*
Entity::getMember(unsigned index)
  PREINIT:
    const char* CLASS = "WorldForge::Eris::Entity";

SV*
Entity::getPosition()
  CODE:
    RETVAL = triplet_to_sv(THIS->getPosition());
  OUTPUT:
    RETVAL

SV*
Entity::getVelocity()
  CODE:
    RETVAL = triplet_to_sv(THIS->getVelocity());
  OUTPUT:
    RETVAL

SV*
Entity::getOrientation()
  CODE:
    AV* array = newAV();
    WFMath::Quaternion q = THIS->getOrientation();
    av_push(array, newSVnv(q.scalar()));
    for(int i = 0; i < 3; ++i)
      av_push(array, newSVnv(q.vector()[i]));
    RETVAL = newRV_noinc((SV*) array);
  OUTPUT:
    RETVAL

SV*
Entity::getBBox()
  CODE:
    if(!THIS->hasBBox())
      XSRETURN_UNDEF;
    AV* array = newAV();
    WFMath::AxisBox<3> bbox = THIS->getBBox();
    av_push(array, triplet_to_sv(bbox.lowCorner()));
    av_push(array, triplet_to_sv(bbox.highCorner()));
    RETVAL = newRV_noinc((SV*) array);
  OUTPUT:
    RETVAL

AtlasConstObject*
Entity::getProperty(string s)
  CODE:
    if(!THIS->hasProperty(s))
      XSRETURN_UNDEF;
    RETVAL = &THIS->getProperty(s);
  OUTPUT:
    RETVAL

string
Entity::getID()

string
Entity::getName()

float
Entity::getStamp()

void
Entity::getInherits()
  PPCODE:
    StringSet strings = THIS->getInherits();
    PUSH_STRING_SET(strings)

TypeInfoHandle*
Entity::getType()
  CODE:
    const char* CLASS = "WorldForge::Eris::TypeInfo";
    RETVAL = new TypeInfoHandle(THIS);
  OUTPUT:
    RETVAL

World*
Entity::getWorld()
  PREINIT:
    const char* CLASS = "WorldForge::Eris::World";

bool
Entity::isVisible()

SignalBase*
Entity::AddedMember()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->AddedMember);
  OUTPUT:
    RETVAL

SignalBase*
Entity::RemovedMember()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->RemovedMember);
  OUTPUT:
    RETVAL

SignalBase*
Entity::Recontainered()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Recontainered);
  OUTPUT:
    RETVAL

SignalBase*
Entity::Changed()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapArraySignal(THIS->Changed);
  OUTPUT:
    RETVAL

SignalBase*
Entity::Moved()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Moved);
  OUTPUT:
    RETVAL

SignalBase*
Entity::Say()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Say);
  OUTPUT:
    RETVAL
