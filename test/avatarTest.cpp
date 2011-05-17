#include "avatarTest.h"
#include "signalHelpers.h"

#include "testUtils.h"
#include "signalHelpers.h"
#include "setupHelpers.h"
#include "viewTest.h"
#include "controller.h"

#include <Eris/Account.h>
#include <Eris/Avatar.h>
#include <Eris/Entity.h>
#include <Eris/Operations.h>
#include <Eris/View.h>
#include <Eris/TypeInfo.h>
#include <Eris/PollDefault.h>

#include <sigc++/functors/mem_fun.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Entity.h>

#include <wfmath/timestamp.h>

using WFMath::TimeStamp;
using WFMath::TimeDiff;

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Root;

void testWield(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_B", "sweede", con.get());
    
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_hammer_1", "acc_b_character");
    
    AutoAvatar av = AvatarGetter(acc.get()).take("acc_b_character");
    Eris::View* v = av->getView();
    {
        WaitForAppearance wf(v, "_hammer_1");
        wf.run();
    }
    
    Eris::EntityRef hammerRef(v->getEntity("_hammer_1"));
    assert(hammerRef);
    
    av->wield(hammerRef.get());
    
    TimeStamp end = TimeStamp::now() + TimeDiff(5 * 1000);
    while (!av->getWielded() && (TimeStamp::now() < end)) {
        Eris::PollDefault::poll();
    }
    
    assert(av->getWielded()->getId() == "_hammer_1");
    const Eris::TypeInfoArray& ops = av->getWielded()->getUseOperations();
    
    assert(ops[0]->getName() == "strike");
    assert(ops[1]->getName() == "tap");
}

void testDeleteWielded(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_B", "sweede", con.get());
    
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_hammer_1", "acc_b_character");
    
    AutoAvatar av = AvatarGetter(acc.get()).take("acc_b_character");
    Eris::View* v = av->getView();
    {
        WaitForAppearance wf(v, "_hammer_1");
        wf.run();
    }
    
    Eris::EntityRef hammerRef(v->getEntity("_hammer_1"));
    assert(hammerRef);    
    av->wield(hammerRef.get());
    
    TimeStamp end = TimeStamp::now() + TimeDiff(5 * 1000);
    while (!av->getWielded() && (TimeStamp::now() < end)) {
        Eris::PollDefault::poll();
    }

    Eris::TestInjector i(con.get());
    SignalCounter0 hammerDeleted, hammerRefChanged;
    
    hammerRef->BeingDeleted.connect(sigc::mem_fun(hammerDeleted, &SignalCounter0::fired));
    hammerRef.Changed.connect(sigc::mem_fun(hammerRefChanged, &SignalCounter0::fired));
    {
        Delete del;
        Anonymous arg;
        arg->setId("_hammer_1"); 
        del->setArgs1(arg);
        
        Sight st;
        st->setArgs1(del);
        st->setTo("acc_b_character");
        
        i.inject(st);
    }

    assert(hammerDeleted.fireCount());
    assert(!hammerRef);
    assert(hammerRefChanged.fireCount());
    assert(!av->getWielded());
}

class Hearer : public sigc::trackable
{
public:
    Hearer() :
        m_count(0),
        m_source(NULL)
    {;}
    
    void fired(Eris::Entity* ent, const RootOperation& op)
    {
        ++m_count;
        m_op = op;
        m_source = ent;
    }
    
    int fireCount() const
    { return m_count; }
    
    const RootOperation argOp() const
    {
        return m_op;
    }
    
    Eris::Entity* source() const
    {
        return m_source;
    }
    
    void reset()
    {
        m_count = 0;
    }
private:
    int m_count;
    RootOperation m_op;
    Eris::Entity* m_source;
};


void testHear(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_B", "sweede", con.get());
    
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_table_1", "acc_b_character");
    ctl.setEntityVisibleToAvatar("_vase_1", "acc_b_character");
    
    AutoAvatar av = AvatarGetter(acc.get()).take("acc_b_character");
    Eris::View* v = av->getView();
    {
        WaitForAppearance wf(v, "_vase_1");
        wf.run();
    }
    
    Eris::TestInjector i(con.get());
    Hearer heard;
    
    av->Hear.connect(sigc::mem_fun(heard, &Hearer::fired));

    {
        Talk talk;
        Atlas::Objects::Entity::Anonymous arg;
        arg->setAttr("what", "shitcock!");
        talk->setArgs1(arg);
        talk->setFrom("_vase_1");
        
        Sound snd;
        snd->setArgs1(talk);
        snd->setTo("acc_b_character");
        snd->setFrom("_vase_1");
        
        i.inject(snd);
    }

    assert(heard.fireCount());
    RootOperation heardOp = heard.argOp();
    assert(heardOp->getClassNo() == TALK_NO);
    const std::vector<Root>& args = heardOp->getArgs();
    
    assert(args.front()->hasAttr("what"));
    assert(args.front()->getAttr("what") == "shitcock!");
    
    assert(heard.source()->getId() == "_vase_1");
}

