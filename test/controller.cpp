#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "controller.h"

#include <cassert>

#include <Atlas/Net/Stream.h>
#include <Atlas/Codec.h>
#include <Atlas/Objects/Encoder.h>

const std::string VAR_DIR = "/tmp";

using std::endl;
using std::cout;

Controller::Controller()
{
    m_stream.open(VAR_DIR + "/testeris.sock");
    while (!m_stream.isReady(100)) { cout << "waiting for stream to open" << endl; }
    
    assert(m_stream.is_open());
    
// force synchrous negotation now
    Atlas::Bridge* dummyBridge = NULL;
    Atlas::Net::StreamConnect sc("eristest_oob", m_stream, *dummyBridge);
    
    // spin (and block) while we negotiate
    do { sc.poll(); } while (sc.getState() == Atlas::Net::StreamConnect::IN_PROGRESS);
    
    assert(sc.getState() == Atlas::Net::StreamConnect::SUCCEEDED);
    
    m_codec = sc.getCodec();
    m_encode = new Atlas::Objects::ObjectsEncoder(*m_codec);
    m_codec->streamBegin();
}
