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
#include <Eris/ViewEntity.h>
#include <Eris/View.h>
#include <Eris/Log.h>

#include "SignalFlagger.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <sigc++/adaptors/hide.h>

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
               Eris::Avatar(*ac, ent_id) { }

    void setup_setEntity(Eris::Entity * ent) {
        m_entity = ent;
        m_entityId = ent->getId();
    }

    void test_onEntityAppear(Eris::Entity * ent) {
        onEntityAppear(ent);
    }

    void test_onCharacterChildAdded(Eris::Entity * ent) {
        onCharacterChildAdded(ent);
    }

    void test_onCharacterChildRemoved(Eris::Entity * ent) {
        onCharacterChildRemoved(ent);
    }

    void test_onCharacterWield(const Atlas::Message::Element & val) {
        onCharacterWield(val);
    }

    void test_updateWorldTime(double seconds) {
        updateWorldTime(seconds);
    }

    void test_logoutResponse(const Atlas::Objects::Operation::RootOperation & op) {
        logoutResponse(op);
    }
};

class TestEntity : public Eris::ViewEntity {
  public:
    TestEntity(const std::string& id, Eris::TypeInfo* ty, Eris::View* vw) :
               Eris::ViewEntity(id, ty, vw) { }

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
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());
        TestEntity * inv_ent = new TestEntity("2", 0, ea->getView());

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
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());
        TestEntity * inv_ent = new TestEntity("2", 0, ea->getView());

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
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());
        TestEntity * inv_ent = new TestEntity("2", 0, ea->getView());

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
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());
        TestEntity * inv_ent = new TestEntity("2", 0, ea->getView());

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
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());
        TestEntity * inv_ent = new TestEntity("2", 0, ea->getView());

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
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());

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
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());

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
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);

        ea->moveToPoint(WFMath::Point<3>(3,4,5));
    }
    
    // Test moveInDirection() with zero velocity
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);

        ea->moveInDirection(WFMath::Vector<3>(0,0,0));
    }
    
    // Test moveInDirection() with vertical velocity
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);

        ea->moveInDirection(WFMath::Vector<3>(0,0,5));
    }
    
    // Test moveInDirection() with high velocity
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);

        ea->moveInDirection(WFMath::Vector<3>(3,4,5));
    }
    
    // FIXME test the conditionals in moveInDirection()

    // Test moveInDirection()
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);

        ea->moveInDirection(WFMath::Vector<3>(3,4,5), WFMath::Quaternion());
    }
    
    // Test place() of something
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());
        TestEntity * inv_ent = new TestEntity("2", 0, ea->getView());

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);
        inv_ent->setup_setLocation(wrld_ent);

        ea->place(inv_ent, wrld_ent, WFMath::Point<3>(1,2,3));
    }
    
    // Test wield() of something not in inventory
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());
        TestEntity * inv_ent = new TestEntity("2", 0, ea->getView());

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);
        inv_ent->setup_setLocation(wrld_ent);

        ea->wield(inv_ent);
    }
    
    // Test wield() of something in inventory
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());
        TestEntity * inv_ent = new TestEntity("2", 0, ea->getView());

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);
        inv_ent->setup_setLocation(char_ent);

        ea->wield(inv_ent);
    }
    
    // Test useOn() of something in inventory
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());
        TestEntity * inv_ent = new TestEntity("2", 0, ea->getView());

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);
        inv_ent->setup_setLocation(char_ent);

        ea->useOn(inv_ent, WFMath::Point<3>(), std::string());
    }
    
    // Test useOn() of something in inventory
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());
        TestEntity * inv_ent = new TestEntity("2", 0, ea->getView());

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);
        inv_ent->setup_setLocation(char_ent);

        ea->useOn(inv_ent, WFMath::Point<3>(1,1,1), std::string());
    }
    
    // Test useOn() with a position
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());
        TestEntity * inv_ent = new TestEntity("2", 0, ea->getView());

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);
        inv_ent->setup_setLocation(char_ent);

        ea->useOn(inv_ent, WFMath::Point<3>(1,1,1), std::string());
    }
    
    // Test useOn() with a pos and target
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());
        TestEntity * inv_ent = new TestEntity("2", 0, ea->getView());

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);
        inv_ent->setup_setLocation(char_ent);

        ea->useOn(inv_ent, WFMath::Point<3>(1,1,1), "0");
    }
    
    // Test attack()
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());
        TestEntity * other_ent = new TestEntity("2", 0, ea->getView());

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);
        other_ent->setup_setLocation(wrld_ent);

        ea->attack(other_ent);
    }
    
    // Test useStop()
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        TestEntity * wrld_ent = new TestEntity("0", 0, ea->getView());
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());

        ea->setup_setEntity(char_ent);
        char_ent->setup_setLocation(wrld_ent);

        ea->useStop();
    }
    
    // Test onEntityAppear() for avatar entity
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        TestEntity * char_ent = new TestEntity(fake_char_id, 0, ea->getView());
        SignalFlagger gotCharacterEntity;

        ea->GotCharacterEntity.connect(sigc::hide(sigc::mem_fun(gotCharacterEntity, &SignalFlagger::set)));

        ea->test_onEntityAppear(char_ent);

        assert(ea->getEntity() == char_ent);
        assert(gotCharacterEntity.flagged());
    }
    
    // Test onEntityAppear() for a different entity
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_char_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_char_id);
        TestEntity * char_ent = new TestEntity("2", 0, ea->getView());
        SignalFlagger gotCharacterEntity;

        ea->GotCharacterEntity.connect(sigc::hide(sigc::mem_fun(gotCharacterEntity, &SignalFlagger::set)));

        ea->test_onEntityAppear(char_ent);

        assert(ea->getEntity() != char_ent);
        assert(!gotCharacterEntity.flagged());
    }
    
    // Test onCharacterChildAdded()
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_id);
        TestEntity * ent = new TestEntity("2", 0, ea->getView());
        SignalFlagger invAdded;

        ea->InvAdded.connect(sigc::hide(sigc::mem_fun(invAdded, &SignalFlagger::set)));

        ea->test_onCharacterChildAdded(ent);

        assert(invAdded.flagged());
    }
    
    // Test onCharacterChildRemoved()
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_id);
        TestEntity * ent = new TestEntity("2", 0, ea->getView());
        SignalFlagger invRemoved;

        ea->InvRemoved.connect(sigc::hide(sigc::mem_fun(invRemoved, &SignalFlagger::set)));

        ea->test_onCharacterChildRemoved(ent);

        assert(invRemoved.flagged());
    }
    
    // Test onCharacterWield() with a non string ID.
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_id);

        ea->test_onCharacterWield(1);
    }
    
    // Test onCharacterWield() with a non string ID.
    // It is currently very hard to simulate the entity being in the view.
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_id);
        new TestEntity("2", 0, ea->getView());

        ea->test_onCharacterWield("2");
    }
    
    // Test getConnection()
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_id);

        assert(ea->getConnection() == con);
    }

    // Test getWorldTime()
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_id);

        ea->getWorldTime();
    }

    // Test updateWorldTime()
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_id);

        ea->test_updateWorldTime(100.0);
    }

    // Test logoutResponse() with a non-info operation
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_id);
        Atlas::Objects::Operation::Get op;
        SignalFlagger avatarDeactivated;

        acc->AvatarDeactivated.connect(sigc::hide(sigc::mem_fun(avatarDeactivated, &SignalFlagger::set)));

        ea->test_logoutResponse(op);

        assert(!avatarDeactivated.flagged());
    }

    // Test logoutResponse() with an empty info operation
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_id);
        Atlas::Objects::Operation::Info op;
        SignalFlagger avatarDeactivated;

        acc->AvatarDeactivated.connect(sigc::hide(sigc::mem_fun(avatarDeactivated, &SignalFlagger::set)));

        ea->test_logoutResponse(op);

        assert(!avatarDeactivated.flagged());
    }

    // Test logoutResponse() with an info operation with bad arg
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_id);
        Atlas::Objects::Operation::Info op;
        Atlas::Objects::Root bad_arg;
        SignalFlagger avatarDeactivated;

        acc->AvatarDeactivated.connect(sigc::hide(sigc::mem_fun(avatarDeactivated, &SignalFlagger::set)));
        op->setArgs1(bad_arg);

        ea->test_logoutResponse(op);

        assert(!avatarDeactivated.flagged());
    }

    // Test logoutResponse() with an empty info logout operation
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_id);
        Atlas::Objects::Operation::Info op;
        Atlas::Objects::Operation::Logout logout;
        SignalFlagger avatarDeactivated;

        acc->AvatarDeactivated.connect(sigc::hide(sigc::mem_fun(avatarDeactivated, &SignalFlagger::set)));
        op->setArgs1(logout);

        ea->test_logoutResponse(op);

        assert(!avatarDeactivated.flagged());
    }

    // Test logoutResponse() with a non-empty info logout operation
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_id);
        Atlas::Objects::Operation::Info op;
        Atlas::Objects::Operation::Logout logout;
        Atlas::Objects::Root logout_arg;
        SignalFlagger avatarDeactivated;

        acc->AvatarDeactivated.connect(sigc::hide(sigc::mem_fun(avatarDeactivated, &SignalFlagger::set)));
        op->setArgs1(logout);
        logout->setArgs1(logout_arg);

        ea->test_logoutResponse(op);

        assert(!avatarDeactivated.flagged());
    }

    // Test logoutResponse() with a non-empty info logout operation
    {
        Eris::Connection * con = new TestConnection("name", "localhost",
                                                    6767, true);

        Eris::Account * acc = new TestAccount(con);
        std::string fake_id("1");
        TestAvatar * ea = new TestAvatar(acc, fake_id);
        Atlas::Objects::Operation::Info op;
        Atlas::Objects::Operation::Logout logout;
        Atlas::Objects::Root logout_arg;
        SignalFlagger avatarDeactivated;

        acc->AvatarDeactivated.connect(sigc::hide(sigc::mem_fun(avatarDeactivated, &SignalFlagger::set)));
        op->setArgs1(logout);
        logout->setArgs1(logout_arg);
        logout_arg->setId(fake_id);

        ea->test_logoutResponse(op);

        assert(avatarDeactivated.flagged());
    }

    return 0;
}
