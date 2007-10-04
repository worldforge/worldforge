// Eris Online RPG Protocol Library
// Copyright (C) 2007 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#include <Eris/Account.h>

#include <Eris/Connection.h>
#include <Eris/Exceptions.h>
#include <Eris/Log.h>

#include "SignalFlagger.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Entity.h>

#include <sigc++/adaptors/hide.h>

class TestConnection : public Eris::Connection {
  public:
    TestConnection(const std::string & name, const std::string & host,
                   short port, bool debug) :
                   Eris::Connection(name, host, port, debug) { }

    void test_setStatus(Eris::BaseConnection::Status sc) {
        setStatus(sc);
    }
};

class TestAccount : public Eris::Account {
  public:
    TestAccount(Eris::Connection * con) : Eris::Account(con) { }

    void setup_setStatus(Eris::Account::Status s) {
        m_status = s;
    }

    void setup_setDoingCharacterRefresh(bool f) {
        m_doingCharacterRefresh = f;
    }

    void setup_fakeCharacter(const std::string & id) {
        m_characterIds.insert(id);
    }

    void test_logoutResponse(const Atlas::Objects::Operation::RootOperation& op) {
        logoutResponse(op);
    }

    void test_loginComplete(const Atlas::Objects::Entity::Account &p) {
        loginComplete(p);
    }

    void test_updateFromObject(const Atlas::Objects::Entity::Account &p) {
        updateFromObject(p);
    }

    void test_loginError(const Atlas::Objects::Operation::Error& err) {
        loginError(err);
    }

    static const Eris::Account::Status LOGGING_IN = Eris::Account::LOGGING_IN;
    static const Eris::Account::Status LOGGED_IN = Eris::Account::LOGGED_IN;
    static const Eris::Account::Status LOGGING_OUT = Eris::Account::LOGGING_OUT;
    static const Eris::Account::Status CREATING_CHAR = Eris::Account::CREATING_CHAR;
    static const Eris::Account::Status TAKING_CHAR = Eris::Account::TAKING_CHAR;
};

static void writeLog(Eris::LogLevel, const std::string & msg)
{
    std::cerr << msg << std::endl << std::flush;
}

