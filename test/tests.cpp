#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <cstdio>
#include <memory>

#include "stubServer.h"
#include "testUtils.h"
#include "controller.h"

#include <Eris/Connection.h>
#include <Eris/Account.h>
#include <Eris/Avatar.h>
#include <Eris/Entity.h>
#include <Eris/View.h>
#include <Eris/PollDefault.h>
#include <sigc++/object_slot.h>
#include <sigc++/object.h>
#include <Eris/LogStream.h>
#include <Eris/Log.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Codecs/Bach.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Message/MEncoder.h>

#include <signal.h>
#include <sys/socket.h>

using namespace Eris;
using std::endl;
using std::cout;
using std::cerr;

typedef std::auto_ptr<Eris::Connection> AutoConnection;
typedef std::auto_ptr<Eris::Account> AutoAccount;
typedef std::auto_ptr<Eris::Avatar> AutoAvatar;

typedef WFMath::Point<3> Point3;

class SignalCounter0 : public SigC::Object
{
public:
    SignalCounter0() :
        m_count(0)
    {;}
    
    void fired()
    {
        ++m_count;
    }
    
    int fireCount() const
    { return m_count; }
private:
    int m_count;
};

template <class P0>
class SignalCounter1 : public SigC::Object
{
public:
    SignalCounter1() :
        m_count(0)
    {;}
    
    void fired(P0)
    {
        ++m_count;
    }
    
    int fireCount() const
    { return m_count; }
    
private:
    int m_count;
};

template <class P0>
class SignalRecorder1 : public SigC::Object
{
public:
    SignalRecorder1() :
        m_count(0)
    {;}
    
    void fired(P0 m)
    {
        ++m_count;
        m_lastArg0 = m;
    }
    
    int fireCount() const
    { return m_count; }
    
    P0 lastArg0() const
    {
        return m_lastArg0;
    }
    
private:
    int m_count;
    P0 m_lastArg0;
};

template <class P0>
class SignalRecorderRef1 : public SigC::Object
{
public:
    SignalRecorderRef1() :
        m_count(0)
    {;}
    
    void fired(const P0& m)
    {
        ++m_count;
        m_lastArg0 = m;
    }
    
    int fireCount() const
    { return m_count; }
    
    P0 lastArg0() const
    {
        return m_lastArg0;
    }
    
private:
    int m_count;
    P0 m_lastArg0;
};

class DummyDecoder : public Atlas::Objects::ObjectsDecoder
{
protected:
    void objectArrived(const Atlas::Objects::Root&)
    {
    }
};

void erisLog(Eris::LogLevel level, const std::string& msg)
{
    switch (level) {
    case LOG_ERROR:
        cerr << "ERIS_ERROR: " << msg << endl; break;
        
    case LOG_WARNING:
        cout << "ERIS_WARN: " << msg << endl; break;
        
    default:
        cout << "ERIS: " << msg << endl;
       return;
    }
}

AutoConnection stdConnect()
{
    AutoConnection con(new Eris::Connection("eris-test", "localhost", 7450, false));
    SignalCounter0 connectCount;
    con->Connected.connect(SigC::slot(connectCount, &SignalCounter0::fired));
    
    SignalCounter1<const std::string&> fail;
    con->Failure.connect(SigC::slot(fail, &SignalCounter1<const std::string&>::fired));
    
    con->connect();
    
    while (!connectCount.fireCount() && !fail.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(con->isConnected());
    return con;
}

AutoAccount stdLogin(const std::string& uname, const std::string& pwd, Eris::Connection* con)
{
    AutoAccount player(new Eris::Account(con));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));
   
    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(SigC::slot(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->login(uname, pwd);

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 0);
    return player;
}

class AvatarGetter : public SigC::Object
{
public:
    AvatarGetter(Eris::Account* acc) : 
        m_acc(acc),
        m_expectFail(false),
        m_failed(false)
    {
        m_acc->AvatarSuccess.connect(SigC::slot(*this, &AvatarGetter::success));
        m_acc->AvatarFailure.connect(SigC::slot(*this, &AvatarGetter::failure));
    }
    
    void expectFailure()
    {
        m_expectFail = true;
    }
    
    AutoAvatar take(const std::string& charId)
    {        
        m_waiting = true;
        m_failed = false;
        
        m_acc->takeCharacter(charId);
        
        while (m_waiting) Eris::PollDefault::poll();
        
        // we wanted to fail, bail out now
        if (m_failed && m_expectFail) return AutoAvatar();
        
        assert(m_av->getEntity() == NULL); // shouldn't have the entity yet
        assert(m_av->getId() == charId); // but should have it's ID
    
        SignalCounter1<Entity*> gotChar;
        m_av->GotCharacterEntity.connect(SigC::slot(gotChar, &SignalCounter1<Entity*>::fired));
    
        while (gotChar.fireCount() == 0) Eris::PollDefault::poll();
    
        assert(m_av->getEntity());
        assert(m_av->getEntity()->getId() == charId);

        return m_av;
    }
    
private:
    void success(Avatar* av)
    {
        m_av.reset(av);
        m_waiting = false;
    }
    
