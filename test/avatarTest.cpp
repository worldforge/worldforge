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

#include <sigc++/object_slot.h>
#include <sigc++/object.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Entity.h>

#include <wfmath/timestamp.h>

using WFMath::TimeStamp;
using WFMath::TimeDiff;

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
    assert(av->getUseOperationsForWielded().empty());
    
    av->wield(hammerRef.get());
    
    TimeStamp end = TimeStamp::now() + TimeDiff(5 * 1000);
    while (!av->getWielded() && (TimeStamp::now() < end)) {
        Eris::PollDefault::poll();
    }
    
    assert(av->getWielded()->getId() == "_hammer_1");
    const Eris::TypeInfoArray& ops = av->getUseOperationsForWielded();
    
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
    
    hammerRef->BeingDeleted.connect(SigC::slot(hammerDeleted, &SignalCounter0::fired));
    hammerRef.Changed.connect(SigC::slot(hammerRefChanged, &SignalCounter0::fired));
    {
        Atlas::Objects::Operation::Delete del;
        Atlas::Objects::Entity::Anonymous arg;
        arg->setId("_hammer_1"); 
        del->setArgs1(arg);
        
        Atlas::Objects::Operation::Sight st;
        st->setArgs1(del);
        st->setTo("acc_b_character");
        
        i.inject(st);
    }

    assert(hammerDeleted.fireCount());
    assert(!hammerRef);
    assert(hammerRefChanged.fireCount());
    assert(!av->getWielded());
}