int main()
{
    Eris::Logged.connect(sigc::ptr_fun(writeLog));

    // Test constructor
    {
        Eris::Connection * con = new Eris::Connection("name", "localhost",
                                                      6767, true);

        Eris::Account acc(con);
    }
    // Test constructor throws on a null connection.
    {
        try {
            Eris::Account acc(0);
            abort();
        }
        catch (Eris::InvalidOperation & eio) {
        }
    }
    // Test getActiveCharacters()
    {
        Eris::Connection * con = new Eris::Connection("name", "localhost",
                                                      6767, true);

        Eris::Account acc(con);

        const Eris::ActiveCharacterMap & acm = acc.getActiveCharacters();

        assert(acm.empty());
    }
    // Test getId()
    {
        Eris::Connection * con = new Eris::Connection("name", "localhost",
                                                      6767, true);

        Eris::Account acc(con);

        const std::string & id = acc.getId();

        assert(id.empty());
    }
    // Test getUsername()
    {
        Eris::Connection * con = new Eris::Connection("name", "localhost",
                                                      6767, true);

        Eris::Account acc(con);

        const std::string & username = acc.getUsername();

        assert(username.empty());
    }

    // Test getConnection()
    {
        Eris::Connection * con = new Eris::Connection("name", "localhost",
                                                      6767, true);

        Eris::Account acc(con);

        Eris::Connection * got_con = acc.getConnection();

        assert(got_con == con);
    }

    // Test login() fails if not connected
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        Eris::Account acc(con);

        Eris::Result res =  acc.login("foo", "bar");
        assert(res == Eris::NOT_CONNECTED);
    }

    // Test login() fails if we fake connected and logged in
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);

        con->test_setStatus(Eris::BaseConnection::CONNECTED);
        acc.setup_setStatus(TestAccount::LOGGING_IN);

        Eris::Result res =  acc.login("foo", "bar");
        assert(res == Eris::ALREADY_LOGGED_IN);
    }

    // Test createAccount() fails if not connected
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        Eris::Account acc(con);

        Eris::Result res =  acc.createAccount("foo", "bar", "baz");
        assert(res == Eris::NOT_CONNECTED);
    }

    // Test createAccount() fails if we fake connected and logged in
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);

        con->test_setStatus(Eris::BaseConnection::CONNECTED);
        acc.setup_setStatus(TestAccount::LOGGING_IN);

        Eris::Result res =  acc.createAccount("foo", "bar", "baz");
        assert(res == Eris::ALREADY_LOGGED_IN);
    }
    // FIXME Cover the rest of createAccount once we can fake connections.

    // Test logout() fails if not connected.
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);

        Eris::Result res =  acc.logout();
        assert(res == Eris::NOT_CONNECTED);
    }
    
    // Test logout() does nothing if we fake connected, and logging out.
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        con->test_setStatus(Eris::BaseConnection::CONNECTED);
        acc.setup_setStatus(TestAccount::LOGGING_OUT);

        Eris::Result res =  acc.logout();
        assert(res == Eris::NO_ERR);
    }
    
    // Test logout() fails if we are fake connected but not logged in.
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        con->test_setStatus(Eris::BaseConnection::CONNECTED);

        Eris::Result res =  acc.logout();
        assert(res == Eris::NOT_LOGGED_IN);
    }
    // FIXME Cover the rest of logout once we can fake connections.
    
    // Test getCharacters()
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);

        const Eris::CharacterMap & ecm = acc.getCharacters();

        assert(ecm.empty());
    }

    // Test refreshCharacterInfo() fails if not connected
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        Eris::Account acc(con);

        Eris::Result res =  acc.refreshCharacterInfo();
        assert(res == Eris::NOT_CONNECTED);
    }

    // Test refreshCharacterInfo() fails if we fake connected and logging in
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);

        con->test_setStatus(Eris::BaseConnection::CONNECTED);
        acc.setup_setStatus(TestAccount::LOGGING_IN);

        Eris::Result res =  acc.refreshCharacterInfo();
        assert(res == Eris::NOT_LOGGED_IN);
    }

    // Test refreshCharacterInfo() returns if we fake connected and logged in,
    // and pretend a query is already occuring.
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);

        con->test_setStatus(Eris::BaseConnection::CONNECTED);
        acc.setup_setStatus(TestAccount::LOGGED_IN);
        acc.setup_setDoingCharacterRefresh(true);

        Eris::Result res =  acc.refreshCharacterInfo();
        assert(res == Eris::NO_ERR);

        // Make sure it no longer thinks it is connected, so it does not try
        // to log out in the destructor.
        acc.setup_setStatus(TestAccount::LOGGING_OUT);
    }

    // Test refreshCharacterInfo() returns if we fake connected and logged in,
    // and have no characters IDs.
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        SignalFlagger gotAllCharacters_checker;

        con->test_setStatus(Eris::BaseConnection::CONNECTED);
        acc.setup_setStatus(TestAccount::LOGGED_IN);
        acc.GotAllCharacters.connect(sigc::mem_fun(gotAllCharacters_checker,
                                                   &SignalFlagger::set));

        assert(!gotAllCharacters_checker.flagged());
        Eris::Result res =  acc.refreshCharacterInfo();
        assert(res == Eris::NO_ERR);
        assert(gotAllCharacters_checker.flagged());

        // Make sure it no longer thinks it is connected, so it does not try
        // to log out in the destructor.
        acc.setup_setStatus(TestAccount::LOGGING_OUT);
    }
    // FIXME Cover the rest of refreshCharacterInfo once we can fake
    // connections.

    // Test createCharacter() fails if not connected
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        Eris::Account acc(con);
        Atlas::Objects::Entity::Anonymous ent;

        Eris::Result res =  acc.createCharacter(ent);
        assert(res == Eris::NOT_CONNECTED);
    }

    // Test createCharacter() fails if we fake connected and creating char
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        Atlas::Objects::Entity::Anonymous ent;

        con->test_setStatus(Eris::BaseConnection::CONNECTED);
        acc.setup_setStatus(TestAccount::CREATING_CHAR);

        Eris::Result res =  acc.createCharacter(ent);
        assert(res == Eris::DUPLICATE_CHAR_ACTIVE);

        // Make sure it no longer thinks it is connected, so it does not try
        // to log out in the destructor.
        acc.setup_setStatus(TestAccount::LOGGING_OUT);
    }

    // Test createCharacter() fails if we fake connected and taking char
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        Atlas::Objects::Entity::Anonymous ent;

        con->test_setStatus(Eris::BaseConnection::CONNECTED);
        acc.setup_setStatus(TestAccount::TAKING_CHAR);

        Eris::Result res =  acc.createCharacter(ent);
        assert(res == Eris::DUPLICATE_CHAR_ACTIVE);

        // Make sure it no longer thinks it is connected, so it does not try
        // to log out in the destructor.
        acc.setup_setStatus(TestAccount::LOGGING_OUT);
    }

    // Test createCharacter() fails if we fake connected but not logged in
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        Atlas::Objects::Entity::Anonymous ent;

        con->test_setStatus(Eris::BaseConnection::CONNECTED);
        acc.setup_setStatus(TestAccount::LOGGING_IN);

        Eris::Result res =  acc.createCharacter(ent);
        assert(res == Eris::NOT_LOGGED_IN);
    }
    // FIXME Cover the rest of createCharacter once we can fake
    // connections.

    // FIXME Cover createCharacter() which is currently not finished.

    // Test takeCharacter() fails if ID is not a real character
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        Eris::Account acc(con);
        std::string fake_char_id("1");

        Eris::Result res =  acc.takeCharacter(fake_char_id);
        assert(res == Eris::BAD_CHARACTER_ID);
    }

    // Test takeCharacter() fails if not connected
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        std::string fake_char_id("1");

        acc.setup_fakeCharacter(fake_char_id);

        Eris::Result res =  acc.takeCharacter(fake_char_id);
        assert(res == Eris::NOT_CONNECTED);
    }

    // Test takeCharacter() fails if we fake connected and logging in
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        std::string fake_char_id("1");

        con->test_setStatus(Eris::BaseConnection::CONNECTED);
        acc.setup_setStatus(TestAccount::LOGGING_IN);
        acc.setup_fakeCharacter(fake_char_id);

        Eris::Result res =  acc.takeCharacter(fake_char_id);
        assert(res == Eris::NOT_LOGGED_IN);
    }

    // Test takeCharacter() fails if we fake connected and logging in
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        std::string fake_char_id("1");

        con->test_setStatus(Eris::BaseConnection::CONNECTED);
        acc.setup_setStatus(TestAccount::CREATING_CHAR);
        acc.setup_fakeCharacter(fake_char_id);

        Eris::Result res =  acc.takeCharacter(fake_char_id);
        assert(res == Eris::DUPLICATE_CHAR_ACTIVE);

        // Make sure it no longer thinks it is connected, so it does not try
        // to log out in the destructor.
        acc.setup_setStatus(TestAccount::LOGGING_OUT);
    }

    // Test takeCharacter() fails if we fake connected and logging in
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        std::string fake_char_id("1");

        con->test_setStatus(Eris::BaseConnection::CONNECTED);
        acc.setup_setStatus(TestAccount::TAKING_CHAR);
        acc.setup_fakeCharacter(fake_char_id);

        Eris::Result res =  acc.takeCharacter(fake_char_id);
        assert(res == Eris::DUPLICATE_CHAR_ACTIVE);

        // Make sure it no longer thinks it is connected, so it does not try
        // to log out in the destructor.
        acc.setup_setStatus(TestAccount::LOGGING_OUT);
    }
    // FIXME Cover the rest of takeCharacter once we can fake
    // connections.

    // FIXME Cover deactivateCharacter once we can fake connections.

    // Test isLoggedIn()
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);

        assert(!acc.isLoggedIn());
    }

    // Test isLoggedIn()
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);

        acc.setup_setStatus(TestAccount::LOGGED_IN);

        assert(acc.isLoggedIn());
    }

    // Test isLoggedIn()
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);

        acc.setup_setStatus(TestAccount::TAKING_CHAR);

        assert(acc.isLoggedIn());
    }

    // Test isLoggedIn()
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);

        acc.setup_setStatus(TestAccount::CREATING_CHAR);

        assert(acc.isLoggedIn());
    }

    // FIXME Cover internalLogin once we can fake connections.

    // FIXME Cover logoutResponse once we can fake connections.