    void failure(const std::string& msg)
    {
        if (msg != "deliberate") {
            std::cerr << "failure getting an avatar: " << msg << endl;
        }
        m_waiting = false;
        m_failed = true;
    }

    bool m_waiting;
    Eris::Account* m_acc;
    AutoAvatar m_av;
    bool m_expectFail, m_failed;
};

class WaitForAppearance : public SigC::Object
{
public:
    WaitForAppearance(Eris::View* v, const std::string& eid) : 
        m_view(v)
    {
        waitFor(eid);
        v->Appearance.connect(SigC::slot(*this, &WaitForAppearance::onAppear));
    }
    
    void waitFor(const std::string& eid)
    {
        Eris::Entity* e = m_view->getEntity(eid);
        if (e && e->isVisible()) {
            return;
        }
        
        m_waiting.insert(eid);
    }
    
    void run()
    {
        while (!m_waiting.empty()) Eris::PollDefault::poll();
    }
private:
    void onAppear(Eris::Entity* e)
    {
        m_waiting.erase(e->getId());
    }
    
    Eris::View* m_view;
    std::set<std::string> m_waiting;
};

class WaitForDisappearance : public SigC::Object
{
public:
    WaitForDisappearance(Eris::View* v, const std::string& eid) : 
        m_view(v)
    {
        waitFor(eid);
        v->Disappearance.connect(SigC::slot(*this, &WaitForDisappearance::onDisappear));
    }
    
    void waitFor(const std::string& eid)
    {
        Eris::Entity* e = m_view->getEntity(eid);
        if (!e || !e->isVisible()) {
            cerr << "wait on invisible entity " << eid << endl;
            return;
        }
        
        m_waiting.insert(eid);
    }
    
    void run()
    {
        while (!m_waiting.empty()) Eris::PollDefault::poll();
    }
private:
    void onDisappear(Eris::Entity* e)
    {
        m_waiting.erase(e->getId());
    }
    
    Eris::View* m_view;
    std::set<std::string> m_waiting;
};
    
class AttributeTracker : public SigC::Object
{
public:
    void waitForChangeOf(Eris::Entity* e, const std::string& attr)
    {
        assert(e);
        m_changed = false;
        
        e->observe(attr, SigC::slot(*this, &AttributeTracker::attrChange));
        while (!m_changed) Eris::PollDefault::poll();
    }
    
    Atlas::Message::Element newValue() const 
    { return m_value; }
    
private:
    void attrChange(const std::string& attr, const Atlas::Message::Element& v)
    {
        m_name = attr;
        m_value = v;
        m_changed = true;
    }
    
    bool m_changed;
    std::string m_name;
    Atlas::Message::Element m_value;
};
    
#pragma mark -

void testLogin()
{
    AutoConnection con = stdConnect();
    AutoAccount player(new Eris::Account(con.get()));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));
   
    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(SigC::slot(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->login("account_A", "pumpkin");

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 0);
    
    assert(player->getId() == "_23_account_A");
    assert(player->isLoggedIn());
}

void testBadLogin()
{
    AutoConnection con = stdConnect();
    AutoAccount player(new Eris::Account(con.get()));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));
   
    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(SigC::slot(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->login("account_B", "zark!!!!ojijiksapumpkin");

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 1);
    assert(loginCount.fireCount() == 0);
    assert(!player->isLoggedIn());
}

void testBadLogin2()
{
    AutoConnection con = stdConnect();
    AutoAccount player(new Eris::Account(con.get()));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));
   
    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(SigC::slot(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->login("_timeout_", "foo");

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 1);
    assert(loginCount.fireCount() == 0);
    assert(!player->isLoggedIn());
}

void testAccCreate()
{
    AutoConnection con = stdConnect();
    AutoAccount player(new Eris::Account(con.get()));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));

    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(SigC::slot(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->createAccount("account_C", "John Doe", "lemon");

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 0);
    
    assert(player->getUsername() == "account_C");
    assert(player->isLoggedIn());
}

void testAccountCharacters()
{
    AutoConnection con = stdConnect();
    AutoAccount player = stdLogin("account_B", "sweede", con.get());

    SignalCounter0 gotChars;
    player->GotAllCharacters.connect(SigC::slot(gotChars, &SignalCounter0::fired));
    player->refreshCharacterInfo();
    
    while (gotChars.fireCount() == 0)
    {
        Eris::PollDefault::poll();
    }
    
    const CharacterMap& chars = player->getCharacters();
    CharacterMap::const_iterator C = chars.find("acc_b_character");
    assert(C != chars.end());
    assert(C->second->getName() == "Joe Blow");
}

