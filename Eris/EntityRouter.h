#include <Eris/router.h>

namespace Eris
{

class Entity;

class EntityRouter : public Router
{
public:
    EntityRouter(Entity* ent);
    virtual ~EntityRouter();
    
protected:
    virtual RouterResult handleOperation(const Atlas::Objects::Operation::RootOperation&);
    
private:
    RouterResult handleSightOp(const Atlas::Objects::Operation::RootOperation&);
    
    Entity* m_entity;
};

}
