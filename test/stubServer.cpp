#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <unistd.h>
#include <sys/socket.h>

#include "stubServer.h"
#include "Time.h"
#include "PollDefault.h"

#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace Time;

//////////////////////////////////////////////////////////////////////////////////////////////

StubServer::StubServer(short port) :
    m_stream(NULL),
    m_msgEncoder(NULL),
    m_acceptor(NULL),
    m_doNegotiate(true)
{
    m_listenSocket = new tcp_socket_server(port);
    CPPUNIT_ASSERT_MESSAGE("failed to open server socket", m_listenSocket);

    if (!m_listenSocket->is_open())
	throw std::invalid_argument("unable to open server socket in stub server");
    
    const int optvalue = 1;
    setsockopt(m_listenSocket->getSocket(), SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(int));
    m_state = LISTEN;
}

StubServer::~StubServer()
{
    delete m_listenSocket;
    
    if (m_stream)
	delete m_stream;
    
    if (m_acceptor)
	delete m_acceptor;
    
    if (m_msgEncoder)
	delete m_msgEncoder;
}

void StubServer::run()
{
    if ((m_state == LISTEN) && m_listenSocket->can_accept()) {
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

void StubServer::accept()
{
    SOCKET_TYPE socket(m_listenSocket->accept());
    if (socket == INVALID_SOCKET)
	throw std::runtime_error("error accepting connection in stub server");
    
    m_stream = new basic_socket_stream(socket);
    CPPUNIT_ASSERT(m_stream->is_open());
    
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

	m_codec->StreamBegin();
	m_state = CONNECTED;
	    
	delete m_acceptor;
	m_acceptor = NULL;
	break;

    default:
	CPPUNIT_ASSERT_MESSAGE("unknown state from Atlas StreamAccept in StubServer::negotiate", false);
    }             
}

void StubServer::disconnect()
{
    CPPUNIT_ASSERT_MESSAGE("stub-server not in connected state", m_state == CONNECTED);
    
    m_stream->close();	// all go bye-bye!
    delete m_stream;
    
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
	CPPUNIT_ASSERT_MESSAGE("stub-server select() failed", false);
    
    return FD_ISSET(fd, &fds);
}

void StubServer::push(const Atlas::Message::Object &obj)
{
    m_msgEncoder->StreamMessage(obj);
    (*m_stream) << std::flush;
}

void StubServer::ObjectArrived(const Atlas::Message::Object& obj)
{
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
    CPPUNIT_ASSERT_MESSAGE("stub-server got failure", false);
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
    
    CPPUNIT_ASSERT_MESSAGE("timed out in stub-server::waitForMessage", false);
}