#include <Eris/Timeout.h>

#include <sigcperl/signal_wrap.h>

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
}

using std::string;
using SigCPerl::SignalBase;
using Eris::Timeout;

MODULE = WorldForge::Eris::Timeout		PACKAGE = WorldForge::Eris::Timeout		

Timeout*
Timeout::new(string label, unsigned long milli)

void
Timeout::DESTROY()

void
Timeout::extend(unsigned long msec)

void
Timeout::reset(unsigned long msec)

bool
Timeout::isExpired()

SignalBase*
Timeout::Expired()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Expired, ST(0));
  OUTPUT:
    RETVAL
