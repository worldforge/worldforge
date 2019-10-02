#include <Eris/Connection.h>
#include <Eris/PollDefault.h>

#include <sigcperl/signal_wrap.h>

#include "../refcount.h"
#include "../roomhandle.h"

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
}

using namespace Eris;
using SigCPerl::SignalBase;
using std::string;

// This wraps the functionality of both Connection and BaseConnection.
// It only wraps the basic connect/disconnect functions at this point.

MODULE = WorldForge::Eris::Connection		PACKAGE = WorldForge::Eris::Connection

Connection*
Connection::new(string client_name, bool debug = false)
  CLEANUP:
    registerControlingSV(connection_hash_string, RETVAL, SvRV(ST(0)));

void
Connection::DESTROY()
  INIT:
    unregisterControlingSV(connection_hash_string, THIS);

void
Connection::connect(string host, short port = 6767)

void
Connection::reconnect()

void
Connection::disconnect()

SignalBase*
Connection::Connected()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Connected, ST(0));
  OUTPUT:
    RETVAL

SignalBase*
Connection::Disconnected()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Disconnected, ST(0));
  OUTPUT:
    RETVAL

SignalBase*
Connection::Failure()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Failure, ST(0));
  OUTPUT:
    RETVAL

SignalBase*
Connection::Disconnecting()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Disconnecting, ST(0));
  OUTPUT:
    RETVAL

Lobby*
Connection::getLobby()
  PREINIT:
    const char *CLASS = "WorldForge::Eris::Lobby";

char*
Connection::getStatus()
  CODE:
    switch(THIS->getStatus()) {
      case Eris::BaseConnection::NEGOTIATE:
        RETVAL = "Negotiate";
        break;
      case Eris::BaseConnection::CONNECTING:
        RETVAL = "Connecting";
        break;
      case Eris::BaseConnection::CONNECTED:
        RETVAL = "Connected";
        break;
      case Eris::BaseConnection::DISCONNECTING:
        RETVAL = "Disconnecting";
        break;
      case Eris::BaseConnection::DISCONNECTED:
        RETVAL = "Disconnected";
        break;
      default:
        RETVAL = "Invalid Status";
        break;
    }
  OUTPUT:
    RETVAL

bool
Connection::isConnected()

MODULE = WorldForge::Eris::Connection		PACKAGE = WorldForge::Eris::Poll

void
poll(unsigned timeout = 0)
  CODE:
    PollDefault::poll(timeout);
