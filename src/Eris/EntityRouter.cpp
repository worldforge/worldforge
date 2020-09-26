#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "EntityRouter.h"
#include "LogStream.h"
#include "ViewEntity.h"
#include "TypeService.h"
#include "Avatar.h"
#include "TypeInfo.h"
#include "View.h"
#include "Connection.h"
#include "TypeBoundRedispatch.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris {

EntityRouter::EntityRouter(Entity& ent, View& view) :
    m_entity(ent),
    m_view(view)
{
}

EntityRouter::~EntityRouter() = default;

Router::RouterResult EntityRouter::handleOperation(const RootOperation& op)
{
    assert(op->getFrom() == m_entity.getId());    
    const std::vector<Root>& args = op->getArgs();
    
    // note it's important we match exactly on sight here, and not derived ops
    // like appearance and disappearance
    if (op->getClassNo() == SIGHT_NO) {
    	for (const auto& arg : args) {
			RootOperation sop = smart_dynamic_cast<RootOperation>(arg);
			if (sop.isValid()) {
				return handleSightOp(sop);
			}
    	}
    }
    
    if (op->getClassNo() == SOUND_NO) {
		for (const auto& arg : args) {
			if (arg->getClassNo() == TALK_NO)
			{
				RootOperation talk = smart_dynamic_cast<RootOperation>(arg);
				m_entity.onTalk(talk);
			} else {
				if (!arg->isDefaultParent()) {
					auto ty = m_view.getTypeService().getTypeForAtlas(arg);
					if (!ty->isBound()) {
						new TypeBoundRedispatch(m_view.getConnection(), op, ty);
					} else if (ty->isA(m_view.getTypeService().getTypeByName("action"))) {
						// sound of action
						RootOperation act = smart_dynamic_cast<RootOperation>(arg);
						m_entity.onSoundAction(act, *ty);
					} else {
						warning() << "entity " << m_entity.getId() << " emitted sound with strange argument: " << op;
					}
				} else {
					warning() << "entity " << m_entity.getId() << " emitted sound with strange argument: " << op;
				}
			}
		}

		return HANDLED;
        // other sounds !
    }

    return IGNORED;
}

Router::RouterResult EntityRouter::handleSightOp(const RootOperation& op)
{
    const std::vector<Root>& args = op->getArgs();
    
//    if (op->getClassNo() == SET_NO) {
//
//        //If we get a SET op for an entity that's not visible, it means that the entity has moved
//        //within our field of vision without sending an Appear op first. We should treat this as a
//        //regular Appear op and issue a Look op back, to get more info.
//        if (!m_entity.isVisible()) {
//            m_entity.getView()->sendLookAt(m_entity.getId());
//        }
//
//		for (const auto& arg : args) {
//			if (arg->hasAttr("loc")) {
//				m_entity.setLocationFromAtlas(arg->getAttr("loc").asString());
//			}
//
//			m_entity.setFromRoot(arg, true /* movement allowed */);
//		}
//
//        return HANDLED;
//    }
    
    if (op->instanceOf(IMAGINARY_NO)) {
        if (args.empty()) {
			error() << "entity " << m_entity.getId() << " sent imaginary with no args: " << op;
		} else {
			for (const auto& arg : args) {
				m_entity.onImaginary(arg);
			}
		}
        return HANDLED;        
    }
    
    // explicitly do NOT handle set ops here, since they can
    // validly change multiple entities - handled by the IGRouter

    return IGNORED;
}


} // of namespace Eris
