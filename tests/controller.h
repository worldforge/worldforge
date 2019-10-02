#ifndef TEST_CONTROLLER_H
#define TEST_CONTROLLER_H

#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/Decoder.h>
#include <wfmath/point.h>

namespace Atlas { class Codec; }

namespace Eris { class Avatar; }

class Controller : public Atlas::Objects::ObjectsDecoder
{
public:
    Controller(const char* pipe);
    
    void setEntityVisibleToAvatar(const std::string& eid, Eris::Avatar* av);
    void setEntityVisibleToAvatar(const std::string& eid, const std::string& charId);

    void setEntityInvisibleToAvatar(const std::string& eid, Eris::Avatar* av);
    void setEntityInvisibleToAvatar(const std::string& eid, const std::string& charId);   
    
    void moveLocation(const std::string& eid, const std::string& loc, const WFMath::Point<3>& pos);
    void movePos(const std::string& eid, const WFMath::Point<3>& pos);
    void moveVelocity(const std::string& eid, const WFMath::Vector<3>& vel);
    void move(const std::string& eid, const Atlas::Objects::Operation::RootOperation& op);
    
    void create(const Atlas::Objects::Entity::RootEntity& ent);
    void deleteEntity(const std::string& eid);
    
    /** generalised attribute modification interface */
    void setAttr(const std::string& eid, const std::string& attr, const Atlas::Message::Element v); 
    
    void broadcastSoundFrom(const std::string& e, const Atlas::Objects::Operation::RootOperation& op);
    void broadcastSightFrom(const std::string& e, const Atlas::Objects::Operation::RootOperation& op);

    void command(const std::string& cid, const std::string& acc);
protected:
    virtual void objectArrived(const Atlas::Objects::Root& obj);
    
private:
    void send(const Atlas::Objects::Root &obj);
    
    unix_socket_stream m_stream;

    Atlas::Objects::ObjectsEncoder* m_encode;
    Atlas::Codec* m_codec;
};

#endif // of TEST_CONTROLLER_H
