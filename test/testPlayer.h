#ifndef TEST_PLAYER_H
#define TEST_PLAYER_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

class StubConnection;
#include "Player.h"

class TestPlayer : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(TestPlayer);
    CPPUNIT_TEST(testLogin);
    CPPUNIT_TEST_SUITE_END();
    
public:
    TestPlayer();

    void testLogin();

    virtual void setUp();
    virtual void tearDown();
protected:

    StubConnection* m_stub;
    Eris::Player* m_player;
};

#endif
