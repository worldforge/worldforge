#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/EntityRouter.h>
#include <Eris/LogStream.h>
#include <Eris/ViewEntity.h>
#include <Eris/TypeService.h>
#include <Eris/Avatar.h>
#include <Eris/TypeInfo.h>
#include <Eris/View.h>
#include <Eris/Connection.h>
#include <Eris/TypeBoundRedispatch.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris {

EntityRouter::EntityRouter(ViewEntity* ent) :
    m_entity(ent)
{
    assert(ent);
}

EntityRouter::~EntityRouter()
{

}

Router::RouterResult EntityRouter::handleOperation(const RootOperation& op)
{
    assert(op->getFrom() == m_entity->getId());    
    const std::vector<Root>& args = op->getArgs();
    
    // note it's important we match exactly on sight here, and not deried ops
    // like appearance and disappearance
    if (op->getClassNo() == SIGHT_NO) {
        assert(!args.empty());
        RootOperation sop = smart_dynamic_cast<RootOperation>(args.front());
        if (sop.isValid()) return handleSightOp(sop);
    }
    
    if (op->getClassNo() == SOUND_NO) {
        assert(!args.empty());
        if (args.front()->getClassNo() == TALK_NO)
        {
            RootOperation talk = smart_dynamic_cast<RootOperation>(args.front());
            m_entity->onTalk(talk);
            return HANDLED;
        } 
        
        TypeInfo* ty = typeService()->getTypeForAtlas(args.front());
        if (!ty->isBound()) {
            new TypeBoundRedispatch(m_entity->getView()->getAvatar()->getConnection(), op, ty);
            return WILL_REDISPATCH;
        }
    
        if (ty->isA(typeService()->getTypeByName("action")))
        {
            // sound of action
            RootOperation act = smart_dynamic_cast<RootOperation>(args.front());
            m_entity->onSoundAction(act);
            return HANDLED;
        }
        
        warning() << "entity " << m_entity->getId() << " emitted sound with strange argument: " << op;
        // other sounds !
    }

    return IGNORED;
}

Router::RouterResult EntityRouter::handleSightOp(const RootOperation& op)
{
    const std::vector<Root>& args = op->getArgs();
    
    if (op->getClassNo() == MOVE_NO) {
        // sight of move, we handle as a specialization of set.
        assert(!args.empty());
        const Root & arg = args.front();
        
        // break out LOC, which MOVE ops are allowed to update
        if (arg->hasAttr("loc")) {
            m_entity->setLocationFromAtlas(arg->getAttr("loc").asString());
        }
        
        m_entity->setFromRoot(arg, true /* movement allowed */);
        return HANDLED;
    }
    
    if (op->instanceOf(IMAGINARY_NO)) {
        if (args.empty())
            error() << "entity " << m_entity->getId() << " sent imaginary with no args: " << op;
        else
            m_entity->onImaginary(args.front());
        return HANDLED;        
    }
    
    // explicitly do NOT handle set ops here, since they can
    // validly change multiple entities - handled by the IGRouter

    return IGNORED;
}

TypeService* EntityRouter::typeService()
{
    return m_entity->getView()->getAvatar()->getConnection()->getTypeService();
}


} // of namespace Eris
