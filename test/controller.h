#ifndef TEST_CONTROLLER_H
#define TEST_CONTROLLER_H

#include <skstream/skstream_unix.h>
#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/Decoder.h>
#include <wfmath/point.h>

namespace Atlas { class Codec; }

namespace Eris { class Avatar; }

class Controller : public Atlas::Objects::ObjectsDecoder
{
public:
    Controller(int fd);
    
    void setEntityVisibleToAvatar(const std::string& eid, Eris::Avatar* av);
    void setEntityVisibleToAvatar(const std::string& eid, const std::string& charId);

    void setEntityInvisibleToAvatar(const std::string& eid, Eris::Avatar* av);
    void setEntityInvisibleToAvatar(const std::string& eid, const std::string& charId);   
    
    void moveLocation(const std::string& eid, const std::string& loc, const WFMath::Point<3>& pos);
    void movePos(const std::string& eid, const WFMath::Point<3>& pos);

    void create(const Atlas::Objects::Entity::GameEntity& ent);
    
    /** generalised attribute modification interface */
    void setAttr(const std::string& eid, const std::string& attr, const Atlas::Message::Element v); 
protected:
    virtual void objectArrived(const Atlas::Objects::Root& obj);
    
private:
    void send(const Atlas::Objects::Root &obj);
    
    tcp_socket_stream m_stream;

    Atlas::Objects::ObjectsEncoder* m_encode;
    Atlas::Codec* m_codec;
};

#endif // of TEST_CONTROLLER_H
