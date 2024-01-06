#ifndef ERIS_IG_ROUTER_H
#define ERIS_IG_ROUTER_H

#include "Router.h"

namespace Eris {

// forward decls
class Avatar;

class View;

class TypeInfo;

class IGRouter : public Router {
public:
	IGRouter(Avatar& av, View& view);

	~IGRouter() override;

	RouterResult handleOperation(const Atlas::Objects::Operation::RootOperation& op) override;

private:

	Avatar& m_avatar;
	View& m_view;
};

} // of namespace Eris

#endif // of ERIS_IG_ROUTER_H
