#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/IGRouter.h>
#include <Eris/Avatar.h>
#include <Eris/Connection.h>
#include <Eris/View.h>
#include <Eris/Entity.h>
#include <Eris/LogStream.h>
#include <Eris/TypeService.h>
#include <Eris/TypeInfo.h>
#include <Eris/TypeBoundRedispatch.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::GameEntity;
using Atlas::Objects::smart_dynamic_cast;

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

Router::RouterResult IGRouter::handleOperation(const RootOperation& op)
{
    if (!op->isDefaultSeconds())
    {
        // grab out world time
        m_avatar->updateWorldTime(op->getSeconds());
    }
    
    const std::vector<Root>& args = op->getArgs();
    
    Sight sight = smart_dynamic_cast<Sight>(op);
    if (sight.isValid())
    {
        assert(!args.empty());
        RootOperation sop = smart_dynamic_cast<RootOperation>(args.front());
        if (sop.isValid())
            return handleSightOp(sop);
            
        // initial sight of entities
        GameEntity gent = smart_dynamic_cast<GameEntity>(args.front());
        if (gent.isValid())
        {
            TypeInfo* type = m_avatar->getConnection()->getTypeService()->getTypeForAtlas(gent);
            if (!type->isBound()) {
                TypeInfoSet unbound;
                unbound.insert(type);
                
                new TypeBoundRedispatch(m_avatar->getConnection(), op, unbound);
                return WILL_REDISPATCH;
            }
    
            m_view->sight(gent);
            return HANDLED;
        }
    }
    
    Appearance appear = smart_dynamic_cast<Appearance>(op);
    if (appear.isValid())
    {
        debug() << "recieved appearance";
        for (unsigned int A=0; A < args.size(); ++A)
        {
            float stamp = -1;
            if (args[A]->hasAttr("stamp"))
                stamp = args[A]->getAttr("stamp").asFloat();
                
            m_view->appear(args[A]->getId(), stamp);
        }
        
        return HANDLED;
    }
    
    Disappearance disappear = smart_dynamic_cast<Disappearance>(op);
    if (disappear.isValid())
    {
        for (unsigned int A=0; A < args.size(); ++A)
            m_view->disappear(args[A]->getId());
            
        return HANDLED;
    }
    
    if (op->getClassNo() == ERROR_NO) {
        Error err = smart_dynamic_cast<Error>(op);
        if (m_view->maybeHandleError(err)) 
            return HANDLED;
    }
    
    return IGNORED;
}

Router::RouterResult IGRouter::handleSightOp(const RootOperation& op)
{
    const std::vector<Root>& args = op->getArgs();

    Create cr = smart_dynamic_cast<Create>(op);
    if (cr.isValid())
    {
        assert(!args.empty());
        GameEntity gent = smart_dynamic_cast<GameEntity>(args.front());
        if (gent.isValid())
        {
            m_view->create(gent);
            return HANDLED;
        }
    }
    
    Delete del = smart_dynamic_cast<Delete>(op);
    if (del.isValid())
    {
        assert(!args.empty());
        m_view->deleteEntity(args.front()->getId());
        return HANDLED;
    }
    
    // becuase a SET op can potentially (legally) update multiple entities,
    // we decode it here, not in the entity router
    Set setOp = smart_dynamic_cast<Set>(op);
    if (setOp.isValid())
    {
        for (unsigned int A=0; A < args.size(); ++A) {
            Entity* ent = m_view->getEntity(args[A]->getId());
            if (!ent)
            {
                if (m_view->isPending(args[A]->getId()))
                    warning() << "got SET with updates for pending entity " << args[A]->getId();
                else
                    error() << " got SET for completely unknown entity " << args[A]->getId();
                continue; // we don't have it, ignore
            }
            
            debug() << "entity " << ent->getId() << " got set: " << setOp;
            ent->setFromRoot(args[A]);
        }
        return HANDLED;
    }
    
    // we have to handle generic 'actions' late, to avoid trapping interesting
    // such as create or divide
    Action act = smart_dynamic_cast<Action>(op);
    if (act.isValid()) {
        if (args.empty()) {
            error() << "entity " << op->getFrom() << " sent action with no args: " << op;
            return IGNORED;
        }
        
        Entity* ent = m_view->getEntity(op->getFrom());
        if (ent)
            ent->action(args.front());
        else
            warning() << "ignoring action for pending entity " << op->getFrom();
        
        return HANDLED;
    }

    
    return IGNORED;
}

} // of namespace Eris
