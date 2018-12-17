#ifndef ERIS_IG_ROUTER_H
#define ERIS_IG_ROUTER_H

#include "Router.h"

namespace Eris {

// forward decls
class Avatar;
class View;
class TypeInfo;

class IGRouter : public Router
{
public:
	explicit IGRouter(Avatar* av);

	~IGRouter() override;

protected:
	RouterResult handleOperation(const Atlas::Objects::Operation::RootOperation& op) override;

private:
    RouterResult handleSightOp(const Atlas::Objects::Operation::RootOperation& op);

    Avatar* m_avatar;
    View* m_view;
    TypeInfo* m_actionType;
};

} // of namespace Eris

#endif // of ERIS_IG_ROUTER_H
