#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/igRouter.h>
#include <Eris/Avatar.h>
#include <Eris/Connection.h>
#include <Eris/view.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::GameEntity;

namespace Eris {

IGRouter::IGRouter(Avatar* av) :
    m_avatar(av),
    m_view(av->getView())
{
    m_avatar->getConnection()->registerRouterForTo(this, m_avatar->getId());
}

IGRouter::~IGRouter()
{
    m_avatar->getConnection()->unregisterRouterForTo(this, m_avatar->getId());
}

#pragma mark -

RouterResult IGRouter::handleOperation(const RootOperation& op)
{
    const std::vector<Root>& args = op->getArgs();
    
    Sight sight = smart_dynamic_cast<Sight>(op);
    if (sight)
    {
        assert(!args.empty());
        RootOperation sop = smart_dynamic_cast<RootOperation>(args.front());
        if (sop)
            return handleSightOp(sop);
            
        // initial sight of entities
        GameEntity gent = smart_dynamic_cast<GameEntity>(args.front())
        if (gent)
        {
            m_view->initialSight(gent);
            return HANDLED;
        }
    }
    
    Apperance appear = smart_dynamic_cast<Appearance>(op);
    if (appear)
    {
        for (unsigned int A=0; A < args.size(); ++A)
        {
            float stamp = 0;
            if (args[A]->hasAttr("stamp"))
                stamp = args[A]->getAttr("stamp").toFloat();
                
            m_view->appear(args[A]->getId(), stamp);
        }
        
        return HANDLED;
    }
    
    Disappearance disappear = smart_dynamic_cast<Disappearance>(op)
    {
        for (unsigned int A=0; A < args.size(); ++A)
            m_view->disappear(args[A]->getId());
            
        return HANDLED;
    }
    
    return IGNORED;
}

RouterResult IGRouter::handleSightOp(const RootOperation& op)
{
    const std::vector<Root>& args = op->getArgs();

    Create cr = smart_dynamic_cast<Create>(op);
    if (cr)
    {
        assert(!args.empty());
        GameEntity gent = smart_dynamic_cast<GameEntity>(args.front())
        if (gent)
        {
            m_view->create(gent);
            return HANDLED;
        }
    }
    
    Delete del = smart_dynamic_cast<Delete>(op)
    if (del)
    {
        assert(!args.empty());
        m_view->deleteEntity(args.front()->getId());
        return HANDLED;
    }
    
    // becuase a SET op can potentially (legally) update multiple entities,
    // we decode it here, not in the entity router
    Set setOp = smart_dynamic_cast<Set>(op);
    if (setOp)
    {
        for (unsigned int A=0; A < args.size(); ++A)
        {
            Entity* ent = m_view-<getExistingEntity(args[A]->getId());
            if (!ent)
            {
                if (m_view->isPending(args[A]->getId())
                    warning() << "got SET with updates for pending entity " << args[A]->getId();
                continue; // we don't have it, ignore
            }
            
            ent->setFromRoot(args[A]);
        }
        return HANDLED;
    }
    
    return IGNORED;
}

} // of namespace Eris