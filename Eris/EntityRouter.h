#include <Eris/Router.h>

namespace Eris
{

class Entity;
class TypeService;

class EntityRouter : public Router
{
public:
    EntityRouter(Entity* ent);
    virtual ~EntityRouter();
    
protected:
    virtual RouterResult handleOperation(const Atlas::Objects::Operation::RootOperation&);
    
private:
    RouterResult handleSightOp(const Atlas::Objects::Operation::RootOperation&);
    
    TypeService* typeService();
    
    Entity* m_entity;
};

}
