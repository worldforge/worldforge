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
    IGRouter(Avatar* av);
    virtual ~IGRouter();

protected:
    virtual RouterResult handleOperation(const Atlas::Objects::Operation::RootOperation& op);

private:
    RouterResult handleSightOp(const Atlas::Objects::Operation::RootOperation& op);

    Avatar* m_avatar;
    View* m_view;
    TypeInfo* m_actionType;
};

} // of namespace Eris

#endif // of ERIS_IG_ROUTER_H