void testLogout()
{
    AutoConnection con = stdConnect();
    AutoAccount player = stdLogin("account_C", "lemon", con.get());
    
    SignalCounter1<bool> gotLogout;
    player->LogoutComplete.connect(SigC::slot(gotLogout, &SignalCounter1<bool>::fired));
    player->logout();
    
    while (gotLogout.fireCount() == 0) Eris::PollDefault::poll();
    
    assert(!player->isLoggedIn());
}

void testCharActivate(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount player = stdLogin("account_B", "sweede", con.get());

    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    AutoAvatar av = AvatarGetter(player.get()).take("acc_b_character");
    
    Eris::View* v = av->getView();
    assert(v->getTopLevel()->getId() == "_world");
    
    assert(v->getTopLevel()->hasChild("_hut_01"));
    assert(!v->getTopLevel()->hasChild("_field_01")); // not yet
}

void testBadTake()
{
    AutoConnection con = stdConnect();
    AutoAccount player = stdLogin("account_B", "sweede", con.get());
    
    AvatarGetter g(player.get());
    g.expectFailure();
    AutoAvatar av2 = g.take("_fail_");
    assert(av2.get() == NULL);
}

void testAppearance(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_B", "sweede", con.get());
    
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    AutoAvatar av = AvatarGetter(acc.get()).take("acc_b_character");
    
    Eris::View* v = av->getView();
    
    ctl.setEntityVisibleToAvatar("_potato_2", av.get());
    ctl.setEntityVisibleToAvatar("_pig_01", av.get());
    ctl.setEntityVisibleToAvatar("_field_01", av.get());
        
    {
        WaitForAppearance wf(v, "_field_01");
        wf.waitFor("_pig_01");
        wf.waitFor("_potato_2");
        wf.run();
    }
    
    ctl.setEntityInvisibleToAvatar("_field_01", av.get());
    WaitForDisappearance df(v, "_field_01");
    df.waitFor("_pig_01");
    df.waitFor("_potato_2");
    
    df.run();
    
    ctl.setEntityVisibleToAvatar("_potato_1", av.get());
    //
    ctl.setEntityVisibleToAvatar("_pig_02", av.get());
    ctl.setEntityVisibleToAvatar("_field_01", av.get());
    
    {
        WaitForAppearance af2(v, "_field_01");
        af2.waitFor("_potato_1");
        af2.run();
    }
}

void testSet(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_B", "sweede", con.get());
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_table_1", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_vase_1", "acc_b_character");
    
    AutoAvatar av = AvatarGetter(acc.get()).take("acc_b_character");

    {
        WaitForAppearance wf(av->getView(), "_table_1");
        wf.waitFor("_vase_1");
        wf.run();
    }
    
    Eris::Entity* table = av->getView()->getEntity("_table_1");
    assert(table);
        
    assert(table->getPosition() == WFMath::Point<3>(1.0, 2.0, 3.0));
    assert(table->numContained() == 1);
    assert(table->getName() == "George");
    
    ctl.setAttr("_table_1", "status", "funky");
    
    AttributeTracker at;
    at.waitForChangeOf(table, "status");
        
    assert(at.newValue() == std::string("funky"));
    // check the value made it through
    assert(table->valueOfAttr("status") == std::string("funky"));
    assert(table->getName() == "George");
    
    // and that nothing else got bent
    assert(table->getPosition() == WFMath::Point<3>(1.0, 2.0, 3.0));
    assert(table->numContained() == 1);
}

class WaitForSay : public SigC::Object
{
public:
    WaitForSay(Eris::Entity* e, const std::string& what) : 
        m_what(what),
        m_heard(false)
    {
        e->Say.connect(SigC::slot(*this, &WaitForSay::onSay));
    }
    
    void run()
    {
        while (!m_heard) Eris::PollDefault::poll();
    }
private:
    void onSay(const std::string& what)
    {
        if (what == m_what) m_heard = true;
    }

    std::string m_what;
    bool m_heard;
};

void testTalk(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_B", "sweede", con.get());
    
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_table_1", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_vase_1", "acc_b_character");
    
    AutoAvatar av = AvatarGetter(acc.get()).take("acc_b_character");
    
    {
        WaitForAppearance wf(av->getView(), "_table_1");
        wf.waitFor("_vase_1");
        wf.run();
    }
    
    Eris::Entity* vase = av->getView()->getEntity("_vase_1");
    WaitForSay wfs(vase, "cut yourself.");
    
    {
        Atlas::Objects::Operation::Talk t;
        Atlas::Objects::Root what;
        what->setAttr("say", "cut yourself.");
        t->setArgs1(what);
        t->setFrom("_vase_1");    
        ctl.broadcastSoundFrom("_vase_1", t);
    }
    
    wfs.run();
}


