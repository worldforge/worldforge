#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/entityRouter.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::GameEntity;

namespace Eris {

EntityRouter::EntityRouter(Entity* ent) :
    m_entity(ent)
{

}

EntityRouter::~EntityRouter()
{

}

RouterResult EntityRouter::handleOperation(const RootOperation& op)
{
    assert(op->getFrom() == m_entity->getID());
    const std::vector<Root>& args = op->getArgs();
    
    Sight sight = smart_dynamic_cast<Sight>(op);
    if (sight)
    {
        assert(!args.empty());
        RootOperation sop = smart_dynamic_cast<RootOperation>(args.front());
        if (sop)
            return handleSightOp(sop);
            
        if (args.front()->getId() == m_entity->getID())
        {
            m_entity->sight(smart_dynamic_cast<GameEntity>(args.front());
            return HANDLED;
        }
    }
    
    Sound snd = smart_dynamic_cast<Sound>(op);
    if (snd)
    {
        assert(!args.empty());
        Talk talk = smart_dynamic_cast<Talk>(args.front());
        if (talk)
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

RouterResult EntityRouter::handleSightOp(const RootOperation& op)
{
    const std::vector<Root>& args = op->getArgs();
    
    // explicitly do NOT handle set ops here, since they can
    // validly change multiple entities - handled by the IGRouter

    return IGNORED;
}

} // of namespace Eris