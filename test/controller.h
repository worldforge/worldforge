#ifndef TEST_CONTROLLER_H
#define TEST_CONTROLLER_H

#include <skstream/skstream_unix.h>
#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/Decoder.h>

namespace Atlas { class Codec; }

namespace Eris { class Avatar; }

class Controller : public Atlas::Objects::ObjectsDecoder
{
public:
    Controller(int fd);
    
    void setEntityVisibleToAvatar(const std::string& eid, Eris::Avatar* av);
    void setEntityVisibleToAvatar(const std::string& eid, const std::string& charId);
    
protected:
    virtual void objectArrived(const Atlas::Objects::Root& obj);
    
private:
    void send(const Atlas::Objects::Root &obj);
    
    tcp_socket_stream m_stream;

    Atlas::Objects::ObjectsEncoder* m_encode;
    Atlas::Codec* m_codec;
};

#endif // of TEST_CONTROLLER_H
