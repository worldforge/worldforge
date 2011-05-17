#include <skstream/skstream.h>

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "stubServer.h"
#include "testUtils.h"
#include "controller.h"
#include "signalHelpers.h"
#include "setupHelpers.h"
#include "testOutOfGame.h"
#include "netTests.h"
#include "viewTest.h"
#include "avatarTest.h"
#include "calendarTest.h"

#include <Eris/Connection.h>
#include <Eris/Account.h>
#include <Eris/Avatar.h>
#include <Eris/Entity.h>
#include <Eris/View.h>
#include <Eris/PollDefault.h>
#include <sigc++/functors/mem_fun.h>
#include <sigc++/adaptors/bind.h>
#include <Eris/LogStream.h>
#include <Eris/Log.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Codecs/Bach.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Message/MEncoder.h>

#include <cstdio>
#include <cstring>
#include <memory>

#include <signal.h>
#include <sys/socket.h>
#include <wfmath/atlasconv.h>

#pragma warning(disable: 4068)  //unknown pragma

using namespace Eris;
using std::endl;
using std::cout;
using std::cerr;

using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Move;

typedef WFMath::Point<3> Point3;

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
    
class AttributeTracker
{
public:
    void waitForChangeOf(Eris::Entity* e, const std::string& attr)
    {
        assert(e);
        m_changed = false;
        
        e->observe(attr, sigc::bind(sigc::mem_fun(*this, &AttributeTracker::attrChange),attr));
        while (!m_changed) Eris::PollDefault::poll();
    }
    
    Atlas::Message::Element newValue() const 
    { return m_value; }
    
private:
    void attrChange(const Atlas::Message::Element& v, const std::string& attr)
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

void testCharCreate()
{
    AutoConnection con = stdConnect();
    AutoAccount player = stdLogin("account_A", "pumpkin", con.get());
    
    RootEntity charEnt;
    StringList prs;
    prs.push_back("settler");
    charEnt->setParents(prs);
    charEnt->setName("Fruitfucker");
    charEnt->setAttr("foo", 42);
    
    AutoAvatar av = AvatarGetter(player.get()).create(charEnt);
    
    assert(av->getEntity()->getName() == "Fruitfucker");
    assert(av->getEntity()->valueOfAttr("foo").asInt() == 42);
    
    assert(player->getActiveCharacters().count(av->getId()) == 1);
}

void testBadCreate()
{
    AutoConnection con = stdConnect();
    AutoAccount player = stdLogin("account_A", "pumpkin", con.get());
    
    AvatarGetter g(player.get());
    g.expectFailure();
    
    RootEntity charEnt;
    StringList prs;
    prs.push_back("__fail__");
    charEnt->setParents(prs);
    
    AutoAvatar av2 = g.create(charEnt);
    assert(av2.get() == NULL);
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
    
    // and that nothing else got bent
    assert(table->getPosition() == WFMath::Point<3>(1.0, 2.0, 3.0));
    assert(table->numContained() == 1);
    
    assert(table->getName() == "George");
}

class WaitForSay
{
public:
    WaitForSay(Eris::Entity* e, const std::string& what) : 
        m_what(what),
        m_heard(false)
    {
        e->Say.connect(sigc::mem_fun(*this, &WaitForSay::onSay));
    }
    
    void run()
    {
        while (!m_heard) Eris::PollDefault::poll();
    }
private:
    void onSay(const Atlas::Objects::Root & what)
    {
		assert(what->hasAttr("say"));
		assert(what->getAttr("say").isString());
		assert(what->getAttr("say").asString() == m_what);
		
		m_heard = true;
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
    vase->Moved.connect(sigc::mem_fun(moved, &SignalCounter0::fired));
    
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
    
    vase->Moved.connect(sigc::mem_fun(moved, &SignalCounter0::fired));
    vase->LocationChanged.connect(sigc::mem_fun(locChanged, &SignalRecorder1<Eris::Entity*>::fired));
    
    Point3 newPos(30, -19, 8);
    ctl.moveLocation(vase->getId(), "_hut_01", newPos);
      
    while (!moved.fireCount() && !locChanged.fireCount()) {
        Eris::PollDefault::poll();
    }
    
    assert(locChanged.lastArg0() == table);
    assert(vase->getPosition() == newPos);
    assert(vase->getLocation() == hut);
}

void testSeeMoveWithUpdates(Controller& ctl)
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
    vase->Moved.connect(sigc::mem_fun(moved, &SignalCounter0::fired));
    assert(vase->valueOfAttr("stamina").asInt() == 105);
    
