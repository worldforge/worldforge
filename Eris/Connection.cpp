#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <algorithm>

#include <skstream.h>
#include <sigc++/signal_system.h>
#include <sigc++/bind.h>

#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Message/Encoder.h>

#include <atlas_utils.h>

#include "Connection.h"
#include "Dispatcher.h"
#include "Wait.h"
#include "Timeout.h"
#include "Utils.h"
#include "TypeInfo.h"

#include "TypeDispatcher.h"
#include "ClassDispatcher.h"
#include "DebugDispatcher.h"
#include "SignalDispatcher.h"
#include "EncapDispatcher.h"

#ifdef __WIN32__

// Provide missing / misaligned function names.  May only be for mingw32
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif

#endif // __WIN32__

namespace Eris {

DebugDispatcher *dd, *sdd;	
	
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
	_logLevel(DEFAULT_LOG)
{
	// setup the singleton instance variable
	assert(_theConnection == NULL);
	_theConnection = this;
	
	// build the initial dispatch hierarchy
	_rootDispatch = new StdBranchDispatcher("root");
		
	Dispatcher *opd = new TypeDispatcher("op", "op");
	_rootDispatch->addSubdispatch(opd);
	opd = opd->addSubdispatch(ClassDispatcher::newAnonymous());
	
	Dispatcher *ifod = opd->addSubdispatch(new EncapDispatcher("info"), "info");
	ifod->addSubdispatch(new TypeDispatcher("entity", "object")); 
	ifod->addSubdispatch(new TypeDispatcher("op", "op"));
    
	Dispatcher *errd = opd->addSubdispatch(new StdBranchDispatcher("error"), "error");
	errd->addSubdispatch(new EncapDispatcher("encap", 1));
	
	if (_debug) {
		dd = new DebugDispatcher(_clientName + ".atlas-recvlog");
		sdd = new DebugDispatcher(_clientName + ".atlas-sendlog");
	}
}
	
Connection::~Connection()
{
	// clear the singleton instance pointer back to NULL
	// (becuase dereferencing deleted memory costs lives!)
	_theConnection = NULL;
	delete _rootDispatch;
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
	
	if (!_statusLock) {
		Eris::Log(LOG_NOTICE, "no locks, doing immediate disconnection");
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
		Eris::Log(LOG_ERROR, "Called Connection::reconnect() without prior sucessful connection");
		handleFailure("Previous connection attempt failed, ignorning reconnect()");
	} else
		BaseConnection::connect(_host, _port);
}

void Connection::poll()
{
	//if (!_stream) return;
	int err = 0;
	
	// sit in idle correctly
	if (_status == DISCONNECTED) return;
		
	if ((err = _stream->getLastError()) != 0) {
		handleFailure("Error reading from socket");
		hardDisconnect(false);
		return;
	}
	
	SOCKET_TYPE fd = _stream->getSocket();
	fd_set pending;
     	struct timeval tv = {0, 10};
      
     	FD_ZERO(&pending);
      	FD_SET(fd, &pending);
      
     	int retval = select(fd+1, &pending, NULL, NULL, &tv);
      	if (retval && FD_ISSET(fd, &pending))
		BaseConnection::recv();
	
	Timeout::pollAll();
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
	if (!d)
		throw InvalidOperation("Unknown dispatcher in path " + stem);
	
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
			Eris::Log(LOG_NOTICE, "Connection unlocked in DISCONNECTING, closing socket");
			hardDisconnect(true);
			break;
		
		default:
			Eris::Log(LOG_WARNING, "Connection unlocked in spurious state : this may case a failure later");
		}
}

Connection* Connection::Instance()
{
	if (_theConnection == NULL)
		throw InvalidOperation("No Connection instance exists");

	return _theConnection;
}

void Connection::setLogLevel(LogLevel lvl)
{
	_logLevel = lvl;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// protected / private gunk

void Connection::ObjectArrived(const Atlas::Message::Object& obj)
{
	Eris::Log(LOG_VERBOSE, "-");
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
		if (_logLevel >= LOG_VERBOSE) {
			std::string summary(objectSummary( Atlas::atlas_cast<Atlas::Objects::Root>(dq.front())));
			Eris::Log(LOG_VERBOSE, "Dispatching %s", summary.c_str());
		}
		
		try {
			// this invokes all manner of smoke and mirrors....
			_rootDispatch->dispatch(dq);
			
			if (_debug) {
				const Atlas::Message::Object::MapType &m(dq.back().AsMap());
				if (m.find("__DISPATCHED__") == m.end()) {
					std::string summary(objectSummary( Atlas::atlas_cast<Atlas::Objects::Root>(dq.front())));
					Eris::Log(LOG_WARNING, "op %s never hit a leaf node", summary.c_str());	
				}
			}
		} 
	
		catch (OperationBlocked &block) {
			std::string summary(objectSummary( Atlas::atlas_cast<Atlas::Objects::Root>(dq.front())));
			Eris::Log(LOG_VERBOSE, "Caugh OperationBlocked exception dispatching %s", summary.c_str());
			new WaitForSignal(block._continue, dq.back());
		}
	
		// catch actual failures, becuase they're bad.
		catch (BaseException &be) {
			Eris::Log(LOG_ERROR, "Dispatch: caught exception: %s", be._msg.c_str());
		}
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
	
	for (WaitForList::iterator Wnext = _waitList.begin(); Wnext != _waitList.end(); ) {
		WaitForList::iterator W = Wnext++;
		if ((*W)->isPending()) {
			delete *W;
			_waitList.erase(W);
		}
	}
	
	ccount -= _waitList.size();
	if (ccount)
		Eris::Log(LOG_VERBOSE, "Cleared %i signalled waitFors", ccount);
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
	TypeInfo::init();
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
		Eris::Log(LOG_WARNING, "recieved op [%s] from %s with no serial number set",
			summary.c_str(), sfm.first.c_str());
		return;
	}
	
	SerialFromSet::iterator S = seen.find(sfm);
	if (S != seen.end()) {
		std::string summary(objectSummary(op));
		Eris::Log(LOG_ERROR, "duplicate process of op [%s] from %s with serial# %i",
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

const int MSG_BUFFER_SIZE = 2048;	///< bounds for the static char[] buffers useds in vsnprintf

/** Log the specified printf() style string if the current LogLevel is sufficent. The client is free to attach
any (and several) outputs to the Connection::Log signal; notably to files, standard out, Quake style 'consoles',
etc, etc. Note this is purely informational - the client should never need to watch the log stream.*/
void Log(LogLevel lvl, const char *str, ...)
{
	if (Connection::Instance()->_logLevel < lvl) return;
	
	va_list args;
	va_start(args, str);
	
	static char buffer[MSG_BUFFER_SIZE];
	::vsnprintf(buffer, MSG_BUFFER_SIZE, str, args);
	
	Connection::Instance()->Log.emit(lvl, buffer);
	va_end(args);
}

} // of namespace
