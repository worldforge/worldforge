#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <algorithm>

#include <skstream/skstream.h>
#include <sigc++/bind.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/signal_system.h>
#else
#include <sigc++/signal.h>
#endif

#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Message/Encoder.h>

#include "atlas_utils.h"

#include "Connection.h"
#include "Dispatcher.h"
#include "Wait.h"
#include "Timeout.h"
#include "Utils.h"
#include "TypeInfo.h"
#include "Poll.h"
#include "PollDefault.h"
#include "Log.h"

#include "TypeDispatcher.h"
#include "ClassDispatcher.h"
#include "DebugDispatcher.h"
#include "SignalDispatcher.h"
#include "EncapDispatcher.h"

#include "Lobby.h"

namespace Eris {

DebugDispatcher *dd = NULL, *sdd = NULL;	
	
StringList tokenize(const std::string &s, char t);

// declare the static member
Connection* Connection::_theConnection = NULL;	

typedef std::pair<std::string, long> SerialFrom;
typedef std::set<SerialFrom> SerialFromSet;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
Connection::Connection(const std::string &cnm, bool dbg) :
	BaseConnection(cnm, "game_", this),
	_statusLock(0),
	_debug(dbg),
	_ti_engine(new TypeInfoEngine(this)),
	_lobby(new Lobby(this))
{
	// setup the singleton instance variable
	if (_theConnection == NULL)
		_theConnection = this;
	
	// build the initial dispatch hierarchy
	_rootDispatch = new StdBranchDispatcher("root");
		
	Dispatcher *opd = new TypeDispatcher("op", "op");
	_rootDispatch->addSubdispatch(opd);
	opd = opd->addSubdispatch(ClassDispatcher::newAnonymous(this));
	
	Dispatcher *ifod = opd->addSubdispatch(new EncapDispatcher("info"), "info");
	ifod->addSubdispatch(new TypeDispatcher("entity", "object")); 
	ifod->addSubdispatch(new TypeDispatcher("op", "op"));
    
	Dispatcher *errd = opd->addSubdispatch(new StdBranchDispatcher("error"), "error");
	errd->addSubdispatch(new EncapDispatcher("encap", 1));
	
	if (_debug) {
		dd = new DebugDispatcher(_clientName + ".atlas-recvlog");
		sdd = new DebugDispatcher(_clientName + ".atlas-sendlog");
	}
	
    Poll::instance().connect(SigC::slot(*this, &Connection::gotData));
}
	
Connection::~Connection()
{
    // clear the singleton instance pointer back to NULL
    // (becuase dereferencing deleted memory costs lives!)
    if(_theConnection == this)
	_theConnection = NULL;
    delete _lobby;
    delete _ti_engine;
    delete _rootDispatch;
    if (_debug) {
      delete dd; dd = NULL;
      delete sdd; sdd = NULL;
    }
}


void Connection::connect(const std::string &host, short port)
{
    // save for reconnection later
    _host = host;
    _port = port;
    
    BaseConnection::connect(host, port);
}

void Connection::disconnect()
{
	assert(_statusLock == 0);
	_statusLock = 0;
	
	// this is a soft disconnect; it will give people a chance to do tear down and so on
	// in response, people who need to hold the disconnect will lock() the
	// connection, and unlock when their work is done. A timeout stops
	// locks from preventing disconnection
	setStatus(DISCONNECTING);
	Disconnecting.emit();
    
	if (!_statusLock) {
		Eris::log(LOG_NOTICE, "no locks, doing immediate disconnection");
		hardDisconnect(true);
		return;
	}
	
	// fell through, so someone has locked =>
	// start a disconnect timeout
	_timeout = new Eris::Timeout("disconnect_" + _host, 5000);
	bindTimeout(*_timeout, DISCONNECTING);
}

void Connection::reconnect()
{
	if (_host.empty()) {
		Eris::log(LOG_ERROR, "Called Connection::reconnect() without prior sucessful connection");
		handleFailure("Previous connection attempt failed, ignorning reconnect()");
	} else
		BaseConnection::connect(_host, _port);
}
   
void Connection::gotData(PollData &data)
{
    if(!_stream || !data.isReady(_stream))
		return;
	
	else if (_status == DISCONNECTED)
		Eris::log(LOG_ERROR, "Got data on a disconnected stream");
	else
		BaseConnection::recv();
}		

void Connection::send(const Atlas::Objects::Root &obj)
{
	if (_status != CONNECTED)
		throw InvalidOperation("Connection is not open");
	
	_encode->StreamMessage(&obj);
	(*_stream) << std::flush;
	
	if (_debug) {
		DispatchContextDeque dq(1, obj.AsObject());
		sdd->dispatch(dq);
	}
}

void Connection::send(const Atlas::Message::Object &msg)
{
	if (_status != CONNECTED)
		throw InvalidOperation("Connection is not open");
	
	_msgEncode->StreamMessage(msg);
	(*_stream) << std::flush;
	
	if (_debug) {
		DispatchContextDeque dq(1, msg);
		sdd->dispatch(dq);
	}
}

Dispatcher* Connection::getDispatcherByPath(const std::string &path) const
{
	if (path.empty() || (":" == path))
		return _rootDispatch;
	
	StringList tokens = tokenize(path, ':');
	Dispatcher *d = _rootDispatch;
	
	while (!tokens.empty()) {
		d = d->getSubdispatch(tokens.front());
		if (!d)
			return NULL;
		tokens.pop_front();
	}
	
	return d;
}

/** Remove a node from the dispatcher tree. Throws InvalidOperation if path is
invalid, or the the node is not found. */
void Connection::removeDispatcherByPath(const std::string &stem, const std::string &n)
{
	Dispatcher *d = getDispatcherByPath(stem);
	if (!d)
		throw InvalidOperation("Unknown dispatcher in path " + stem);
	
	Dispatcher *rm = d->getSubdispatch(n);
	if (!rm)
		throw InvalidOperation("Unknown dispatcher " + n + " at " + stem);
	
	d->rmvSubdispatch(rm);
}

void Connection::removeIfDispatcherByPath(const std::string &stem, const std::string &n)
{
	Dispatcher *d = getDispatcherByPath(stem);
	if (!d) {
		// this used to throw, but this allows very robust clean-ups (silently, hmm)
		return;
	}
	
	Dispatcher *rm = d->getSubdispatch(n);
	if (rm)
	    d->rmvSubdispatch(rm);
}

void Connection::lock()
{
	_statusLock++;
}

void Connection::unlock()
{
	if (_statusLock < 1)
		throw InvalidOperation("Imbalanced lock/unlock calls on Connection");
	--_statusLock;
	
	if (!_statusLock) 
		switch (_status) {
		case DISCONNECTING:	
			Eris::log(LOG_NOTICE, "Connection unlocked in DISCONNECTING, closing socket");
			hardDisconnect(true);
			break;

		default:
			Eris::log(LOG_WARNING, "Connection unlocked in spurious state : this may case a failure later");
		}
}

Connection* Connection::Instance()
{
	if (_theConnection == NULL)
		throw InvalidOperation("No Connection instance exists");

	return _theConnection;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// protected / private gunk

void Connection::ObjectArrived(const Atlas::Message::Object& obj)
{
	Eris::log(LOG_VERBOSE, "-");
	postForDispatch(obj);
    
	if (_debug) {
		Atlas::Objects::Operation::RootOperation op = 
			Atlas::atlas_cast<Atlas::Objects::Operation::RootOperation>(obj);
		validateSerial(op);
	}
	
	while (!_repostQueue.empty()) {
		DispatchContextDeque dq(1, _repostQueue.front());
		_repostQueue.pop_front();
		
		if (_debug)
			dd->dispatch(dq);
		
		// manual check becuase objectSummary is a minor hit
		if (getLogLevel() >= LOG_VERBOSE) {
		    std::string summary(objectSummary( Atlas::atlas_cast<Atlas::Objects::Root>(dq.front())));
		    Eris::log(LOG_VERBOSE, "Dispatching %s", summary.c_str());
		}
		
		Dispatcher::enter();
		
		try {
		    
			// this invokes all manner of smoke and mirrors....
			_rootDispatch->dispatch(dq);
			
			if (_debug) {
				const Atlas::Message::Object::MapType &m(dq.back().AsMap());
				if (m.find("__DISPATCHED__") == m.end()) {
					std::string summary(objectSummary( Atlas::atlas_cast<Atlas::Objects::Root>(dq.front())));
					Eris::log(LOG_WARNING, "op %s never hit a leaf node", summary.c_str());	
				}
			}
		} 
	
		catch (OperationBlocked &block) {
			std::string summary(objectSummary( Atlas::atlas_cast<Atlas::Objects::Root>(dq.front())));
			Eris::log(LOG_VERBOSE, "Caugh OperationBlocked exception dispatching %s", summary.c_str());
			new WaitForSignal(block._continue, dq.back(), this);
		}
	
		// catch actual failures, becuase they're bad.
		catch (BaseException &be) {
			Eris::log(LOG_ERROR, "Dispatch: caught exception: %s", be._msg.c_str());
		}
		
		Dispatcher::exit();
	}
	
	clearSignalledWaits();
}

void Connection::addWait(WaitForBase *w)
{
	assert(w);
	_waitList.push_front(w);
}

void Connection::clearSignalledWaits()
{
	int ccount = _waitList.size();
	
	for (WaitForList::iterator W = _waitList.begin(); W != _waitList.end(); ) {
	    if ((*W)->isPending()) {
		    delete *W;
		    W = _waitList.erase(W);
	    } else
		++W;
	}
	
	ccount -= _waitList.size();
	if (ccount)
		Eris::log(LOG_VERBOSE, "Cleared %i signalled waitFors", ccount);
}

void Connection::setStatus(Status ns)
{
	if (_status != ns) {
		StatusChanged.emit(ns);
	}
	_status = ns;
}

void Connection::handleFailure(const std::string &msg)
{
	Failure.emit(msg);
	
	if ((_status == CONNECTING) || (_status == NEGOTIATE)) {
		// got a failure during connection; therefore, clear
		// the reconnect data
		_host = "";
	}
	
	// FIXME - reset I think, but ensure this is safe
	_statusLock = 0;
}

void Connection::bindTimeout(Eris::Timeout &t, Status sc)
{
	// wire up all the stuff
	t.Expired.connect( SigC::bind(Timeout.slot(),sc) );
}

void Connection::onConnect()
{
	BaseConnection::onConnect();
	_ti_engine->init();
}

void Connection::postForDispatch(const Atlas::Message::Object &msg)
{
	_repostQueue.push_back(msg);
}

void Connection::validateSerial(const Atlas::Objects::Operation::RootOperation &op)
{
	static SerialFromSet seen;
	SerialFrom sfm(op.GetFrom(), op.GetSerialno());
	
	// don't bother to validate if the serial-no is 0
	if (sfm.second == 0) {
		std::string summary(objectSummary(op));
		Eris::log(LOG_WARNING, "recieved op [%s] from %s with no serial number set",
			summary.c_str(), sfm.first.c_str());
		return;
	}
	
	SerialFromSet::iterator S = seen.find(sfm);
	if (S != seen.end()) {
		std::string summary(objectSummary(op));
		Eris::log(LOG_ERROR, "duplicate process of op [%s] from %s with serial# %i",
			summary.c_str(), sfm.first.c_str(), sfm.second);
	} else
		seen.insert(sfm);
	
}

/** strtok for the next generation : uses STL strings and returns a list*/
StringList tokenize(const std::string &s, char t)
{
	StringList ret;
	
	unsigned int pos = 0, back = pos;
	while (pos < s.size()) {
		pos = s.find(t, back);
		
		// addthe next part
		ret.push_back(s.substr(back, pos - back));
		back = pos + 1;
	}
	
	return ret;
}

} // of namespace
