#include <Eris/Log.h>

#include <sigcperl/signal_wrap.h>

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
}

static Eris::LogLevel string_to_log_level(const std::string& level)
{
  if(level == "error")
    return Eris::LOG_ERROR;
  else if(level == "notice")
    return Eris::LOG_NOTICE;
  else if(level == "verbose")
    return Eris::LOG_VERBOSE;
  else if(level == "debug")
    return Eris::LOG_DEBUG;
  else
    return Eris::LOG_WARNING;
}

static std::string log_level_to_string(Eris::LogLevel level)
{
  switch(level) {
    case Eris::LOG_ERROR:
      return "error";
    case Eris::LOG_WARNING:
      return "warning";
    case Eris::LOG_NOTICE:
      return "notice";
    case Eris::LOG_VERBOSE:
      return "verbose";
    case Eris::LOG_DEBUG:
      return "debug";
    default:
      assert(false);
  }
}

using namespace Eris;
using std::string;
using SigCPerl::SignalBase;

// Conversion for 'Logged' signal
template<> SigCPerl::ArgBox<LogLevel>::ArgBox(SV *val) throw()
	: m_val(string_to_log_level(SigCPerl::get_string(val))) {}
SV* GetSV(LogLevel val) throw() {return SigCPerl::GetSV(log_level_to_string(val));}

MODULE = WorldForge::Eris::Log		PACKAGE = WorldForge::Eris::Log

void
setLogLevel(string level)
  CODE:
    setLogLevel(string_to_log_level(level));

string
getLogLevel()
  CODE:
    RETVAL = log_level_to_string(getLogLevel());
  OUTPUT:
    RETVAL

SignalBase*
Logged()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(Logged);
  OUTPUT:
    RETVAL
