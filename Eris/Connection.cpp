#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Connection.h>

#include <Eris/Wait.h>
#include <Eris/Timeout.h>
#include <Eris/Utils.h>
#include <Eris/TypeInfo.h>
#include <Eris/Poll.h>
#include <Eris/Log.h>
#include <Eris/Exceptions.h>
#include <Eris/router.h>

#include <skstream/skstream.h>

#include <sigc++/bind.h>
#include <sigc++/object_slot.h>

#include <cassert>
#include <algorithm>

namespace Eris {

Connection::Connection(const std::string &cnm, bool dbg, Router* dr) :
	BaseConnection(cnm, "game_", this),
	_statusLock(0),
	_debug(dbg),
    m_typeService(new TypeService(this)),
    m_defaultRouter(dr)
{	
    Poll::instance().connect(SigC::slot(*this, &Connection::gotData));
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
	_timeout = new Eris::Timeout("disconnect_" + _host, this, 5000);
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
}		

void Connection::send(const Atlas::Objects::Root &obj)
{
    if ((_status != CONNECTED) && (_status != DISCONNECTING))
    {
        error() << "called send on closed connection";
        return;
    }
	
    _encode->streamMessage(&obj);
    (*_stream) << std::flush;
}

void Connection::registerRouterForTo(Router* router, const std::string toId)
{
    m_toRouters[toId] = router;
}
        
void Connection::registerRouterForFrom(Router* router, const std::string fromId)
{
    m_fromRouters[fromId] = router;
}
        
void Connection::setDefaultRouter(Router* router)
{
    if (m_defaultRouter || !router)
    {
        error() << "setDefaultRoute duplicate set or null argument";
        return;
    }
    
    m_defaultRoute = router;
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

#pragma mark -

void Connection::objectArrived(const Atlas::Objects::Root& obj)
{
    Operation::RootOperation op = smart_dynamic_cast<Operation::RootOperation>(obj);
    m_opDeque.push_back(op);
}

void Connection::dispatchOp(const Atlas::Objects::Operation::RootOperation& op)
{
    RouterResult rr;
    bool anonymous = op->getTo().empty();
    
// give the type service a go   
    /// @todo - wrap this in an anonymous=true guard?
    rr = m_typeService->handleOperation(op);
    if (rr == Router::HANDLED)
        return;
    
// locate a router based on from
    IdRouterMap::const_iterator R = m_fromRouters.find(op->getFrom());
    if (R != m_fromRouters.end())
    {
        rr = R->second->handleOperation(op);
        if (rr == Router::HANDLED)
            return;
    }
    
// locate a router based on the op's TO value
    R = m_toRouters.find(op->getTo());
    if (R != m_toRouters.end())
    {
        rr = R->second->handleOperation(op);
        if (rr == Router::HANDLED)
            return;
        else
            debug() << "TO router " << R->first << "didn't handle op";
    } else if (!anonymous)
        warning() << "recived op with TO=" << op->getTo() << ", but no router is registered for that id";
            
// go to the default router
    rr = m_defaultRoute->handleOperation(op);
    if (rr != Router::HANDLED)
        warning() << "no-one handled op";
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
    m_typeService->init();
}

void Connection::postForDispatch(const Root& obj)
{
    Operation::RootOperation op = smart_dynamic_cast<Operation::RootOperation>(obj);
    m_opDeque.push_back(op);
}

} // of namespace
