#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/EntityRouter.h>
#include <Eris/LogStream.h>
#include <Eris/Entity.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::GameEntity;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris {

EntityRouter::EntityRouter(Entity* ent) :
    m_entity(ent)
{

}

EntityRouter::~EntityRouter()
{

}

Router::RouterResult EntityRouter::handleOperation(const RootOperation& op)
{
    assert(op->getFrom() == m_entity->getId());
    const std::vector<Root>& args = op->getArgs();
    
    Sight sight = smart_dynamic_cast<Sight>(op);
    if (sight.isValid())
    {
        assert(!args.empty());
        RootOperation sop = smart_dynamic_cast<RootOperation>(args.front());
        if (sop.isValid())
            return handleSightOp(sop);
            
        if (args.front()->getId() == m_entity->getId())
        {
            m_entity->sight(smart_dynamic_cast<GameEntity>(args.front()));
            return HANDLED;
        }
    }
    
    Sound snd = smart_dynamic_cast<Sound>(op);
    if (snd.isValid())
    {
        assert(!args.empty());
        Talk talk = smart_dynamic_cast<Talk>(args.front());
        if (talk.isValid())
        {
            const std::vector<Root>& args = talk->getArgs();
            if (args.empty())
            {
                error() << "entity " << m_entity->getId() << " got sound(talk) with no args";
                return IGNORED;
            }
            
            m_entity->talk(args.front());
            return HANDLED;
        }
        
        // other sounds !
    }

    return IGNORED;
}

Router::RouterResult EntityRouter::handleSightOp(const RootOperation& op)
{
    const std::vector<Root>& args = op->getArgs();
    
    Move mv = smart_dynamic_cast<Move>(op);
    if (mv.isValid())
    {
        // sight of move, we handle as a specialization of set.
        assert(!args.empty());
        Root arg = args.front();
        
        // break out LOC, which MOVE ops are allowed to update
        if (arg->hasAttr("loc"))
            m_entity->setLocationFromAtlas(arg->getAttr("loc").asString());
        
        m_entity->setFromRoot(arg);
        return HANDLED;
    }
    
    // explicitly do NOT handle set ops here, since they can
    // validly change multiple entities - handled by the IGRouter

    return IGNORED;
}

} // of namespace Eris