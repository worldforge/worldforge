#include <Eris/TypeInfo.h>

#include <sigcperl/signal_wrap.h>

#include "../typeinfohandle.h"

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
}

#define PUSH_STRING_SET(list) \
for(StringSet::iterator i = list.begin(); i != list.end(); ++i) \
  XPUSHs(sv_2mortal(newSVpv(i->c_str(), i->size())));

using namespace Eris;
using std::string;
using SigCPerl::SignalBase;

MODULE = WorldForge::Eris::TypeInfo		PACKAGE = WorldForge::Eris::TypeInfo		

void
TypeInfoHandle::DESTROY()

bool
TypeInfo::isA(TypeInfo* t)
  CODE:
    if(!THIS->isBound())
      XSRETURN_UNDEF;
    RETVAL = THIS->isA(t);
  OUTPUT:
    RETVAL

bool
TypeInfo::isBound()

bool
TypeInfo::_equal(TypeInfo* t)
  CODE:
    RETVAL = *THIS == *t;
  OUTPUT:
    RETVAL

bool
TypeInfo::_less_than(TypeInfo* t)
  CODE:
    RETVAL = *THIS < *t;
  OUTPUT:
    RETVAL

string
TypeInfo::getName()

void
TypeInfo::getChildren()
  PPCODE:
    const char* CLASS = "WorldForge::Eris::TypeInfo";
    const TypeInfoSet& children = THIS->getChildren();
    TypeInfoSet::const_iterator I;
    for(I = children.begin(); I != children.end(); ++I) {
      SV* child = sv_newmortal();
      sv_setref_pv(child, CLASS, (void*) *I);
      XPUSHs(child);
    }

int
TypeInfo::getTypeId()

SignalBase*
TypeInfo::getBoundSignal()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->getBoundSignal());
  OUTPUT:
    RETVAL

void
TypeInfo::getParentsAsSet()
  PPCODE:
    StringSet parents = THIS->getParentsAsSet();
    PUSH_STRING_SET(parents);