    Point3 newPos(-2, -5, 1);
    
    Move mv;
    Root arg;
    arg->setAttr("pos", newPos.toAtlas());
    arg->setAttr("stamina", 100);
    mv->setArgs1(arg);
        
    ctl.move(vase->getId(), mv);
      
    while (!moved.fireCount()) {
        Eris::PollDefault::poll();
    }
    
    assert(vase->getPosition() == newPos);
    assert(vase->valueOfAttr("stamina").asInt() == 100);
    
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

    SignalRecorderRef1<Atlas::Objects::Operation::RootOperation> action;
    Eris::Entity* vase = av->getView()->getEntity("_vase_1");
    vase->Acted.connect(sigc::mem_fun(action, 
        &SignalRecorderRef1<Atlas::Objects::Operation::RootOperation>::fired));
    
    Atlas::Objects::Operation::Touch t;
    t->setFrom(vase->getId());
    ctl.broadcastSightFrom(vase->getId(), t);
    
    while (!action.fireCount()) {
        Eris::PollDefault::poll();
    }
    
    assert(action.lastArg0()->asMessage() == t->asMessage());
// same again, but with a non-built-in op type    
    {
        Atlas::Objects::Operation::Action parry;
        parry->setFrom(vase->getId());
        StringList prs;
        prs.push_back("parry");
        parry->setParents(prs);
        
        action.reset();
        ctl.broadcastSightFrom(vase->getId(), parry);
        
        while (!action.fireCount()) {
            Eris::PollDefault::poll();
        }
        
        assert(action.lastArg0()->asMessage() == parry->asMessage());
    }
}

void testSoundAction(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_B", "sweede", con.get());
    
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_table_1", "acc_b_character");
    
    AutoAvatar av = AvatarGetter(acc.get()).take("acc_b_character");
    {
        WaitForAppearance wf(av->getView(), "_table_1");
        wf.run();
    }

    SignalRecorderRef1<Atlas::Objects::Root> heard;
    Eris::Entity* table = av->getView()->getEntity("_table_1");
    table->Noise.connect(sigc::mem_fun(heard, 
        &SignalRecorderRef1<Atlas::Objects::Root>::fired));
    
    Atlas::Objects::Operation::Touch t;
    t->setFrom(table->getId());
    ctl.broadcastSoundFrom(table->getId(), t);
    
    while (!heard.fireCount()) {
        Eris::PollDefault::poll();
    }
    
    assert(heard.lastArg0()->asMessage() == t->asMessage());
// same again, but with a non-built-in op type    
    {
        Atlas::Objects::Operation::Action parry;
        parry->setFrom(table->getId());
        StringList prs;
        prs.push_back("parry");
        parry->setParents(prs);
        
        heard.reset();
        ctl.broadcastSoundFrom(table->getId(), parry);
        
        while (!heard.fireCount()) {
            Eris::PollDefault::poll();
        }
        
        assert(heard.lastArg0()->asMessage() == parry->asMessage());
    }
}

class EntityDeleteWatcher
{
public:
    EntityDeleteWatcher(Eris::Entity* e) :
        m_ent(e),
        m_originalLoc(e->getLocation()),
        m_fired(false)
    { }
    
    void onEntityDelete(Eris::Entity* e)
    {
        assert(e == m_ent);
        assert(e->getLocation() == m_originalLoc);
        m_fired = true;
    }
    
    bool fired() const
    { return m_fired; }
    
private:
    Eris::Entity* m_ent;
    Eris::Entity* m_originalLoc;
    bool m_fired;
};

void testSightDelete(Controller& ctl)
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

    Eris::Entity* vase = av->getView()->getEntity("_vase_1");
    Eris::Entity* table = av->getView()->getEntity("_table_1");
    
    EntityDeleteWatcher dw(table);
    av->getView()->EntityDeleted.connect(sigc::mem_fun(dw, 
        &EntityDeleteWatcher::onEntityDelete));
    
    assert(vase->getLocation() == table);
    assert(vase->getPosition() == Point3(50, 40, 0));
    
    ctl.deleteEntity("_table_1");
    
    while (!dw.fired()) Eris::PollDefault::poll();
    
    
    assert(vase->getLocation() == av->getView()->getEntity("_hut_01"));
    assert(vase->getPosition() == Point3(51, 42, 3));
}


