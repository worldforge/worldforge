#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <assert.h>
#include <cstdio>
#include <skstream/skstream.h>
#include <sigc++/signal_system.h>

#include "Metaserver.h"
#include "MetaQuery.h"
#include "Utils.h"
#include "ServerInfo.h"
#include "Timeout.h"
#include "Poll.h"
#include "PollDefault.h"
#include "Connection.h"
#include "Log.h"

namespace Eris {

char* pack_uint32(uint32_t data, char* buffer, unsigned int &size);
char* unpack_uint32(uint32_t &dest, char* buffer);

const int META_SERVER_PORT = 8453;

// meta-server protocol commands	
const uint32_t CKEEP_ALIVE = 2,
	HANDSHAKE = 3,
	CLIENTSHAKE = 5,
	LIST_REQ = 7,
	LIST_RESP = 8,
	PROTO_ERANGE = 9;

// special  command value to track LIST_RESP processing
const uint32_t LIST_RESP2 = 999;		
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Meta::Meta(const std::string &/*cnm*/, 
	const std::string &msv, 
	unsigned int maxQueries) :
	_status(INVALID),
	_metaHost(msv),
	_maxActiveQueries(maxQueries),
	_timeout(NULL)
{
    _stream = new udp_socket_stream();  
    _stream->setTimeout(30);	
    
    Poll::instance().connect(SigC::slot(*this, &Meta::gotData));
    Poll::instance().addStream(_stream);
    
    // open connection to meta server
   
    remote_host metaserver_data(msv, META_SERVER_PORT);
    (*_stream) << metaserver_data;
    if (!_stream->is_open()) {
	doFailure("Couldn't open connection to metaserver " + msv);
	return;
    }
    
    // build the initial 'ping' and send
    unsigned int dsz = 0;
    pack_uint32(CKEEP_ALIVE, _data, dsz);
    (*_stream) << std::string(_data, dsz) << std::flush;
    setupRecvCmd();
    _status = IN_PROGRESS;
    
    // check for meta-server timeouts; this is going to be
    // fairly common as long as the protocol is UDP, I think
    _timeout = new Timeout("meta_ckeepalive_"+msv, 8000);
    _timeout->Expired.connect(SigC::slot(*this, &Meta::metaTimeout));
}

Meta::~Meta()
{
    if(_stream)
      {
	Poll::instance().removeStream(_stream);
	delete _stream;
      }
    
    for (MetaQueryList::iterator Q=_activeQueries.begin(); Q!=_activeQueries.end();++Q)
	delete *Q;
	
    if (_timeout)
      delete _timeout;
}

void Meta::queryServer(const std::string &ip)
{
    if (_status != IN_PROGRESS)
	    _status = IN_PROGRESS;
    
    if (_activeQueries.size() >= _maxActiveQueries) {
	    _pendingQueries.push_back(ip);
    } else {
	    MetaQuery *q =  new MetaQuery(this, ip);
	    if (q->isComplete()) {
		// indicated a failure occurred, so we'll kill it now and say no more
		delete q;
	    } else
		_activeQueries.push_back(q);
    }
}

void Meta::refresh()
{
    if (_status == VALID) {
	// FIXME - save the current list
    }

    _gameServers.clear();
	
    // close the existing connection (rather brutal this...)
    if (_stream) {
	Poll::instance().removeStream(_stream);
	_stream->close();
    } else {
	_stream = new udp_socket_stream;
	Poll::instance().addStream(_stream);
    }
    
    // open connection to meta server
    remote_host metaserver_data(_metaHost, META_SERVER_PORT);
    (*_stream) << metaserver_data;
    if (!_stream->is_open()) {
	doFailure("Couldn't contact metaserver " + _metaHost);
	return;
    }
    
    listReq(0);
    _status = IN_PROGRESS;
}

ServerList Meta::getGameServerList()
{
	ServerList ret;
	// bail out quickly if INVALID
	if (_status == INVALID) return ret;
		
	for (ServerInfoMap::iterator I=_gameServers.begin();  I!=_gameServers.end();++I)
		ret.push_back(I->second);
	return ret;
}
		
int Meta::getGameServerCount()
{
    if (_status == INVALID) return 0;
    return _gameServers.size();
}

void Meta::gotData(PollData &data)
{
    bool got_one = false;
    
    if (_stream) {
	if (!_stream->is_open()) {
	    // it died, delete it
	    doFailure("Connection to the meta-server failed");
	} else
	    if (data.isReady(_stream)) {
		recv();
		got_one = true;
	    }
    } // of _stream being valid

    for (MetaQueryList::iterator Q=_activeQueries.begin();
	    Q != _activeQueries.end(); ++Q)
    {
	if (!(*Q)->_stream->is_open()) {
	    queryFailure(*Q, "Query connection closed unexpectedly!");
	    continue;
	}
	
	if ((*Q)->isReady(data)) {
	    (*Q)->recv();	
	    got_one = true;
	}
    }

    if(!got_one)
	    return;

	// clean up old queries
	while (!_deleteQueries.empty()) {
	    MetaQuery *qr = _deleteQueries.front();
	    _activeQueries.remove(qr);
	    delete qr;
	    _deleteQueries.pop_front();
	}
	
	// start pending queries as slots become available
	while (!_pendingQueries.empty() && (_activeQueries.size() < _maxActiveQueries)) {
		queryServer(_pendingQueries.front());
		_pendingQueries.pop_front();
	}
	
	if ((_status == VALID) && (_activeQueries.empty())) {
	    // we're all done, emit the signal
	    CompletedServerList.emit();
	}
}

void Meta::recv()
{
	assert(_bytesToRecv);
	Eris::log(LOG_DEBUG, "got data from the meta-server");
	
	do {
		int d = _stream->get();
		*(_dataPtr++) = static_cast<char>(d);
		_bytesToRecv--;
	} while (_stream->rdbuf()->in_avail() && _bytesToRecv);
	
	if (_bytesToRecv == 0) {
		// recieved the next set
		if (_recvCmd) {
			uint32_t op;
			unpack_uint32(op, _data);
			recvCmd(op);
		} else {
			processCmd();
		}
		
		// try and read more
		if (_bytesToRecv && _stream->rdbuf()->in_avail())
		    recv();
	}
}

void Meta::cancel()
{
	_pendingQueries.clear();
	for (MetaQueryList::iterator Q=_activeQueries.begin(); Q!=_activeQueries.end();++Q)
		delete *Q;
	_activeQueries.clear();
	
	if(_stream) {
	    Poll::instance().removeStream(_stream);
	    delete _stream;
	    _stream = NULL;
	}
	
	// FIXME - revert to the last valid server list?
	_status = INVALID;
}

void Meta::recvCmd(uint32_t op)
{
    Eris::log(LOG_DEBUG, "recvd meta-server CMD %i", op);
    
	switch (op) {
	case HANDSHAKE:
		setupRecvData(1, HANDSHAKE);
		break;
	
	case PROTO_ERANGE:
		doFailure("Got list range error from Metaserver");
		break;
	
	case LIST_RESP:
		setupRecvData(2, LIST_RESP);
		break;
	
	default:
		doFailure("Unknown Meta server command");
	}
}

void Meta::processCmd()
{
	switch (_gotCmd) {
	case HANDSHAKE: {	
		uint32_t stamp;
		unpack_uint32(stamp, _data);
			
		unsigned int dsz = 0;
		_dataPtr = pack_uint32(CLIENTSHAKE, _data, dsz);
		pack_uint32(stamp, _dataPtr, dsz);
		
		(*_stream) << std::string(_data, dsz) << std::flush;
		
		// clear the handshake timeout, so listReq can start it's own.
		delete _timeout;
		_timeout = NULL;
		
	    Eris::log(LOG_DEBUG, "processed HANDSHAKE, sending list request");
		// send the initial list request
		listReq(0);
		} break;
	
	case LIST_RESP:	{
		//uint32_t _totalServers, _packed;
		_dataPtr = unpack_uint32(_totalServers, _data);
		unpack_uint32(_packed, _dataPtr);
		setupRecvData(_packed, LIST_RESP2);
		
	    Eris::log(LOG_DEBUG, "processed LIST_RESP");
		// allow progress bars to setup, etc, etc
		GotServerCount.emit(_totalServers);
		
		} break;
	
	case LIST_RESP2: {
		_dataPtr = _data;
		while (_packed--) {
			uint32_t ip;
			_dataPtr = unpack_uint32(ip, _dataPtr);
			
			char buf[32];
			snprintf(buf, 32, "%i.%i.%i.%i", 
				(ip & 0x000000FF),
				(ip & 0x0000FF00) >> 8,
				(ip & 0x00FF0000) >> 16,
				(ip & 0xFF000000) >> 24
			);
			
			// FIXME  - decide whther a reverse name lookup is necessary here or not
	
			// create as required
			if (_gameServers.find(buf) == _gameServers.end())
				_gameServers.insert(
					ServerInfoMap::value_type(buf, ServerInfo(buf))
				);
			
			Eris::log(LOG_DEBUG, "queueing game server %s for query", buf);
			// is always querying a good idea?
			queryServer(buf);
		}
			
		if (_gameServers.size() < _totalServers) {
			// request some more
			Eris::log(LOG_DEBUG, "in LIST_RESP2, issuing request for next block");
			listReq(_gameServers.size());
		} else {
		    // all done, clean everything up
		    delete _timeout;
		    _timeout = NULL;
		    _status = VALID;
	
		    if(_stream) {	    
			Eris::log(LOG_DEBUG, "deleting meta-server stream");
			Poll::instance().removeStream(_stream);
			delete _stream;
			_stream = NULL;
		    }
		}
		
		} break;
		
	default:
		doFailure("Unknown Meta server command");
	}
}

void Meta::listReq(int base)
{
	unsigned int dsz=0;
	char* _dataPtr = pack_uint32(LIST_REQ, _data, dsz);
	pack_uint32(base, _dataPtr, dsz);
	
	(*_stream) << std::string(_data, dsz) << std::flush;
	setupRecvCmd();
	
	if (_timeout)
		_timeout->reset(5000);
	else {
		_timeout = new Timeout("meta_list_req", 8000);
		_timeout->Expired.connect(SigC::slot(*this, &Meta::metaTimeout));
	}
}

void Meta::ObjectArrived(const Atlas::Message::Object &msg)
{	
	// build an info op (quick hack)
	Atlas::Objects::Operation::Info ifo = 
		Atlas::Objects::Operation::Info::Instantiate();
	
	Atlas::Message::Object::MapType::const_iterator I = msg.AsMap().begin();
	for (; I != msg.AsMap().end(); ++I)
        	ifo.SetAttr(I->first, I->second);
	
	// work out which query this is
	long refno = ifo.GetRefno();
	MetaQueryList::iterator Q;
	
	for (Q = _activeQueries.begin(); Q != _activeQueries.end(); ++Q)
		if ((*Q)->getQueryNo() == refno) break;
	
	if (Q == _activeQueries.end()) {
	    // handle old behaviour (no serial / refno set in reply); this only works
	    // with single query mode (i.e Max-Queries = 1)
	    
	    if (_activeQueries.size() == 1)
		    Q = _activeQueries.begin();
	    else {
		Eris::log(LOG_ERROR, "Couldn't locate query for meta-query reply");
		return;
	    }
	}
	
	// extract the server object
	Atlas::Objects::Entity::RootEntity svr =
		Atlas::Objects::Entity::RootEntity::Instantiate();
	
	Atlas::Message::Object::MapType m = getArg(ifo, 0).AsMap();
	for (Atlas::Message::Object::MapType::iterator I=m.begin() ; I != m.end(); ++I)
        	svr.SetAttr(I->first, I->second);
	
	// update the server-info structure
	ServerInfoMap::iterator S = _gameServers.find((*Q)->getHost());
	assert(S != _gameServers.end());
	
	S->second.processServer(svr);
	S->second.setPing((*Q)->getElapsed());
	
	// emit the signal
	ReceivedServerInfo.emit(S->second);
	
	_deleteQueries.push_back(*Q);
}

void Meta::doFailure(const std::string &msg)
{
    Failure.emit(msg);
    if(_stream) {
	Poll::instance().removeStream(_stream);
	delete _stream;
	_stream = NULL;
    }
    
    // try to revert back to the last good list
    if (!_lastValidList.empty()) {
	    _gameServers = _lastValidList;
	    _status = VALID;
    } else
	    _status = INVALID;
    
    if (_timeout) {
	    delete _timeout;
	    _timeout = NULL;
    }
		
}

void Meta::metaTimeout()
{
	// might want different behaviour in the future, I suppose
	doFailure("Connection to the meta-server timed out");
}

void Meta::queryFailure(MetaQuery *q, const std::string &msg)
{
	// we do NOT emit a failure signal here (becuase that would probably cause the 
	// host app to pop up a dialog or something) since query failures are likely to
	// be very frequent.
         Eris::log(LOG_DEBUG, "Query Failure: %s", msg.c_str());

	_deleteQueries.push_back(q);
}

void Meta::queryTimeout(MetaQuery *q)
{
        Eris::log(LOG_DEBUG, "Query Timeout");
	_deleteQueries.push_back(q);
}

void Meta::setupRecvCmd()
{
	_recvCmd = true;
	_bytesToRecv = sizeof(uint32_t);
	_dataPtr = (char*) _data;
}

void Meta::setupRecvData(int words, uint32_t got)
{
	_recvCmd = false;
	_bytesToRecv = words * sizeof(uint32_t);
	_dataPtr = (char*) _data;
	_gotCmd = got;
}

/* pack the data into the specified buffer, update the buffer size, and return
the new buffer insert pointer */

char* pack_uint32(uint32_t data, char* buffer, unsigned int &size)
{
	uint32_t netorder;
 
	netorder = htonl(data);
	memcpy(buffer, &netorder, sizeof(uint32_t));
	size += sizeof(uint32_t);
 	return buffer+sizeof(uint32_t);
}

/* unpack one data from the buffer, and return the next extract pointer */

char* unpack_uint32(uint32_t &dest, char* buffer)
{
	uint32_t netorder;
 
	memcpy(&netorder, buffer, sizeof(uint32_t));
	dest = ntohl(netorder);
	return buffer+sizeof(uint32_t);
} 

} // of Eris namespace

