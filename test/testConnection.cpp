#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "testConnection.h"
#include "stubServer.h"
#include "testHarness.h"

#include <unistd.h>	// for usleep()
#include <Atlas/Message/Object.h>

#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Entity/Account.h>

#include <sigc++/bind.h>
#include <sigc++/signal.h>
#include <sigc++/object_slot.h>

#include "testUtils.h"
#include <Eris/Utils.h>
#include <Eris/Timestamp.h>
#include <Eris/PollDefault.h>
#include <Eris/SignalDispatcher.h>

using namespace Atlas::Message;
using namespace Atlas::Objects;

using namespace Time;

const short TEST_SERVER_PORT = 21588;

TestConnection::TestConnection()
{
    ;
}

void TestConnection::setUp()
{
    //Time::Init();
    m_con = new Eris::Connection("test_connection", false);
    
    m_server = new StubServer(TEST_SERVER_PORT);
    
    // attach to all the signals globaly for monitoring later
    m_con->Timeout.connect(SigC::slot(*this, &TestConnection::onTimeout));
    m_con->Failure.connect(SigC::slot(*this, &TestConnection::onFailure));
    m_con->Connected.connect(SigC::slot(*this, &TestConnection::onConnect));
    m_con->Disconnected.connect(SigC::slot(*this, &TestConnection::onDisconnect));
	
    m_gotFailure = m_gotConnect = m_gotTimeout = false;
	m_gotDisconnect = false;
}

void TestConnection::tearDown()
{
    delete m_con;
    delete m_server;
}

void TestConnection::waitFor(int timeoutSeconds, bool &watch1, bool &watch2)
{
    Time::Stamp ts(Time::Stamp::now());
    ts = ts + (timeoutSeconds * 1000);
    
    while (!watch1 && !watch2) {
		if (ts < Time::Stamp::now()) {
			ERIS_MESSAGE("timed out in waitFor");
			return;
		}
		m_server->run();
		Eris::PollDefault::poll();
		usleep(10000); // 10 msec = 1/100 of a second
    }
}

void TestConnection::testConnect()
{    
    m_con->connect("127.0.0.1", TEST_SERVER_PORT);
    ERIS_ASSERT(!m_gotFailure);
    
    waitFor(10, m_gotConnect, m_gotFailure);
    
    ERIS_ASSERT(!m_gotFailure);
    ERIS_ASSERT(m_gotConnect);
    ERIS_ASSERT(m_con->isConnected());
}

void TestConnection::testDisconnect()
{
    testConnect();
    
    SigC::Connection ref = m_con->Disconnecting.connect(
		SigC::slot(*this, &TestConnection::onDisconnecting));
    m_gotDisconnecting = false;
    
    m_con->disconnect();
    
    waitFor(30, m_gotDisconnect, m_gotFailure);
    
    ERIS_ASSERT(!m_gotFailure);
    ERIS_ASSERT(m_gotDisconnect);
    ERIS_ASSERT(m_gotDisconnecting);
    
    ref.disconnect();
}

void TestConnection::testComplexDisconnect()
{
    // same as above, but set a dummy disconnecting op (and the time it out)
    
    
}

bool TestConnection::onDisconnecting()
{
    ERIS_ASSERT(m_con->getStatus() == Eris::BaseConnection::DISCONNECTING);
    m_gotDisconnecting = true;
    return false;
}

bool TestConnection::onDisconnecting_delay()
{
    m_con->lock();
    
    Atlas::Message::Object::MapType mt;
    mt["parents"] = Atlas::Message::Object::ListType(1, "delay");
    mt["objtype"] = "op";
    
    Atlas::Message::Object::MapType args;
    args["time"] = 1.0;	 // wait a second
    mt["args"] = Atlas::Message::Object::ListType(1, args);
    
    m_con->send(mt);
    return true;
}

void TestConnection::testReconnect()
{
    testConnect();
    
    // tell the server to pull the plug (mimic network loss / crash)
    m_server->disconnect();
	
	ERIS_ASSERT(m_gotDisconnect);
try {
    
    m_con->reconnect();
    waitFor(30, m_gotConnect, m_gotFailure);
    
    ERIS_ASSERT(m_gotConnect);
    ERIS_ASSERT(!m_gotFailure);
}
    catch (...) {
		ERIS_MESSAGE("got exception testing Connection::reconnect()");
    }
}

void TestConnection::testTimeout()
{
    m_server->setNegotiation(false);	// tell it not to negotiate ever
    
    m_con->connect("127.0.0.1", TEST_SERVER_PORT);
    
    waitFor(60, m_gotFailure, m_gotTimeout);
    ERIS_ASSERT(m_gotTimeout);
}

void TestConnection::testDispatch()
{
    testConnect();
    
    // add a sample dispatcher (assumes dispatcher works of course)
    Eris::Dispatcher *rd = m_con->getDispatcher();
    ERIS_ASSERT(rd);
    
    Eris::Dispatcher *sigD = rd->addSubdispatch(new Eris::SignalDispatcher<Operation::Info>(
	"dummy", 
	SigC::slot(*this, &TestConnection::onAnyDispatch)
    ));
    
    Eris::Dispatcher *sigD2 = m_con->getDispatcherByPath("dummy");
    ERIS_ASSERT(sigD == sigD2);
    
    m_gotArbitraryDispatch = false;
    
    Atlas::Message::Object::MapType testOp;
    testOp["parents"] = Atlas::Message::Object::ListType(1, "info");
    testOp["from"] = "stub-server";
    testOp["to"] = "test-client";
    m_server->push(testOp);
    
    waitFor(10, m_gotFailure, m_gotArbitraryDispatch);
    
    ERIS_ASSERT(!m_gotFailure);
    ERIS_ASSERT_MESSAGE(m_gotArbitraryDispatch, "dispatch failed");}

void TestConnection::onAnyDispatch(const Operation::Info &)
{
    m_gotArbitraryDispatch = true;
}

void TestConnection::testSend()
{
    testConnect();
    
    Operation::Get get(Operation::Get::Instantiate());
    
    m_con->send(get);
    m_server->waitForMessage(10);
    
    Atlas::Message::Object received;
    ERIS_ASSERT(m_server->get(received));
    
    ERIS_ASSERT(getType(received) == "get");
}

void TestConnection::onConnect()
{
    m_gotConnect=true;
}

void TestConnection::onFailure(const std::string &)
{
    m_gotFailure=true;
}

void TestConnection::onTimeout(Eris::BaseConnection::Status)
{
    m_gotTimeout=true;
}

void TestConnection::onDisconnect()
{
    m_gotDisconnect = true;
}