void testMovement(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_B", "sweede", con.get());
    ctl.setEntityVisibleToAvatar("_ball", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    
    AutoAvatar av = AvatarGetter(acc.get()).take("acc_b_character");
    {
        WaitForAppearance wf(av->getView(), "_ball");
        wf.run();
    }

    Eris::Entity* ball = av->getView()->getEntity("_ball");
    assert(!ball->isMoving());
    
    WFMath::Vector<3> vel(2, 3, 0);
    ctl.moveVelocity("_ball", vel);
    
    SignalRecorder1<bool> moving;
    ball->Moving.connect(sigc::mem_fun(moving, &SignalRecorder1<bool>::fired));
    
    while (!moving.fireCount()) {
        Eris::PollDefault::poll();
    }
    
    assert(ball->getVelocity() == vel);
    
    /*
    
    run loop 50 times
    {
        each 0.1 of a second, do view::update prediction
        check predicted pos is close to locally predicted.
    }
    
    send stop
    
    check moving goes false, check final pos is 'exact' match, etc
 
    */
}

void testEmote(Controller& ctl)
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

    Atlas::Objects::Operation::Imaginary imag;
    
    Atlas::Objects::Entity::Anonymous em;
    em->setId("emote");
    em->setAttr("description", "I wish I was a teapot");
    
    imag->setArgs1(em);
    imag->setFrom("_vase_1");
    imag->setSerialno(getNewSerialno());
    
    SignalRecorderRef1<std::string> emote;
    
    Eris::Entity* vase = av->getView()->getEntity("_vase_1");
    vase->Emote.connect(sigc::mem_fun(emote, &SignalRecorderRef1<std::string>::fired));
    
    ctl.broadcastSightFrom("_vase_1", imag);

    while (!emote.fireCount()) {
        Eris::PollDefault::poll();
    }
    
    assert(emote.lastArg0() == "I wish I was a teapot");   
}

int runTests(Controller& ctl)
{
    try {
        testLogin();
        testBadLogin();
        testBadLogin2();
        testAccCreate();
        testLogout();
        testAccountCharacters();
        testBadTake();
        testServerInfo();
        testDuplicateCreate();
        
        testServerSocketShutdown(ctl);
    
        testCharActivate(ctl);
        testCharCreate();
        testBadCreate();
        
        testAppearance(ctl);
        testSet(ctl);
        testTalk(ctl);
        testSeeMove(ctl);
        testSeeMoveWithUpdates(ctl);
        testLocationChange(ctl);
        testSightAction(ctl);
        testSightDelete(ctl);
        testSoundAction(ctl);
        testMovement(ctl); 
        
        testCharacterInitialVis(ctl);
        testLookQueue(ctl);
        testEmote(ctl);
        testEntityCreation(ctl);
        testUnseen(ctl);
        
        testWield(ctl);
        testDeleteWielded(ctl);
        testHear(ctl);
        testTasks(ctl);
        

        testSimpleCalendar(ctl);
        testLogoutRequest(ctl);
        testTransferRequest(ctl);
        testTransferRequestWithInvalidOp(ctl);
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
}

int forkAndRunBoth(int argc, char* argv[])
{
    pid_t childPid = fork();
    if (childPid == 0)
    {
        sleep(1);
        Controller ctl("/tmp/eris-test");
        
        Eris::setLogLevel(LOG_DEBUG);
        Eris::Logged.connect(sigc::ptr_fun(&erisLog));
    
        return runTests(ctl);
    } else {
        Eris::setLogLevel(LOG_DEBUG);
        Eris::Logged.connect(sigc::ptr_fun(&erisLog));
        
        StubServer stub(7450);
        return stub.run(childPid);
    }
}

int runServer(int argc, char* argv[])
{    
    Eris::setLogLevel(LOG_DEBUG);
    Eris::Logged.connect(sigc::ptr_fun(&erisLog));
    
    StubServer stub(7450);
    return stub.run(0);

}

int runClient(int argc, char* argv[])
{    
    Controller ctl("/tmp/eris-test");
        
    Eris::setLogLevel(LOG_DEBUG);
    Eris::Logged.connect(sigc::ptr_fun(&erisLog));

    return runTests(ctl);
}

int main(int argc, char **argv)
{
    if (argc > 1) {
        if (!strcmp(argv[1], "--server")) return runServer(argc, argv);
        if (!strcmp(argv[1], "--client")) return runClient(argc, argv);
        
        if (strcmp(argv[1], "--both")) {
            cerr << "unrecognized test mode: " << argv[1] << endl;
            return EXIT_FAILURE;
        }
    }

    return forkAndRunBoth(argc, argv);
}
