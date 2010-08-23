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
#include <Eris/Operations.h>
#include "TransferInfo.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

#pragma warning(disable: 4068)  //unknown pragma

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Message::Element;

namespace Eris {

IGRouter::IGRouter(Avatar* av) :
    m_avatar(av),
    m_view(av->getView())
{
    m_avatar->getConnection()->registerRouterForTo(this, m_avatar->getId());
    m_actionType = m_avatar->getConnection()->getTypeService()->getTypeByName("action");
}

IGRouter::~IGRouter()
{
    m_avatar->getConnection()->unregisterRouterForTo(this, m_avatar->getId());
}

#pragma mark -

Router::RouterResult IGRouter::handleOperation(const RootOperation& op)
{
    if (!op->isDefaultSeconds()) {
        // grab out world time
        m_avatar->updateWorldTime(op->getSeconds());
    }
    
    const std::vector<Root>& args = op->getArgs();

    if (op->getClassNo() == SIGHT_NO) {
        assert(!args.empty());
        if (args.front()->instanceOf(ROOT_OPERATION_NO)) return handleSightOp(op);
        
        // initial sight of entities
        RootEntity gent = smart_dynamic_cast<RootEntity>(args.front());
        if (gent.isValid()) {
            // View needs a bound TypeInfo for the entity
            TypeInfo* ty = m_avatar->getConnection()->getTypeService()->getTypeForAtlas(gent);
            if (!ty->isBound()) {
                new TypeBoundRedispatch(m_avatar->getConnection(), op, ty);
                return WILL_REDISPATCH;
            }

            m_view->sight(gent);
            return HANDLED;
        }
    }
    
    if (op->getClassNo() == APPEARANCE_NO) {
        for (unsigned int A=0; A < args.size(); ++A) {
            float stamp = -1;
            if (args[A]->hasAttr("stamp")) {
                stamp = args[A]->getAttr("stamp").asFloat();
            }
            
            m_view->appear(args[A]->getId(), stamp);
        }
        
        return HANDLED;
    }
    
    if (op->getClassNo() == DISAPPEARANCE_NO) {
        for (unsigned int A=0; A < args.size(); ++A) {
            m_view->disappear(args[A]->getId());
        }
        
        return HANDLED;
    }
    
    if (op->getClassNo() == UNSEEN_NO)
    {
        m_view->unseen(args.front()->getId());
        return HANDLED;
    }

    // logout
    if (op->getClassNo() == LOGOUT_NO) {
        debug() << "Avatar received forced logout from server";
        const std::vector<Root>& args = op->getArgs();

        if(args.size() == 2) {
            bool gotArgs = true;
            // Teleport logout op
            const Root & arg = args.back();
            Element tp_host_attr;
            Element tp_port_attr;
            Element pkey_attr;
            Element pentity_id_attr;
            std::string teleport_host;
            int teleport_port;
            std::string possess_key;
            std::string possess_entity_id;
            if(arg->copyAttr("teleport_host", tp_host_attr) != 0
                    || !tp_host_attr.isString()) {
                debug() << "No teleport host specified. Doing normal logout."
                        << std::endl << std::flush;
                gotArgs = false;
            } else if (arg->copyAttr("teleport_port", tp_port_attr) != 0
                    || !tp_port_attr.isInt()) {
                debug() << "No teleport port specified. Doing normal logout."
                        << std::endl << std::flush;
                gotArgs = false;
            } else if (arg->copyAttr("possess_key", pkey_attr) != 0
                    || !pkey_attr.isString()) {
                debug() << "No possess key specified. Doing normal logout."
                        << std::endl << std::flush;
                gotArgs = false;
            } else if (arg->copyAttr("possess_entity_id", pentity_id_attr) != 0
                    || !pentity_id_attr.isString()) {
                debug() << "No entity ID specified. Doing normal logout."
                        << std::endl << std::flush;
                gotArgs = false;
            }

            // Extract argument data only if we succeed in extracting them all
            if (gotArgs) {
                teleport_host = tp_host_attr.String();
                teleport_port = tp_port_attr.Int();
                possess_key = pkey_attr.String();
                possess_entity_id = pentity_id_attr.String();
                debug() << "Server transfer data: Host: " << teleport_host << ", Port: "
                << teleport_port << ", " << "Key: "
                << possess_key << ", " << "ID: "
                << possess_entity_id << std::endl << std::flush;
            }


            // Request transfer if all the arguments were extracted nicely
            if (gotArgs) {
                // Now do a transfer request
                TransferInfo transfer(teleport_host, teleport_port, possess_key
                        , possess_entity_id);
                m_avatar->logoutRequested(transfer);
            } else {
                m_avatar->logoutRequested();
            }

        } else {
            // Regular force logout op
            m_avatar->logoutRequested();
        }

        return HANDLED;
    }
    
    return IGNORED;
}

Router::RouterResult IGRouter::handleSightOp(const RootOperation& sightOp)
{
    RootOperation op = smart_dynamic_cast<RootOperation>(sightOp->getArgs().front());
    const std::vector<Root>& args = op->getArgs();

    if (op->getClassNo() == CREATE_NO) {
        assert(!args.empty());
        RootEntity gent = smart_dynamic_cast<RootEntity>(args.front());
        if (gent.isValid()) {
            // View needs a bound TypeInfo for the entity
            TypeInfo* ty = m_avatar->getConnection()->getTypeService()->getTypeForAtlas(gent);
            if (!ty->isBound()) {
                new TypeBoundRedispatch(m_avatar->getConnection(), sightOp, ty);
                return WILL_REDISPATCH;
            }
    
            m_view->create(gent);
            return HANDLED;
        }
    }
    
    if (op->getClassNo() == DELETE_NO) {
        assert(!args.empty());
        m_view->deleteEntity(args.front()->getId());
        return HANDLED;
    }
    
    // becuase a SET op can potentially (legally) update multiple entities,
    // we decode it here, not in the entity router
    if (op->getClassNo() == SET_NO) {
        for (unsigned int A=0; A < args.size(); ++A) {
            Entity* ent = m_view->getEntity(args[A]->getId());
            if (!ent) {
                if (m_view->isPending(args[A]->getId())) {
                    /* no-op, we'll get the state later */
                } else {
                    warning() << " got SET for completely unknown entity " << args[A]->getId();
                }
                    
                continue; // we don't have it, ignore
            }
            
            ent->setFromRoot(args[A], false);
        }
        return HANDLED;
    }
    
    // we have to handle generic 'actions' late, to avoid trapping interesting
    // such as create or divide
    TypeInfo* ty = m_avatar->getConnection()->getTypeService()->getTypeForAtlas(op);
    if (!ty->isBound()) {
        new TypeBoundRedispatch(m_avatar->getConnection(), sightOp, ty);
        return WILL_REDISPATCH;
    }
    
    if (ty->isA(m_actionType)) {
        if (op->isDefaultFrom()) {
            warning() << "received op " << ty->getName() << " with FROM unset";
            return IGNORED;
        }
        
        Entity* ent = m_view->getEntity(op->getFrom());
        if (ent) ent->onAction(op);
        
        return HANDLED;
    }
    
    return IGNORED;
}

} // of namespace Eris