void testSeeMove(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_B", "sweede", con.get());
    
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_table_1", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_vase_1", "acc_b_character");
    
    AutoAvatar av = AvatarGetter(acc.get()).take("acc_b_character");
    {
        WaitForAppearance wf(av->getView(), "_table_1");
        wf.waitFor("_vase_1");
        wf.run();
    }
    
    SignalCounter0 moved;
    Eris::Entity* vase = av->getView()->getEntity("_vase_1");
    vase->Moved.connect(SigC::slot(moved, &SignalCounter0::fired));
    
 //   assert(vase->getPosition() == Point3(1.0, 2.0, 3.0));

    Point3 newPos(-2, -5, 1);
    ctl.movePos(vase->getId(), newPos);
      
    while (!moved.fireCount()) {
        Eris::PollDefault::poll();
    }
    
    assert(vase->getPosition() == newPos);
}

void testLocationChange(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_B", "sweede", con.get());
    
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_table_1", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_vase_1", "acc_b_character");
    
    AutoAvatar av = AvatarGetter(acc.get()).take("acc_b_character");
    {
        WaitForAppearance wf(av->getView(), "_vase_1");
        wf.run();
    }
    
    SignalCounter0 moved;
    SignalRecorder1<Eris::Entity*> locChanged;
    
    Eris::Entity* vase = av->getView()->getEntity("_vase_1");
    Eris::Entity* hut = av->getView()->getEntity("_hut_01");
    Eris::Entity* table = av->getView()->getEntity("_table_1");
    
    vase->Moved.connect(SigC::slot(moved, &SignalCounter0::fired));
    vase->LocationChanged.connect(SigC::slot(locChanged, &SignalRecorder1<Eris::Entity*>::fired));
    
    Point3 newPos(1, -2, -1);
    ctl.moveLocation(vase->getId(), "_hut_01", newPos);
      
    while (!moved.fireCount() && !locChanged.fireCount()) {
        Eris::PollDefault::poll();
    }
    
    assert(locChanged.lastArg0() == table);
    assert(vase->getPosition() == newPos);
    assert(vase->getLocation() == hut);
}

void testSightAction(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_B", "sweede", con.get());
    
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_table_1", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_vase_1", "acc_b_character");
    
    AutoAvatar av = AvatarGetter(acc.get()).take("acc_b_character");
    {
        WaitForAppearance wf(av->getView(), "_vase_1");
        wf.run();
    }

    SignalRecorderRef1<Atlas::Objects::Operation::Action> action;
    Eris::Entity* vase = av->getView()->getEntity("_vase_1");
    vase->Acted.connect(SigC::slot(action, 
        &SignalRecorderRef1<Atlas::Objects::Operation::Action>::fired));
    
    Atlas::Objects::Operation::Touch t;
    t->setFrom(vase->getId());
    ctl.broadcastSightFrom(vase->getId(), t);
    
    while (!action.fireCount()) {
        Eris::PollDefault::poll();
    }
    
    assert(action.lastArg0()->asMessage() == t->asMessage());
}

int main(int argc, char **argv)
{
    int sockets[2];
    int err = socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);
    if (err != 0) {
        cerr << "unable to create socket-pair" << endl;
        return EXIT_FAILURE;
    }
    
    pid_t childPid = fork();
    if (childPid == 0)
    {
        Controller ctl(sockets[1]);
        
        Eris::setLogLevel(LOG_DEBUG);
        Eris::Logged.connect(SigC::slot(&erisLog));
    
        try {    
            testLogin();
            testBadLogin();
            testBadLogin2();
            testAccCreate();
            testLogout();
            testAccountCharacters();
            testCharActivate(ctl);
            testBadTake();
            
            testAppearance(ctl);
            testSet(ctl);
            testTalk(ctl);
            testSeeMove(ctl);
            testLocationChange(ctl);
            testSightAction(ctl);
        }
        catch (TestFailure& tfexp)
        {
            cout << "tests failed: " << tfexp.what() << endl;
            return EXIT_FAILURE;
        }
        catch (std::exception& except)
        {
            cout << "caught general exception: " << except.what() << endl;
            return EXIT_FAILURE;
        }

        cout << "all tests passed" << endl;
        return EXIT_SUCCESS;
    } else {
        sleep(1);
        
        Eris::setLogLevel(LOG_DEBUG);
        Eris::Logged.connect(SigC::slot(&erisLog));
        
        StubServer stub(7450, sockets[0]);
        return stub.run(childPid);
    }
}


