#include "Router.h"

namespace Eris
{

class Entity;
class View;

class EntityRouter : public Router
{
public:
	explicit EntityRouter(Entity& ent, View& view);

	~EntityRouter() override;
    
protected:
	RouterResult handleOperation(const Atlas::Objects::Operation::RootOperation&) override;
    
private:
    RouterResult handleSightOp(const Atlas::Objects::Operation::RootOperation&);

    Entity& m_entity;
    View& m_view;
};

}
