#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Connection.h"

#include "TypeInfo.h"
#include "Log.h"
#include "Exceptions.h"
#include "Router.h"
#include "Redispatch.h"
#include "Response.h"
#include "EventService.h"

#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <sigc++/bind.h>

#include <Atlas/Codecs/Bach.h>

#include <cassert>
#include <algorithm>

#define ATLAS_LOG 0

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris {


struct ConnectionDecoder : Atlas::Objects::ObjectsDecoder {
	Connection& m_connection;

	ConnectionDecoder(Connection& connection, const Atlas::Objects::Factories& factories) :
			ObjectsDecoder(factories), m_connection(connection) {
	}

	void objectArrived(const Root& obj) override {
		m_connection.objectArrived(obj);
	}
};

Connection::Connection(boost::asio::io_service& io_service,
					   EventService& eventService,
					   std::string clientName,
					   const std::string& host,
					   short port) :
		BaseConnection(io_service, std::move(clientName), "game_"),
		m_decoder(new ConnectionDecoder(*this, *_factories)),
		_eventService(eventService),
		_host(host),
		_port(port),
		m_typeService(new TypeService(this)),
		m_defaultRouter(nullptr),
		m_lock(0),
		m_info(host),
		m_responder(new ResponseTracker) {
	_bridge = m_decoder.get();
}

Connection::Connection(boost::asio::io_service& io_service,
					   EventService& eventService,
					   std::string clientName,
					   std::string socket) :
		BaseConnection(io_service, std::move(clientName), "game_"),
		m_decoder(new ConnectionDecoder(*this, *_factories)),
		_eventService(eventService),
		_host("local"),
		_port(0),
		_localSocket(std::move(socket)),
		m_typeService(new TypeService(this)),
		m_defaultRouter(nullptr),
		m_lock(0),
		m_info(_host),
		m_responder(new ResponseTracker) {
	_bridge = m_decoder.get();
}


Connection::~Connection() {
	// ensure we emit this before our vtable goes down, since we are the
	// Bridge on the underlying Atlas codec, and otherwise we might get
	// a pure virtual method call
	hardDisconnect(true);
}

EventService& Connection::getEventService() {
	return _eventService;
}

int Connection::connect() {
	if (!_localSocket.empty()) {
		return BaseConnection::connectLocal(_localSocket);
	}

	return BaseConnection::connectRemote(_host, _port);

}

int Connection::disconnect() {
	if (_status == DISCONNECTING) {
		warning() << "duplicate disconnect on Connection that's already disconnecting";
		return -1;
	}

	if (_status == DISCONNECTED) {
		warning() << "called disconnect on already disconnected Connection";
		return -1;
	}

	// This assert means that this function will always return early below
	// where m_lock is checked. m_lock seems to be used by Account to prevent
	// disconnecting when something is pending.
	// FIXME Look into this.
	assert(m_lock == 0);

	if (_socket && _status == CONNECTED) {
		//Be nice and send a Logout op to the connection when disconnecting down.
		_socket->getEncoder().streamObjectsMessage(Logout());
		_socket->write();
	}

	// this is a soft disconnect; it will give people a chance to do tear down and so on
	// in response, people who need to hold the disconnect will lock() the
	// connection, and unlock when their work is done. A timeout stops
	// locks from preventing disconnection
	setStatus(DISCONNECTING);
	Disconnecting.emit();

	if (m_lock == 0) {
		hardDisconnect(true);
		return 0;
	}

	// fell through, so someone has locked =>
	// start a disconnect timeout
//    _timeout = new Timeout(5000);
//    _timeout->Expired.connect(sigc::mem_fun(this, &Connection::onDisconnectTimeout));
	return 0;
}

void Connection::dispatch() {

	// now dispatch received ops
	while (!m_opDeque.empty()) {
		RootOperation op = std::move(m_opDeque.front());
		m_opDeque.pop_front();
		dispatchOp(op);
	}

	// finally, clean up any redispatches that fired (aka 'deleteLater')
	for (auto& finishedRedispatch : m_finishedRedispatches) {
		delete finishedRedispatch;
	}

	m_finishedRedispatches.clear();
}

void Connection::send(const Atlas::Objects::Root& obj) {
	if ((_status != CONNECTED) && (_status != DISCONNECTING)) {
		error() << "called send on closed connection";
		return;
	}

	if (!_socket) {
		handleFailure("Connection::send: stream failed");
		hardDisconnect(true);
		return;
	}

#if ATLAS_LOG == 1
	std::stringstream debugStream;

	Atlas::Codecs::Bach debugCodec(debugStream, debugStream, *this /*dummy*/);
	Atlas::Objects::ObjectsEncoder debugEncoder(debugCodec);
	debugEncoder.streamObjectsMessage(obj);
	debugStream << std::flush;

	debug() << "sending:" << debugStream.str();
#endif

	_socket->getEncoder().streamObjectsMessage(obj);
	_socket->write();
}

void Connection::registerRouterForTo(Router* router, const std::string& toId) {
	m_toRouters[toId] = router;
}

void Connection::unregisterRouterForTo(Router* router, const std::string& toId) {
	assert(m_toRouters[toId] == router);
	m_toRouters.erase(toId);
}

void Connection::registerRouterForFrom(Router* router, const std::string& fromId) {
	m_fromRouters[fromId] = router;
}

void Connection::unregisterRouterForFrom(Router* router, const std::string& fromId) {
	assert(m_fromRouters[fromId] == router);
	m_fromRouters.erase(fromId);
}

void Connection::setDefaultRouter(Router* router) {
	if (m_defaultRouter || !router) {
		error() << "setDefaultRouter duplicate set or null argument";
		return;
	}

	m_defaultRouter = router;
}

void Connection::clearDefaultRouter() {
	m_defaultRouter = nullptr;
}

void Connection::lock() {
	++m_lock;
}

void Connection::unlock() {
	if (m_lock < 1) {
		throw InvalidOperation("Imbalanced lock/unlock calls on Connection");
	}

	if (--m_lock == 0) {
		switch (_status) {
			case DISCONNECTING:
				debug() << "Connection unlocked in DISCONNECTING, closing socket";
				debug() << "have " << m_opDeque.size() << " ops waiting";
				m_opDeque.clear();
				hardDisconnect(true);
				break;

			default:
				warning() << "Connection unlocked in spurious state : this may cause a failure later";
				break;
		}
	}
}

void Connection::getServerInfo(ServerInfo& si) const {
	si = m_info;
}

void Connection::refreshServerInfo() {
	if (_status != CONNECTED) {
		warning() << "called refreshServerInfo while not connected, ignoring";
		return;
	}

	m_info.setStatus(ServerInfo::QUERYING);
	Get gt;
	gt->setSerialno(getNewSerialno());
	send(gt);
}

void Connection::objectArrived(const Root& obj) {
#if ATLAS_LOG == 1
	std::stringstream debugStream;
	Atlas::Codecs::Bach debugCodec(debugStream, debugStream, *this /* dummy */);
	Atlas::Objects::ObjectsEncoder debugEncoder(debugCodec);
	debugEncoder.streamObjectsMessage(obj);
	debugStream << std::flush;

	debug() << "received:" << debugStream.str();
#else
	debug() << "received op:" << obj->getParent();
#endif
	RootOperation op = smart_dynamic_cast<RootOperation>(obj);
	if (op.isValid()) {
		m_opDeque.push_back(op);
	} else {
		error() << "Con::objectArrived got non-op";
	}
}

void Connection::dispatchOp(const RootOperation& op) {
	try {
		bool anonymous = op->isDefaultTo();

		Router::RouterResult rr = m_responder->handleOp(op);
		if (rr == Router::HANDLED) {
			return;
		}

		// locate a router based on from
		if (!op->isDefaultFrom()) {
			auto R = m_fromRouters.find(op->getFrom());
			if (R != m_fromRouters.end()) {
				rr = R->second->handleOperation(op);
				if (rr == Router::HANDLED) {
					return;
				}
			}
		}

		// locate a router based on the op's TO value
		if (!anonymous) {
			auto R = m_toRouters.find(op->getTo());
			if (R != m_toRouters.end()) {
				rr = R->second->handleOperation(op);
				if (rr == Router::HANDLED) {
					return;
				}
			} else if (!m_toRouters.empty()) {
				warning() << "received op with TO=" << op->getTo() << ", but no router is registered for that id";
			}
		}

		// special-case, server info refreshes are handled here directly
		if (op->instanceOf(INFO_NO) && anonymous) {
			handleServerInfo(op);
			return;
		}

		// go to the default router
		if (m_defaultRouter) {
			rr = m_defaultRouter->handleOperation(op);
		}
		if (rr != Router::HANDLED) {
			warning() << "no-one handled op:" << op;
		}
	} catch (const Atlas::Exception& ae) {
		error() << "caught Atlas exception: '" << ae.getDescription() <<
				"' while dispatching op:\n" << op;
	}
}


void Connection::setStatus(Status ns) {
	if (_status != ns) StatusChanged.emit(ns);
	_status = ns;
}

void Connection::handleFailure(const std::string& msg) {
	Failure.emit(msg);
	// FIXME - reset I think, but ensure this is safe
	m_lock = 0;
}

void Connection::handleTimeout(const std::string& msg) {
	handleFailure(msg); // all the same in the end
}

void Connection::handleServerInfo(const RootOperation& op) {
	if (!op->getArgs().empty()) {
		RootEntity svr = smart_dynamic_cast<RootEntity>(op->getArgs().front());
		if (!svr.isValid()) {
			error() << "server INFO argument object is broken";
			return;
		}

		m_info.processServer(svr);
		GotServerInfo.emit();
	}
}

void Connection::onConnect() {
	BaseConnection::onConnect();
	m_typeService->init();
	m_info = ServerInfo(_host);
}

void Connection::onDisconnectTimeout() {
	handleTimeout("timed out waiting for disconnection");
	hardDisconnect(true);
}

void Connection::postForDispatch(const Root& obj) {
	RootOperation op = smart_dynamic_cast<RootOperation>(obj);
	assert(op.isValid());
	m_opDeque.push_back(op);

#if ATLAS_LOG == 1
	std::stringstream debugStream;
	Atlas::Codecs::Bach debugCodec(debugStream, debugStream, *this /* dummy */);
	Atlas::Objects::ObjectsEncoder debugEncoder(debugCodec);
	debugEncoder.streamObjectsMessage(obj);
	debugStream << std::flush;

	debug() << "posted for re-dispatch:" << debugStream.str();
#endif
}

void Connection::cleanupRedispatch(Redispatch* r) {
	m_finishedRedispatches.push_back(r);
}

long getNewSerialno() {
	static long _nextSerial = 1001;
	// note this will eventually loop (in theory), but that's okay
	// FIXME - using the same intial starting offset is problematic
	// if the client dies, and quickly reconnects
	return _nextSerial++;
}

} // of namespace
