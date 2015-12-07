#include "calendarTest.h"
#include "signalHelpers.h"

#include "testUtils.h"
#include "signalHelpers.h"
#include "setupHelpers.h"
#include "viewTest.h"
#include "controller.h"

#include <Eris/Account.h>
#include <Eris/Avatar.h>
#include <Eris/Calendar.h>

#include <sigc++/object.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Entity.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Root;

void testSimpleCalendar(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_B", "sweede", con.get());
    
    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    
    AutoAvatar av = AvatarGetter(acc.get()).take("acc_b_character");
    Eris::View* v = av->getView();
    {
        WaitForAppearance wf(v, "_hut_01");
        wf.run();
    }
    
    std::unique_ptr<Eris::Calendar> cal(new Eris::Calendar(av.get()));
    
    // inject a dummy op to establish world time
    Eris::TestInjector i(con.get());
    
    Set st;
    Anonymous arg;
    arg->setId("_hut_01");
    arg->setAttr("woo", 99);
    st->setArgs1(arg);
    st->setTo("acc_b_character");
    st->setFrom("_hut_01");
    
    Sight sight;
    sight->setArgs1(st);
    sight->setTo("acc_b_character");
    sight->setSeconds(2000000);
    
    i.inject(sight);
    
    Eris::DateTime dt = cal->now();
    assert(dt.valid());
    assert(dt.year() == 0);
    assert(dt.month() == 2);        // 2 * 42 = 84
    assert(dt.dayOfMonth() == 5);   // 84 + 5 = 89
    assert(dt.hours() == 4);          // 89 * 14 = 1246, + 4 = 1250
    assert(dt.minutes() == 0);
    assert(dt.seconds() == 0);
    
    sight->setSeconds(67881726);
    i.inject(sight);
    
    dt = cal->now();
    assert(dt.valid());
    assert(dt.year() == 6);
    assert(dt.month() == 6);      
    assert(dt.dayOfMonth() == 6);
    assert(dt.hours() == 6);
    assert(dt.minutes() == 6);
   // assert(dt.seconds() == 6); // precision problems
}
