

#include <cassert>
#include <skstream/skstream_unix.h>

#include <Atlas/Net/Stream.h>
#include <Atlas/Codec.h>
#include <Atlas/Objects/Encoder.h>

const std::string VAR_DIR = "/tmp";

class Controller
{
public:
    Controller()
    {
        m_stream.open(VAR_DIR + "/testeris.sock");
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

private:
    unix_socket_stream m_stream;

    Atlas::Objects::ObjectsEncoder* m_encode;
    Atlas::Codec* m_codec;

};