void testLogoutRequest(Controller& ctl)
{

    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_B", "sweede", con.get());
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");

    AutoAvatar av = AvatarGetter(acc.get()).take("acc_b_character");
    Eris::TestInjector i(con.get());

    Atlas::Objects::Operation::Logout logout;
    logout->setTo(av->getId());


    SignalCounter1<Eris::Avatar*> avatarDeactivated;

    acc->AvatarDeactivated.connect(sigc::mem_fun(avatarDeactivated, &SignalCounter1<Eris::Avatar*>::fired));

    i.inject(logout);
    TimeStamp end = TimeStamp::now() + TimeDiff(5 * 1000);
    while (avatarDeactivated.fireCount() == 0 && (TimeStamp::now() < end)) {
        Eris::PollDefault::poll();
    }

    assert(avatarDeactivated.fireCount() != 0);
    assert(acc->isLoggedIn());
    assert(acc->getActiveCharacters().size() == 0);

    //The avatar has been destroyed when logging out, so we shouldn't let the auto_ptr do that too.
    av.release();

}

void testTransferRequest(Controller& ctl)
{

    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_C", "turnip", con.get());
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_c_character");

    AutoAvatar av = AvatarGetter(acc.get()).take("acc_c_character");
    Eris::TestInjector i(con.get());


    std::vector<Root> logout_args;

    Anonymous op_arg;
    op_arg->setId(av->getId());
    logout_args.push_back(op_arg);

    Atlas::Objects::Operation::Logout logout;
    Anonymous transferArg;
    transferArg->setAttr("teleport_host", "teleport_host");
    transferArg->setAttr("teleport_port", 6768);
    transferArg->setAttr("possess_key", "possess_key");
    transferArg->setAttr("possess_entity_id", "possess_entity_id");
    logout_args.push_back(transferArg);

    logout->setTo(av->getId());
    logout->setArgs(logout_args);


    SignalCounter1<const Eris::TransferInfo&> transferRequested;

    av->TransferRequested.connect(sigc::mem_fun(transferRequested, &SignalCounter1<const Eris::TransferInfo&>::fired));

    i.inject(logout);

    TimeStamp end = TimeStamp::now() + TimeDiff(5 * 1000);
    while (transferRequested.fireCount() == 0 && (TimeStamp::now() < end)) {
        Eris::PollDefault::poll();
    }

    assert(transferRequested.fireCount() == 1);
    assert(acc->isLoggedIn());
    assert(acc->getActiveCharacters().size() == 0);

    //The avatar has been destroyed when logging out, so we shouldn't let the auto_ptr do that too.
    av.release();

}

void testTransferRequestWithInvalidOp(Controller& ctl)
{

    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_C", "turnip", con.get());
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_c_character");

    AutoAvatar av = AvatarGetter(acc.get()).take("acc_c_character");
    Eris::TestInjector i(con.get());


    std::vector<Root> logout_args;

    Anonymous op_arg;
    op_arg->setId(av->getId());
    logout_args.push_back(op_arg);

    Atlas::Objects::Operation::Logout logout;
    Anonymous transferArg;
    transferArg->setAttr("teleport_host", "teleport_host");
//    transferArg->setAttr("teleport_port", 6768);
    transferArg->setAttr("possess_key", "possess_key");
    transferArg->setAttr("possess_entity_id", "possess_entity_id");
    logout_args.push_back(transferArg);

    logout->setTo(av->getId());
    logout->setArgs(logout_args);


    SignalCounter1<const Eris::TransferInfo&> transferRequested;
    SignalCounter1<Eris::Avatar*> avatarDeactivated;

    av->TransferRequested.connect(sigc::mem_fun(transferRequested, &SignalCounter1<const Eris::TransferInfo&>::fired));
    acc->AvatarDeactivated.connect(sigc::mem_fun(avatarDeactivated, &SignalCounter1<Eris::Avatar*>::fired));

    i.inject(logout);

    TimeStamp end = TimeStamp::now() + TimeDiff(5 * 1000);
    while (avatarDeactivated.fireCount() == 0 && (TimeStamp::now() < end)) {
        Eris::PollDefault::poll();
    }

    assert(avatarDeactivated.fireCount() == 1);
    assert(transferRequested.fireCount() == 0);
    assert(acc->isLoggedIn());
    assert(acc->getActiveCharacters().size() == 0);

    //The avatar has been destroyed when logging out, so we shouldn't let the auto_ptr do that too.
    av.release();

}
