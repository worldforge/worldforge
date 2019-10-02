#include <Eris/Metaserver.h>

#include <sigcperl/signal_wrap.h>

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
}

inline SV* string_to_sv(const std::string &str)
{
  return newSVpv(str.c_str(), str.size());
}

namespace SigCPerl {

// These are used in emit() by a signal which should
// never be user emitted. They convert an SV* into
// a reference which is really the nullptr pointer,
// and assert(false) in the process.
template<> inline ArgBox<Eris::ServerInfo*>::ArgBox(SV *val)
  throw(BadConvertVal) : m_val(0) {assert(false);}

template<> struct ArgBox<Eris::ServerInfo> : public ArgBox<Eris::ServerInfo*>
{
  typedef ArgBox<Eris::ServerInfo*> Parent;
  ArgBox(SV *sv) : Parent(sv) {}
  Eris::ServerInfo val() {return *Parent::val();}
  Eris::ServerInfo ret_val() {return *Parent::ret_val();}
};

template<> struct ArgBox<const Eris::ServerInfo&> : public ArgBox<Eris::ServerInfo*>
{
  typedef ArgBox<Eris::ServerInfo*> Parent;
  ArgBox(SV *sv) : Parent(sv) {}
  Eris::ServerInfo val() {return *Parent::val();}
  Eris::ServerInfo ret_val() {return *Parent::ret_val();}
};

static SV* GetSV(const Eris::ServerInfo &info)
{
  const char	hostname_key[] = "hostname",
		servername_key[] = "servername",
		ruleset_key[] = "ruleset",
		server_key[] = "server",
		clients_key[] = "clients",
		ping_key[] = "ping",
		uptime_key[] = "uptime";

  HV *val = newHV();

  hv_store(val, hostname_key, sizeof(hostname_key) - 1,
	string_to_sv(info.getHostname()),0);
  hv_store(val, servername_key, sizeof(servername_key) - 1,
	string_to_sv(info.getServername()),0);
  hv_store(val, ruleset_key, sizeof(ruleset_key) - 1,
	string_to_sv(info.getRuleset()),0);
  hv_store(val, server_key, sizeof(server_key) - 1,
	string_to_sv(info.getServer()),0);
  hv_store(val, clients_key, sizeof(clients_key) - 1,
	newSViv(info.getNumClients()),0);
  hv_store(val, ping_key, sizeof(ping_key) - 1,
	newSViv(info.getPing()),0);
  hv_store(val, uptime_key, sizeof(uptime_key) - 1,
	newSVnv(info.getUptime()),0);

  return newRV_noinc((SV*) val);
}

} // namespace SigCPerl

using namespace Eris;
using SigCPerl::SignalBase;
using std::string;

MODULE = WorldForge::Eris::Meta		PACKAGE = WorldForge::Eris::Meta		

Meta*
Meta::new(string cnm, string msv, unsigned int maxQueries)

void
Meta::DESTROY()

void
Meta::getGameServerList()
  PPCODE:
    ServerList list = THIS->getGameServerList();
    for(ServerList::iterator I = list.begin(); I != list.end(); ++I)
      XPUSHs(sv_2mortal(SigCPerl::GetSV(*I)));

int
Meta::getGameServerCount()

void
Meta::queryServer(string host)

void
Meta::refresh()

void
Meta::cancel()

string
Meta::getClientName()

string
Meta::getStatus()
  CODE:
    switch(THIS->getStatus()) {
      case VALID:
        RETVAL = "valid";
        break;
      case IN_PROGRESS:
        RETVAL = "in progress";
        break;
      case INVALID:
      default:
        XSRETURN_UNDEF;
    }
  OUTPUT:
    RETVAL

SignalBase*
Meta::ReceivedServerInfo()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->ReceivedServerInfo, ST(0));
  OUTPUT:
    RETVAL

SignalBase*
Meta::GotServerCount()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->GotServerCount, ST(0));
  OUTPUT:
    RETVAL

SignalBase*
Meta::CompletedServerList()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->CompletedServerList, ST(0));
  OUTPUT:
    RETVAL

SignalBase*
Meta::Failure()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Failure, ST(0));
  OUTPUT:
    RETVAL
