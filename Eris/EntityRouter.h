#include <Eris/router.h>

namespace Eris
{

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