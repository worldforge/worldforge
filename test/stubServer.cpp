#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "stubServer.h"
#include "testHarness.h"

#include <Eris/Timestamp.h>
#include <Eris/PollDefault.h>

#include <Atlas/Objects/Encoder.h>

#include "Eris/atlas_utils.h"

#include <Atlas/Objects/Operation/RootOperation.h>
#include <Atlas/Objects/Operation/Error.h>
#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Get.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Appearance.h>

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Entity/RootEntity.h>
#include <Atlas/Objects/Entity/GameEntity.h>

using namespace Time;

using namespace Atlas::Objects;
using namespace Atlas::Message;

//////////////////////////////////////////////////////////////////////////////////////////////

StubServer::StubServer(short port) :
    m_stream(NULL),
    m_msgEncoder(NULL),
    m_acceptor(NULL),
    m_doNegotiate(true),
	m_handleTypeQueries(true)
{
     // create socket
    m_listenSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    ERIS_ASSERT_MESSAGE(m_listenSocket != INVALID_SOCKET, "failed to create listen socket");

    const int optvalue = 1;
    setsockopt(m_listenSocket, SOL_SOCKET, 
	  SO_REUSEADDR, &optvalue, sizeof(int));
  
    // Bind Socket
    sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY; // we want to connect to ANY client!
    sa.sin_port = htons((unsigned short)port); // define service port
    if(::bind(m_listenSocket, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR) {
		ERIS_MESSAGE("bind() failed in StubServer");
		close(m_listenSocket);
		return;
    }

    // Listen
    if(::listen(m_listenSocket, 5) == SOCKET_ERROR) { // max backlog
		ERIS_MESSAGE("listen() failed in StubServer");
		close(m_listenSocket);
		return;
    }
    
    m_state = LISTEN;
	
}

StubServer::~StubServer()
{
    close(m_listenSocket);
    
//    if (m_stream)
//	delete m_stream;
    
    if (m_acceptor)
	delete m_acceptor;
    
    if (m_msgEncoder)
	delete m_msgEncoder;
}

void StubServer::run()
{
    if ((m_state == LISTEN) && can_accept()) {
		accept();
		return;
    }
    
    if (m_state == NEGOTIATE) {
		negotiate();
		return;
    }
    
    if (!isStreamPending(m_stream)) 
		return; // if there's no data on the Atlas connection, we're done
    
    m_codec->Poll();	// read any data in
}

bool StubServer::can_accept()
{
    fd_set sock_fds;
    struct timeval tv = {0,0};
    FD_ZERO(&sock_fds);
    FD_SET(m_listenSocket, &sock_fds);

    int ret = ::select((m_listenSocket + 1), &sock_fds, NULL, NULL, &tv);
	if (ret < 0) {
		ERIS_MESSAGE("select() failed in StubServer");
		throw std::runtime_error("select() failed in StubServer");
    }
	
    return (ret > 0);
}

void StubServer::accept()
{
    SOCKET_TYPE socket = ::accept(m_listenSocket, NULL, NULL);
    if (socket == INVALID_SOCKET)
		throw std::runtime_error("error accepting connection in stub server");
    
    m_stream = new basic_socket_stream(socket);
    ERIS_ASSERT(m_stream->is_open());
    
    m_acceptor = new Atlas::Net::StreamAccept("Eris Stub Server", *m_stream, this);
    m_state = NEGOTIATE;
}

void StubServer::negotiate()
{
    if (!m_doNegotiate) return;	// stall here forever
    
    m_acceptor->Poll(false); // don't block
    
    // get any more
    while (isStreamPending(m_stream))
		m_acceptor->Poll();
    
    switch (m_acceptor->GetState()) {
    case Atlas::Net::StreamAccept::IN_PROGRESS:
		break;

    case Atlas::Net::StreamAccept::FAILED:
		fail();
		break;

    case Atlas::Net::StreamAccept::SUCCEEDED:
		m_codec = m_acceptor->GetCodec();
	
		m_msgEncoder = new Atlas::Message::Encoder(m_codec);
		m_objectEncoder = new Atlas::Objects::Encoder(m_codec);
	
		m_codec->StreamBegin();
		m_state = CONNECTED;
			
		delete m_acceptor;
		m_acceptor = NULL;
		break;

    default:
		ERIS_MESSAGE("unknown state from Atlas StreamAccept in StubServer::negotiate");
    }             
}

void StubServer::disconnect()
{
    ERIS_ASSERT(m_state == CONNECTED);
    
    m_stream->close();	// all go bye-bye!
    delete m_stream;
    m_stream = NULL;
    
    m_state = LISTEN;
}

bool StubServer::isStreamPending(basic_socket_stream *stream)
{
    if (stream->rdbuf()->in_avail() > 0)
		return true;
    
    SOCKET_TYPE fd(stream->getSocket());
    fd_set fds;
    FD_ZERO(&fds);
    
    FD_SET(fd, &fds);
    struct timeval TIMEOUT_ZERO = {0, 0};
    
    int retval = select(fd + 1,&fds, NULL, NULL, &TIMEOUT_ZERO);
    if (retval < 0)
		ERIS_MESSAGE("stub-server select() failed");
    
    return FD_ISSET(fd, &fds);
}

void StubServer::push(const Atlas::Message::Object &obj)
{
    m_msgEncoder->StreamMessage(obj);
    (*m_stream) << std::flush;
}

void StubServer::push(const Atlas::Objects::Root &obj)
{
    m_objectEncoder->StreamMessage(&obj);
    (*m_stream) << std::flush;
}

void StubServer::ObjectArrived(const Atlas::Message::Object& obj)
{
	if (m_handleTypeQueries) {
		Operation::RootOperation op(Atlas::atlas_cast<Operation::RootOperation>(obj));
		
		if ((op.GetParents() == Object::ListType(1, "get")) &&
			(op.GetFrom() == "") && 
			(op.GetTo() == "")) 
		{
			/* okay, we have an anonymous GET, now we just need to discard server object
			lookups, be ensuring we have an argument value. */
			
			Object::ListType &args(op.GetArgs());
			if ((args.size() == 1) && (args[0].IsString())) {
				/* yay, we're good to go */
				sendInfoForType(args[0].AsString(), op);
				return;
			}
		}
	}
	
    m_queue.push(obj);
}

bool StubServer::get(Atlas::Message::Object &obj)
{
    if (m_queue.empty()) return false;
    obj = m_queue.front();
    m_queue.pop();
    return true;
}

void StubServer::fail()
{
    m_state = FAILURE;
    ERIS_MESSAGE("stub-server got failure");
    // what else?
}

void StubServer::waitForMessage(int timeout)
{
    Time::Stamp ts(Time::Stamp::now());
    ts = ts + (timeout * 1000);
    
    while (ts > Time::Stamp::now()) {
		run();
		Eris::PollDefault::poll();
		usleep(10000); // 10 msec = 1/100 of a second
		
		if (!m_queue.empty()) return;	// all done
    }
    
    ERIS_MESSAGE("timed out in stub-server::waitForMessage");
}

#pragma mark -

void StubServer::sendInfoForType(const std::string &type, const Operation::RootOperation &get)
{
	Operation::Info info(Operation::Info::Instantiate());
	Object::ListType &args(info.GetArgs());
	
	if (type == "root")
		args.push_back(Atlas::Objects::Root().AsObject());
	else if (type == "root_entity") {
		args.push_back(Entity::RootEntity().AsObject());
	} else if (type == "game_entity") {
		args.push_back(Entity::GameEntity().AsObject());
	} else if (type == "root_operation") {
		args.push_back(Operation::RootOperation().AsObject());
	} else if (type == "info") {
		args.push_back(Operation::Info().AsObject());
	} else if (type == "get") {
		args.push_back(Operation::Get().AsObject());
	} else if (type == "set") {
		args.push_back(Operation::Set().AsObject());
	} else if (type == "error") {
		args.push_back(Operation::Error().AsObject());
	} else if (type == "create") {
		args.push_back(Operation::Create().AsObject());
	} else if (type == "move") {
		args.push_back(Operation::Move().AsObject());
	} else if (type == "appearance") {
		args.push_back(Operation::Appearance().AsObject());
	} else {
		ERIS_MESSAGE("unknown type in sendInfoForType, responing with ERROR instead");
	
		Operation::Error error(Operation::Error::Instantiate());
	
		Object::ListType& eargs(error.GetArgs());
		eargs.push_back("undefined type " + type);
		eargs.push_back(get.AsObject());
	
		error.SetRefno(get.GetSerialno());
	
		push(error);
	}
}
