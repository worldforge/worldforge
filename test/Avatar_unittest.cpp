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
#include <Eris/Entity.h>
#include <Eris/View.h>
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

    void setup_setEntity(Eris::Entity * ent) {
        m_entity = ent;
        m_entityId = ent->getId();
    }
};

class TestEntity : public Eris::Entity {
  public:
    TestEntity(const std::string& id, Eris::TypeInfo* ty, Eris::View* vw) :
               Eris::Entity(id, ty, vw) { }

    void setup_setLocation(Eris::Entity * e) {
        setLocation(e);
    }
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

    // Test drop() of something not in inventory
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        Eris::View * vw = new Eris::View(ea);
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, vw);
        TestEntity * inv_ent = new TestEntity("2", 0, vw);

        ea->setup_setEntity(char_ent);

        ea->drop(inv_ent, WFMath::Point<3>(0,0,0), "3");
    }
    
    // Test drop() of something in inventory by position
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        Eris::View * vw = new Eris::View(ea);
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, vw);
        TestEntity * inv_ent = new TestEntity("2", 0, vw);

        ea->setup_setEntity(char_ent);
        inv_ent->setup_setLocation(char_ent);

        ea->drop(inv_ent, WFMath::Point<3>(0,0,0), "3");
    }
    
    // Test drop() of something in inventory by offset
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        Eris::View * vw = new Eris::View(ea);
        TestEntity * wrld_ent = new TestEntity("0", 0, vw);
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, vw);
        TestEntity * inv_ent = new TestEntity("2", 0, vw);

        char_ent->setup_setLocation(wrld_ent);
        ea->setup_setEntity(char_ent);
        inv_ent->setup_setLocation(char_ent);

        ea->drop(inv_ent, WFMath::Vector<3>(0,0,0));
    }
    
    // Test take() of something
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        Eris::View * vw = new Eris::View(ea);
        TestEntity * wrld_ent = new TestEntity("0", 0, vw);
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, vw);
        TestEntity * inv_ent = new TestEntity("2", 0, vw);

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);
        inv_ent->setup_setLocation(wrld_ent);

        ea->take(inv_ent);
    }
    
    // Test touch() of something
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        Eris::View * vw = new Eris::View(ea);
        TestEntity * wrld_ent = new TestEntity("0", 0, vw);
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, vw);
        TestEntity * inv_ent = new TestEntity("2", 0, vw);

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);
        inv_ent->setup_setLocation(wrld_ent);

        ea->touch(inv_ent);
    }
    
    // Test say() of a message
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        Eris::View * vw = new Eris::View(ea);
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, vw);

        ea->setup_setEntity(char_ent);

        ea->say("Hello world");
    }
    
    // Test say() of a message
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        Eris::View * vw = new Eris::View(ea);
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, vw);

        ea->setup_setEntity(char_ent);

        ea->emote("greets the world.");
    }
    
    // Test moveToPoint()
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        Eris::View * vw = new Eris::View(ea);
        TestEntity * wrld_ent = new TestEntity("0", 0, vw);
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, vw);

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);

        ea->moveToPoint(WFMath::Point<3>(3,4,5));
    }
    
    // Test moveInDirection()
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        Eris::View * vw = new Eris::View(ea);
        TestEntity * wrld_ent = new TestEntity("0", 0, vw);
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, vw);

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);

        ea->moveInDirection(WFMath::Vector<3>(3,4,5));
    }
    
    // FIXME test the conditionals in moveInDirection()

    return 0;
}
