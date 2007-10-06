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

#include <Eris/Avatar.h>

#include <Eris/Connection.h>
#include <Eris/Account.h>
#include <Eris/Log.h>

#include <Atlas/Objects/SmartPtr.h>

#include <iostream>

class TestConnection : public Eris::Connection {
  public:
    TestConnection(const std::string & name, const std::string & host,
                   short port, bool debug) :
                   Eris::Connection(name, host, port, debug) { }

    virtual void send(const Atlas::Objects::Root &obj) {
        std::cout << "Sending " << obj->getParents().front()
                  << std::endl << std::flush;
    }
};

class TestAccount : public Eris::Account {
  public:
    TestAccount(Eris::Connection * con) : Eris::Account(con) { }

    void setup_insertActiveCharacters(Eris::Avatar * ea) {
        m_activeCharacters.insert(std::make_pair(ea->getId(), ea));
    }
};

class TestAvatar : public Eris::Avatar {
  public:
    TestAvatar(Eris::Account * ac, const std::string & ent_id) :
               Eris::Avatar(ac, ent_id) { }
};

static void writeLog(Eris::LogLevel, const std::string & msg)
{       
    std::cerr << msg << std::endl << std::flush;
}

int main()
{
    Eris::Logged.connect(sigc::ptr_fun(writeLog));
    Eris::setLogLevel(Eris::LOG_DEBUG);

    // Test constructor
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        new TestAvatar(acc, fake_char_id);
    }

    // Test destructor
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        TestAccount * acc = new TestAccount(con);
        std::string fake_char_id("1");
        Eris::Avatar * ea = new TestAvatar(acc, fake_char_id);

        // The account must know about this Avatar, as avatar removes itself
        // from account on destruction.
        acc->setup_insertActiveCharacters(ea);

        delete ea;
    }

    // Test deactivate()
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        Eris::Avatar * ea = new TestAvatar(acc, fake_char_id);

        ea->deactivate();
    }

    

    return 0;
}
