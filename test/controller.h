#ifndef TEST_CONTROLLER_H
#define TEST_CONTROLLER_H

#include <skstream/skstream_unix.h>
#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/Decoder.h>

namespace Atlas { class Codec; }

class Controller : public Atlas::Objects::ObjectsDecoder
{
public:
    Controller(int fd);
    
protected:
    virtual void objectArrived(const Atlas::Objects::Root& obj);
    
private:
    tcp_socket_stream m_stream;

    Atlas::Objects::ObjectsEncoder* m_encode;
    Atlas::Codec* m_codec;
};

#endif // of TEST_CONTROLLER_H