#ifndef TEST_PLAYER_H
#define TEST_PLAYER_H

#include "stubServer.h"
#include <Atlas/Objects/Entity/GameEntity.h>
#include <sigc++/object.h>

namespace Eris
{
	class Player;
	class Connection;
}

class TestPlayer : public SigC::Object
{
/*	
    CPPUNIT_TEST_SUITE(TestPlayer);
    CPPUNIT_TEST(testLogin);
    CPPUNIT_TEST_SUITE_END();
    */
public:
    TestPlayer();

    void testLogin();
	void testAccountCreate();

	void testClientLogout();
	void testCharacterLook();

    virtual void setUp();
    virtual void tearDown();
protected:

    StubServer* m_server;
    Eris::Player* m_player;
	Eris::Connection* m_connection;

	void doStandardLogin();

// callbacks
	void gotCharacterInfo(const Atlas::Objects::Entity::GameEntity &character);

	void onLoginComplete()
	{
		m_gotLoginComplete = true;
	}
	
	void onLogout(bool clean)
	{
		if (clean) m_logoutValue = 1;
		else m_logoutValue = 0;
	}

	void onGotAllChars()
	{
		m_gotAllChars = true;
	}
	
	bool m_gotLoginComplete;
	int m_logoutValue;
	bool m_gotAllChars;

};

#endif
