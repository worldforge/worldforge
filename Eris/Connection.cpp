#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

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

namespace Eris {

DebugDispatcher *dd, *sdd;	
	
StringList tokenize(const string &s, char t);

// declare the static member
Connection* Connection::_theConnection = NULL;	

typedef std::pair<std::string, long> SerialFrom;
typedef std::set<SerialFrom> SerialFromSet;
	
////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
Connection::Connection(const string &cnm) :
	BaseConnection(cnm, "game_", this),
	_statusLock(0),
	_debug(true)
{
	// setup the singleton instance variable
	assert(_theConnection == NULL);
	_theConnection = this;
	
	// build the initial dispatch hierarchy
	_rootDispatch = new Dispatcher("root");
		
	Dispatcher *opd = new TypeDispatcher("op", "op");
	_rootDispatch->addSubdispatch(opd);

	if (_debug) {
		opd->addSubdispatch(new SignalDispatcher<Atlas::Objects::Operation::RootOperation>(
			"serial-validator", SigC::slot(this, &Connection::validateSerial)
		));
	}
				
	opd->addSubdispatch(new EncapDispatcher("info", "info"));
	opd->addSubdispatch(new ClassDispatcher("error", "error"));
		
	dd = new DebugDispatcher(_clientName + ".atlas-recvlog");
	sdd = new DebugDispatcher(_clientName + ".atlas-sendlog");
}
	
Connection::~Connection()
{
	// clear the singleton instance pointer back to NULL
	// (becuase dereferencing deleted memory costs lives!)
	_theConnection = NULL;
	delete _rootDispatch;
}


void Connection::connect(const string &host, short port)
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
		Eris::Log("no locks, doing immediate disconnection");
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
	if (_host.empty())
		throw InvalidOperation("Called Reconnect() without prior completion of connection");
	
	BaseConnection::connect(_host, _port);
}

void Connection::poll()
{
	//if (!_stream) return;
	int err = 0;
	
	// sit in idle correctly
	if (_status == DISCONNECTED) return;
		
	if ((err = _stream->getLastError()) != 0) {
		handleFailure("Stream error");
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

Dispatcher* Connection::getDispatcherByPath(const string &path) const
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
void Connection::removeDispatcherByPath(const string &stem, const string &n)
{
	Dispatcher *d = getDispatcherByPath(stem);
	if (!d)
		throw InvalidOperation("Unknown dispatcher in path " + stem);
	
	Dispatcher *rm = d->getSubdispatch(n);
	if (!rm)
		throw InvalidOperation("Unknown dispatcher " + n + " at " + stem);
	d->rmvSubdispatch(rm);
}

void Connection::removeIfDispatcherByPath(const string &stem, const string &n)
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
			Eris::Log("Connection unlocked in DISCONNECTING, closing socket");
			hardDisconnect(true);
			break;
		
		default:
			Eris::Log("Connection unlocked in spurious state : this may case a failure later");
		}
}

Connection* Connection::Instance()
{
	if (_theConnection)
		return _theConnection;
	else
		throw InvalidOperation("No Connection instance exists");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// protected / private gunk

void Connection::ObjectArrived(const Atlas::Message::Object& obj)
{
	Eris::Log("-");
	postForDispatch(obj);

	while (!_repostQueue.empty()) {
		DispatchContextDeque dq(1, _repostQueue.front());
		_repostQueue.pop_front();
		
		if (_debug) {	
			dd->dispatch(dq);
		
			std::string summary(objectSummary( Atlas::atlas_cast<Atlas::Objects::Root>(dq.front())));
			Eris::Log("Dispatching %s", summary.c_str());
		}
		
		try {
			// this invokes all manner of smoke and mirrors....
			_rootDispatch->dispatch(dq);
			
			if (_debug) {
				const Atlas::Message::Object::MapType &m(dq.front().AsMap());
				if (m.find("__DISPATCHED__") == m.end()) {
					std::string summary(objectSummary( Atlas::atlas_cast<Atlas::Objects::Root>(dq.front())));
					Eris::Log("WARNING : op %s never hit a leaf node", summary.c_str());	
				}
			}
		} 
	
		catch (OperationBlocked &block) {
			std::string summary(objectSummary( Atlas::atlas_cast<Atlas::Objects::Root>(dq.front())));
			Eris::Log("Caugh OperationBlocked exception dispatching %s", summary.c_str());
			new WaitForSignal(block._continue, dq.back());
		}
	
		// catch actual failures, becuase they're bad.
		catch (BaseException &be) {
			Eris::Log("Dispatch: caught exception: %s", be._msg.c_str());
			dd->dispatch(dq);
		}
	}
	
	clearSignalledWaits();
}

void Connection::addWait(WaitForBase *w)
{
	assert(w);
	_waitList.push_front(w);
}

void deleteFiredWait(WaitForBase *w)
{
	assert(w);
	if (w->isPending())
		delete w;
}

void Connection::clearSignalledWaits()
{
	int ccount = _waitList.size();
	
	std::for_each(_waitList.begin(), _waitList.end(), deleteFiredWait); 
	
	_waitList.erase(
		std::remove_if(_waitList.begin(), _waitList.end(), WaitForBase::hasFired), 
		_waitList.end());
	
	ccount -= _waitList.size();
	if (ccount)
		Eris::Log("Cleared %i signalled waitFors", ccount);
}

void Connection::setStatus(Status ns)
{
	if (_status != ns) {
		StatusChanged.emit(ns);
	}
	_status = ns;
}

void Connection::handleFailure(const string &msg)
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
		Eris::Log("WARNING: recieved op [%s] from %s with no serial number set",
			summary.c_str(), sfm.first.c_str());
		return;
	}
	
	SerialFromSet::iterator S = seen.find(sfm);
	if (S != seen.end()) {
		assert(false);
		std::string summary(objectSummary(op));
		Eris::Log("ERROR: duplicate process of op [%s] from %s with serial# %i",
			summary.c_str(), sfm.first.c_str(), sfm.second);
	} else
		seen.insert(sfm);
	
}

StringList tokenize(const string &s, char t)
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

/*
void _Pending::Trigger()
{
	con->ObjectArrived(msg);
	con->Delete
}
*/

void Log(const char *str, ...)
{
	va_list args;
	va_start(args, str);
	
	char buffer[1024];
	::vsnprintf(buffer, 1024, str, args);
	
	Connection::Instance()->Log.emit(buffer);
	va_end(args);
}

}; // of namespace