#if 0
    // Test logoutResponse() does nothing when given the wrong op.
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);

        Atlas::Objects::Operation::RootOperation op;
        acc.test_logoutResponse(op);
    }
#endif

    // Test loginComplete() does nothing when not logged in.
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        Atlas::Objects::Entity::Account p;

        p->setUsername("bob");

        acc.test_loginComplete(p);
    }

    // Test loginComplete()
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        Atlas::Objects::Entity::Account p;
        SignalFlagger loginSuccess_checker;

        p->setUsername("bob");
        acc.LoginSuccess.connect(sigc::mem_fun(loginSuccess_checker,
                                               &SignalFlagger::set));

        acc.test_loginComplete(p);

        assert(acc.isLoggedIn());
        assert(loginSuccess_checker.flagged());
        // FIXME Verify timeout has been cleared?
    }

    // Test updateFromObject() with a bad character list
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        Atlas::Objects::Entity::Account p;

        p->setAttr("character_types", "non-list");

        acc.test_updateFromObject(p);
    }

    // Test updateFromObject() with a character list with something bad in it
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        Atlas::Objects::Entity::Account p;

        Atlas::Message::ListType character_types;
        character_types.push_back(1);
        character_types.push_back("string");

        p->setAttr("character_types", character_types);

        acc.test_updateFromObject(p);
    }

    // Test loginError() with no arg
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        Atlas::Objects::Operation::Error err;

        acc.test_loginError(err);
    }

    // Test loginError() with empty arg
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        Atlas::Objects::Operation::Error err;
        Atlas::Objects::Root err_arg;
        err->setArgs1(err_arg);

        acc.test_loginError(err);
    }

    // Test loginError() with arg that has non-string message
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        Atlas::Objects::Operation::Error err;
        Atlas::Objects::Root err_arg;
        err_arg->setAttr("message", 1);
        err->setArgs1(err_arg);

        acc.test_loginError(err);
    }

    // Test loginError() with arg that has non-string message
    {
        TestConnection * con = new TestConnection("name", "localhost",
                                                  6767, true);

        TestAccount acc(con);
        Atlas::Objects::Operation::Error err;
        Atlas::Objects::Root err_arg;
        SignalFlagger loginFailure_checker;

        err_arg->setAttr("message", "Yur loginz, let me show you them.");
        err->setArgs1(err_arg);
        acc.LoginFailure.connect(sigc::hide(sigc::mem_fun(loginFailure_checker,
                                                          &SignalFlagger::set)));

        acc.test_loginError(err);

        assert(loginFailure_checker.flagged());
    }

    // NEXT handleLoginTimeout

    return 0;
}
