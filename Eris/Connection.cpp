#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Connection.h>

#include <Eris/Timeout.h>
#include <Eris/TypeInfo.h>
#include <Eris/Poll.h>
#include <Eris/Log.h>
#include <Eris/Exceptions.h>
#include <Eris/Router.h>
#include <Eris/Redispatch.h>

#include <skstream/skstream.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Operation.h>
#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

#include <Atlas/Codecs/Bach.h>

#include <cassert>
#include <algorithm>

// #define ATLAS_LOG 1

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris {

Connection::Connection(const std::string &cnm, bool dbg) :
    BaseConnection(cnm, "game_", this),
    m_typeService(new TypeService(this)),
    m_defaultRouter(NULL),
    m_lock(0)
{	
    // SigC::slot(*this, &Account::handleLoginTimeout)
    Poll::instance().Ready.connect(SigC::slot(*this, &Connection::gotData));
}
	
Connection::~Connection()
{
    delete m_typeService;
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
    if (_status == DISCONNECTING) {
        warning() << "duplicate disconnect on Connection that's already disconnecting";
        return;
    }

    if (_status == DISCONNECTED)
        throw InvalidOperation("called disconnect on already disconnected Connection");

    assert(m_lock == 0);
	
    // this is a soft disconnect; it will give people a chance to do tear down and so on
    // in response, people who need to hold the disconnect will lock() the
    // connection, and unlock when their work is done. A timeout stops
    // locks from preventing disconnection
    setStatus(DISCONNECTING);
    Disconnecting.emit();

    if (m_lock == 0) {
        debug() << "no locks, doing immediate disconnection";
        hardDisconnect(true);
        return;
    }
    
    // fell through, so someone has locked =>
    // start a disconnect timeout
    _timeout = new Eris::Timeout("disconnect_" + _host, this, 5000);
    bindTimeout(*_timeout, DISCONNECTING);
}

void Connection::reconnect()
{
    if (_host.empty()) {
        handleFailure("Previous connection attempt failed, ignorning reconnect()");
    } else
        BaseConnection::connect(_host, _port);
}
   
void Connection::gotData(PollData &data)
{
    if (!_stream || !data.isReady(_stream))
        return;
	
    if (_status == DISCONNECTED) {
        error() << "Got data on a disconnected stream";
	return;
    }
   
    BaseConnection::recv();
    
// now dispatch recieved ops
    while (!m_opDeque.empty()) {
        dispatchOp(m_opDeque.front());
        m_opDeque.pop_front();
    }
    
// finally, clean up any redispatches that fired (aka 'deleteLater')
    for (unsigned int D=0; D < m_finishedRedispatches.size(); ++D)
        delete m_finishedRedispatches[D];
        
    m_finishedRedispatches.clear();
}		

void Connection::send(const Atlas::Objects::Root &obj)
{
    if ((_status != CONNECTED) && (_status != DISCONNECTING))
    {
        error() << "called send on closed connection";
        return;
    }

#ifdef ATLAS_LOG	
    std::stringstream debugStream;
    
    Atlas::Codecs::Bach debugCodec(debugStream, *this /*dummy*/);
    Atlas::Objects::ObjectsEncoder debugEncoder(debugCodec);
    debugEncoder.streamObjectsMessage(obj);
    debugStream << std::flush;

    std::cout << "sending:" << debugStream.str() << std::endl;
#endif
        
    _encode->streamObjectsMessage(obj);
    (*_stream) << std::flush;
}

void Connection::registerRouterForTo(Router* router, const std::string toId)
{
    m_toRouters[toId] = router;
}

void Connection::unregisterRouterForTo(Router* router, const std::string toId)
{
    assert(m_toRouters[toId] == router);
    m_toRouters.erase(toId);
}
        
void Connection::registerRouterForFrom(Router* router, const std::string fromId)
{
    m_fromRouters[fromId] = router;
}

void Connection::unregisterRouterForFrom(Router* router, const std::string fromId)
{
    assert(m_fromRouters[fromId] == router);
    m_fromRouters.erase(fromId);
}
        
void Connection::setDefaultRouter(Router* router)
{
    if (m_defaultRouter || !router) {
        error() << "setDefaultRouter duplicate set or null argument";
        return;
    }
    
    m_defaultRouter = router;
}

void Connection::clearDefaultRouter()
{
    m_defaultRouter = NULL;
}
    
void Connection::lock()
{
    ++m_lock;
}

void Connection::unlock()
{
    if (m_lock < 1)
        throw InvalidOperation("Imbalanced lock/unlock calls on Connection");
	
    if (--m_lock == 0) {
        switch (_status)
        {
        case DISCONNECTING:	
            debug() << "Connection unlocked in DISCONNECTING, closing socket";
            hardDisconnect(true);
            break;

        default:
            warning() << "Connection unlocked in spurious state : this may case a failure later";
        }
    }
}

#pragma mark -

void Connection::objectArrived(const Root& obj)
{
#ifdef ATLAS_LOG
    std::stringstream debugStream;
    Atlas::Codecs::Bach debugCodec(debugStream, *this /* dummy */);
    Atlas::Objects::ObjectsEncoder debugEncoder(debugCodec);
    debugEncoder.streamObjectsMessage(obj);
    debugStream << std::flush;

    std::cout << "recieved:" << debugStream.str() << std::endl;
#endif
    if (!m_typeService->verifyObjectTypes(obj)) return;
    
    RootOperation op = smart_dynamic_cast<RootOperation>(obj);
    if (op.isValid()) {
        m_opDeque.push_back(op);
    } else
        error() << "Con::objectArrived got non-op";
}

void Connection::dispatchOp(const RootOperation& op)
{
    Router::RouterResult rr;
    bool anonymous = op->getTo().empty();
    
// give the type service a go   
    /// @todo - wrap this in an anonymous=true guard?
    rr = m_typeService->handleOperation(op);
    if (rr == Router::HANDLED) return;
    
// locate a router based on from
    IdRouterMap::const_iterator R = m_fromRouters.find(op->getFrom());
    if (R != m_fromRouters.end()) {
        rr = R->second->handleOperation(op);
        if ((rr == Router::HANDLED) || (rr == Router::WILL_REDISPATCH))
            return;
    }
    
// locate a router based on the op's TO value
    R = m_toRouters.find(op->getTo());
    if (R != m_toRouters.end()) {
        rr = R->second->handleOperation(op);
        if ((rr == Router::HANDLED) || (rr == Router::WILL_REDISPATCH))
            return;
    } else if (!anonymous && !m_toRouters.empty())
        warning() << "recived op with TO=" << op->getTo() << ", but no router is registered for that id";
            
// go to the default router
    rr = m_defaultRouter->handleOperation(op);
    if (rr != Router::HANDLED)
        warning() << "no-one handled op:" << op;
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
        m_lock= 0;
}

void Connection::bindTimeout(Eris::Timeout &t, Status sc)
{
    // wire up all the stuff
    t.Expired.connect( SigC::bind(Timeout.slot(),sc) );
}

void Connection::onConnect()
{
    BaseConnection::onConnect();
    m_typeService->init();
}

void Connection::postForDispatch(const Root& obj)
{
    if (!m_typeService->verifyObjectTypes(obj)) {
        debug() << "amazingly, re-dispatched object failed to verify!";
        return;
    }
    
    RootOperation op = smart_dynamic_cast<RootOperation>(obj);
    m_opDeque.push_back(op);
}

void Connection::cleanupRedispatch(Redispatch* r)
{
    m_finishedRedispatches.push_back(r);
}

#pragma mark -

long getNewSerialno()
{
	static long _nextSerial = 1001;
	// note this will eventually loop (in theorey), but that's okay
	// FIXME - using the same intial starting offset is problematic
	// if the client dies, and quickly reconnects
	return _nextSerial++;
}

} // of namespace
