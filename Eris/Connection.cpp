#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <skstream.h>
#include <sigc++/signal_system.h>
#include <sigc++/bind.h>

#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Message/Encoder.h>

#include "Connection.h"
#include "Dispatcher.h"
#include "Wait.h"
#include "Timeout.h"

#include "TypeDispatcher.h"
#include "ClassDispatcher.h"
#include "DebugDispatcher.h"

namespace Eris {

DebugDispatcher *dd, *sdd;	
	
StringList tokenize(const string &s, char t);

// declare the static member
Connection* Connection::_theConnection = NULL;	


////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
Connection::Connection(const string &cnm) :
	BaseConnection(cnm, this),
	_statusLock(0)
{
	// setup the singleton instance variable
	assert(_theConnection == NULL);
	_theConnection = this;
	
	// build the initial dispatch hierarchy
	_rootDispatch = new Dispatcher("root");
	
	Dispatcher *opd = new TypeDispatcher("op", "op");
	_rootDispatch->addSubdispatch(opd);

	opd->addSubdispatch(new ClassDispatcher("info", "info"));
	opd->addSubdispatch(new ClassDispatcher("error", "error"));
	
	dd = new DebugDispatcher(_clientName + ".log");
	sdd = new DebugDispatcher(_clientName + ".sendlog");
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
		cerr << "doing immmediate disconnect" << endl;
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
	
	DispatchContextDeque dq(1, obj.AsObject());
	sdd->dispatch(dq);
}

void Connection::send(const Atlas::Message::Object &msg)
{
	if (_status != CONNECTED)
		throw InvalidOperation("Connection is not open");
	
	_msgEncode->StreamMessage(msg);
	(*_stream) << std::flush;
	
	DispatchContextDeque dq(1, msg);
	sdd->dispatch(dq);
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
	StringList tokens = tokenize(stem, ':');
	Dispatcher *d = _rootDispatch;
	
	while (!tokens.empty()) {
		d = d->getSubdispatch(tokens.front());	
		if (!d)
			throw InvalidOperation("Unknown dispatcher " + tokens.front() 
				+ " in path " + stem);
		tokens.pop_front();
	}
	
	Dispatcher *rm = d->getSubdispatch(n);
	if (!rm)
		throw InvalidOperation("Unknown dispatcher " + n + " at " + stem);
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
			hardDisconnect(true);
			break;
		
		default:
			cerr << "Connection unlocked in spurious state : no harm done (yet)"
				<< " but this indicates things are awry" << endl; 
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
/*
void Connection::ObjectArrived(const Atlas::Message::Object& obj)
{
	
	
	// stage 2 : having completed the primary dispatch, check for any dependants that
	// are now pending.
	
	try {
		// pick up an re-dispatches which have been intiated by the incoming message
		while (!_postQueue.empty()) {
			RepostDispatcher *rp = _postQueue.front();
			DispatchContextDeque pdq(1, rp->_post);	
			_rootDispatch->dispatch(pdq);
			
			// note this will delete rp!
			removeDispatcherByPath(rp->_removePath, rp->getName());
			_postQueue.pop_front();
		}
		
	} catch (BaseException &be) {
		cerr << "Dispatch: caught exception from post queue: " << be._msg << endl;
		DispatchContextDeque pdq(1, _postQueue.front());
		dd->dispatch(pdq);
	}

}
*/
void Connection::ObjectArrived(const Atlas::Message::Object& obj)
{
	postForDispatch(obj);
	
	while (!_repostQueue.empty()) {
		DispatchContextDeque dq(1, _repostQueue.front());
		dd->dispatch(dq);
		
		try {
			// this invokes all manner of smoke and mirrors....
			_rootDispatch->dispatch(dq);
		} 
	
		catch (OperationBlocked &block) {
			new WaitForSignal(block._continue, _repostQueue.front());
		}
	
		// catch actual failures, becuase they're bad.
		catch (BaseException &be) {
			cerr << "Dispatch: caught exception: " << be._msg << endl;
			dd->dispatch(dq);
		}
		
		_repostQueue.pop_front();
	}
	
	clearSignalledWaits();
}

void Connection::clearSignalledWaits()
{
	for (WaitForList::iterator I=_waitList.begin(); I!=_waitList.end(); ) {
		WaitForList::iterator cur = I++;
		if ((*cur)->isPending()) {
			delete (*cur);
			_waitList.erase(cur);
		}
	}
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

void Connection::postForDispatch(const Atlas::Message::Object &msg)
{
	_repostQueue.push_back(msg);
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

}; // of namespace
