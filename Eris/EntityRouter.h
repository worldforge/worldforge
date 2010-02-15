#include <Eris/Router.h>

namespace Eris
{

class ViewEntity;
class TypeService;

class EntityRouter : public Router
{
public:
    EntityRouter(ViewEntity* ent);
    virtual ~EntityRouter();
    
protected:
    virtual RouterResult handleOperation(const Atlas::Objects::Operation::RootOperation&);
    
private:
    RouterResult handleSightOp(const Atlas::Objects::Operation::RootOperation&);
    
    TypeService* typeService();
    
    ViewEntity* m_entity;
};

}
