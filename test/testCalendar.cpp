#include <Eris/Calendar.h>

void testCalendar(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount acc = stdLogin("account_B", "sweede", con.get());
    
    AutoAvatar av = AvatarGetter(acc.get()).take("acc_b_character");
    std::unique_ptr<Eris::Calendar> cal(new Eris::Calendar(av.get())); 
    
    Eris::DataTime dt = cal->now();
    assert(dt.valid() == false);
    
    ctl.setEntityVisibleToAvatar("_field_01", av.get());
    WaitForAppearance af2(av, "_field_01");
    af2.run();
    
    ctl.command("set-world-time", 6000.0);
    // spin eris
    
    dt = cal->now();
    assert(dt.valid());
    // inspect
}

