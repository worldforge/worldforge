#ifndef TEST_CONTROLLER_H
#define TEST_CONTROLLER_H

#include <skstream/skstream_unix.h>
#include <Atlas/Objects/ObjectsFwd.h>


namespace Atlas { class Codec; }

class Controller
{
public:
    Controller();
private:
    unix_socket_stream m_stream;

    Atlas::Objects::ObjectsEncoder* m_encode;
    Atlas::Codec* m_codec;
};

#endif // of TEST_CONTROLLER_H