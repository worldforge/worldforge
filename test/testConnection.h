#ifndef TEST_CONNECTION_H
#define TEST_CONNECTION_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include <sigc++/object.h>

#include <Atlas/Objects/Operation/Info.h>

class StubServer;
#include "Connection.h"

class TestConnection : 
    public CppUnit::TestCase,
    public SigC::Object
{
    CPPUNIT_TEST_SUITE(TestConnection);
    CPPUNIT_TEST(testConnect);
    CPPUNIT_TEST(testReconnect);
    CPPUNIT_TEST(testTimeout);
    CPPUNIT_TEST(testDisconnect);
    CPPUNIT_TEST(testDispatch);
    CPPUNIT_TEST(testSend);
    CPPUNIT_TEST_SUITE_END();
    
public:
    TestConnection();

    // setup / tear-down tests
    void testConnect();
    void testReconnect();
    void testTimeout();

    void testDisconnect();
    void testComplexDisconnect();

    // Atlas transmission tests
    void testDispatch();
    void testSend();

    virtual void setUp();
    virtual void tearDown();
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
