#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "controller.h"

#include <cassert>

#include <Atlas/Net/Stream.h>
#include <Atlas/Codec.h>
#include <Atlas/Objects/Encoder.h>

#include <Eris/Exceptions.h>

using std::endl;
using std::cout;

Controller::Controller(int fd) :
    m_stream(fd)
{
    assert(m_stream.is_open());
    
// force synchrous negotation now
    Atlas::Bridge* br = this;
    Atlas::Net::StreamConnect sc("eristest_oob", m_stream, *br);
    
    // spin (and block) while we negotiate
    do { sc.poll(); } while (sc.getState() == Atlas::Net::StreamConnect::IN_PROGRESS);
    
    if (sc.getState() == Atlas::Net::StreamConnect::FAILED)
        throw Eris::InvalidOperation("controller negotation failed");
            
    assert(sc.getState() == Atlas::Net::StreamConnect::SUCCEEDED);
    
    m_codec = sc.getCodec();
    m_encode = new Atlas::Objects::ObjectsEncoder(*m_codec);
    m_codec->streamBegin();
}

void Controller::objectArrived(const Atlas::Objects::Root&)
{
    cout << "controller recieved op!" << endl;
}
