#ifndef TEST_LOBBY_H
#define TEST_LOBBY_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "Lobby.h"
#include "stubConnection.h"

class TestLobby : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(TestLobby);
    CPPUNIT_TEST(testEntry);
    CPPUNIT_TEST(testRoomCreate);
    CPPUNIT_TEST_SUITE_END();
    
public:
    TestLobby();

    void testEntry();
    void testRoomCreate();
    void testPrivateChat();

    virtual void setUp();
    virtual void tearDown();
protected:

    Eris::Lobby *test_lobby;
    StubConnection *con;
};

#endif
