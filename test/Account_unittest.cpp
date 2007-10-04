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

    static const Eris::Account::Status LOGGING_IN = Eris::Account::LOGGING_IN;
    static const Eris::Account::Status LOGGING_OUT = Eris::Account::LOGGING_OUT;
};

int main()
{
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

    NEXT refreshCharacterInfo, much like login() tests.
    return 0;
}
