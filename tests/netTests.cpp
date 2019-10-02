#include "stubServer.h"
#include "testUtils.h"
#include "controller.h"
#include "setupHelpers.h"
#include "signalHelpers.h"

#include <Eris/PollDefault.h>

void testServerSocketShutdown(Controller& ctl)
{
    AutoConnection con = stdConnect();
    AutoAccount player = stdLogin("account_B", "sweede", con.get());

    ctl.setEntityVisibleToAvatar("_hut_01", "acc_b_character");
    AutoAvatar av = AvatarGetter(player.get()).take("acc_b_character");
 
    SignalRecorderRef1<std::string> conErr;
    con->Failure.connect(sigc::mem_fun(conErr, &SignalRecorderRef1<std::string>::fired));
    
    ctl.command("socket-shutdown", "_24_account_B");
    
    std::string d;
    while (!conErr.fireCount()) Eris::PollDefault::poll();
    
    
}
