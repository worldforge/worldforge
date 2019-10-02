#include "Router.h"

namespace Eris
{

class ViewEntity;
class TypeService;

class EntityRouter : public Router
{
public:
	explicit EntityRouter(ViewEntity* ent);

	~EntityRouter() override;
    
protected:
	RouterResult handleOperation(const Atlas::Objects::Operation::RootOperation&) override;
    
private:
    RouterResult handleSightOp(const Atlas::Objects::Operation::RootOperation&);
    
    TypeService* typeService();
    
    ViewEntity* m_entity;
};

}
