#ifndef ERIS_IG_ROUTER_H
#define ERIS_IG_ROUTER_H

#include <Eris/router.h>

namespace Eris {

// forward decls
class Avatar;
class View;

class IGRouter : public Router
{
public:
    IGRouter(Avatar* av)
    virtual ~IGRouter();
        
protected:
    virtual RouterResult handleOperation(const Atlas::Objects::Operation::RootOperation& op);

private:
    RouterResult IGRouter::handleSightOp(const Atlas::Objects::Operation::RootOperation& op);
    
    Avatar* m_avatar;
    View* m_view;
};

} // of namespace Eris

#endif // of ERIS_IG_ROUTER_H