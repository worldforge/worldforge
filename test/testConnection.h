#ifndef TEST_CONNECTION_H
#define TEST_CONNECTION_H


#include <sigc++/object.h>

#include <Atlas/Objects/Operation/Info.h>

class StubServer;
#include <Eris/Connection.h>

class TestConnection : 
    public SigC::Object
{
	/*
   ERIS_TEST_SUITE(TestConnection);
    ERIS_TEST(testConnect);
    CPPUNIT_TEST(testReconnect);
    CPPUNIT_TEST(testTimeout);
    CPPUNIT_TEST(testDisconnect);
    CPPUNIT_TEST(testDispatch);
    CPPUNIT_TEST(testSend);
    CPPUNIT_TEST_SUITE_END();
    */
public:
    TestConnection();

    
    void testConnect();
    void testReconnect();
    void testTimeout();

    void testDisconnect();
    void testComplexDisconnect();

    // Atlas transmission tests
    void testDispatch();
    void testSend();

	void setUp();
	void tearDown();
protected:
    StubServer* m_server;
    Eris::Connection* m_con;

    void waitFor(int timeoutSeconds, bool &watch1, bool &watch2);

// callback traps
    void onConnect();
    void onFailure(const std::string &msg);
    void onTimeout(Eris::BaseConnection::Status);

    bool onDisconnecting();
    bool onDisconnecting_delay();
    void onDisconnect();

    void onAnyDispatch(const Atlas::Objects::Operation::Info&);

    bool m_gotFailure, 
		m_gotConnect,
		m_gotTimeout,
		m_gotDisconnecting,
		m_gotDisconnect,
		m_gotArbitraryDispatch;
};

#endif
