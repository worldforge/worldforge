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
    virtual RouterResult handleOperation();
    
private:
    Entity* m_entity;
};

}