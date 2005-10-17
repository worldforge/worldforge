#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/View.h>
#include <Eris/Entity.h>
#include <Eris/LogStream.h>
#include <Eris/Connection.h>
#include <Eris/Exceptions.h>
#include <Eris/Avatar.h>
#include <Eris/Factory.h>
#include <Eris/TypeService.h>
#include <Eris/TypeInfo.h>

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::GameEntity;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris
{

View::View(Avatar* av) :
    m_owner(av),
    m_topLevel(NULL),
    m_maxPendingCount(10)
{
    
}

View::~View()
{
    // cascaded delete of all entities, is the hope
    delete m_topLevel;
    assert(m_contents.empty());
    
    for (FactoryStore::iterator F=m_factories.begin(); F != m_factories.end(); ++F) {
        delete *F;
    }
}

Entity* View::getEntity(const std::string& eid) const
{
    IdEntityMap::const_iterator E = m_contents.find(eid);
    if (E == m_contents.end()) return NULL;

    return E->second;
}

void View::setEntityVisible(Entity* ent, bool vis)
{
    assert(ent);
    if (vis) {
        Appearance.emit(ent);
    } else {
        Disappearance.emit(ent);
    }
}

void View::registerFactory(Factory* f)
{
    m_factories.insert(f);
}

void View::notifyWhenEntitySeen(const std::string& eid, const EntitySightSlot& slot)
{
    if (m_contents.count(eid)) {
        error() << "notifyWhenEntitySeen: entity " << eid << " already in View";
        return;
    }
    
    m_notifySights[eid].connect(slot);
    getEntityFromServer(eid);
}

#pragma mark -

void View::update()
{
    WFMath::TimeStamp t(WFMath::TimeStamp::now());
    
    // run motion prediction for each moving entity
    for (EntitySet::iterator it=m_moving.begin(); it != m_moving.end(); ++it)
        (*it)->updatePredictedState(t);
}

void View::addToPrediction(Entity* ent)
{
    assert(ent->isMoving());
    assert(m_moving.count(ent) == 0);
    m_moving.insert(ent);
}

void View::removeFromPrediction(Entity* ent)
{
    assert(ent->isMoving());
    assert(m_moving.count(ent) == 1);
    m_moving.erase(ent);
}

#pragma mark -
// Atlas operation handlers

void View::appear(const std::string& eid, float stamp)
{
    Entity* ent = getEntity(eid);
    if (!ent) {
        getEntityFromServer(eid);
        return; // everything else will be done once the SIGHT arrives
    }

    if (ent->isVisible()) return;
    
    if ((stamp == 0) || (stamp > ent->getStamp())) {
        if (isPending(eid)) {
            m_pending[eid] = SACTION_APPEAR;
        } else {
            // local data is out of data, re-look
            getEntityFromServer(eid);
        }
    } else
        ent->setVisible(true);

}

void View::disappear(const std::string& eid)
{    
    Entity* ent = getEntity(eid);
    if (ent) {
        ent->setVisible(false); // will ultimately cause disapeparances
    } else {
        if (isPending(eid)) {
            debug() << "got disappearance for pending " << eid;
            m_pending[eid] = SACTION_HIDE;
        } else
            warning() << "got disappear for unknown entity " << eid;
    }
}

void View::sight(const GameEntity& gent)
{
    bool visible = true;
    std::string eid = gent->getId();
    
// examine the pending map, to see what we should do with this entity
    if (m_pending.count(eid)) {
        switch (m_pending[eid])
        {
        case SACTION_APPEAR:
            visible = true;
            break;

        case SACTION_DISCARD:
            m_pending.erase(eid);
            return;

        case SACTION_HIDE:
            visible = false;
            break;

        default:
            throw InvalidOperation("got bad pending action for entity");
        }
    
         m_pending.erase(eid);
    }
    
// if we got this far, go ahead and build / update it
    Entity *ent = getEntity(eid);
    if (ent) {
        // existing entity, update in place
        ent->sight(gent);
    } else {
        ent = initialSight(gent);
        EntitySeen.emit(ent);
    }
    
    ent->setVisible(visible);
    
    if (gent->isDefaultLoc()) { // new top level entity
        setTopLevelEntity(ent);
    }
    
// issue the next LOOK of there's a queue. (Note we should stay below the
// max count since we erased an item from the pending map above)
    if (!m_lookQueue.empty()) {
        sendLookAt(m_lookQueue.front());
        m_lookQueue.pop_front();
    }
}

Entity* View::initialSight(const GameEntity& gent)
{
    Entity* ent = createEntity(gent);
    assert(m_contents.count(gent->getId()) == 0);
    m_contents[gent->getId()] = ent;
    ent->init(gent);
    
    InitialSightEntity.emit(ent);
 
    NotifySightMap::iterator it = m_notifySights.find(gent->getId());
    if (it != m_notifySights.end()) {
        it->second.emit(ent);
        m_notifySights.erase(it);
    }
    
    return ent;
}

void View::create(const GameEntity& gent)
{
    Entity* ent = createEntity(gent);
    assert(m_contents.count(gent->getId()) == 0);
    m_contents[gent->getId()] = ent;
    ent->init(gent);
    
    if (gent->isDefaultLoc())
        setTopLevelEntity(ent);

    InitialSightEntity.emit(ent);
    ent->setVisible(true);
    EntityCreated.emit(ent);
}

void View::deleteEntity(const std::string& eid)
{
    Entity* ent = getEntity(eid);
    if (ent) {
        // copy the child array, since setLocation will modify it
        EntityArray contents;
        for (unsigned int c=0; c < ent->numContained(); ++c) {
            contents.push_back(ent->getContained(c));
        }
        
        while (!contents.empty()) {
            Entity* child = contents.back();
            child->setLocation(ent->getLocation());
            
            WFMath::Point<3> newPos = ent->toLocationCoords(child->getPosition());
            WFMath::Quaternion newOrient = ent->getOrientation() * child->getOrientation();
            child->m_position = newPos;
            child->m_orientation = newOrient;
            
            contents.pop_back();
        }

        // force a disappear if one hasn't already happened
        ent->setVisible(false); // redundant?
        EntityDeleted.emit(ent);
        delete ent; // actually kill it off
    } else {
        if (isPending(eid)) {
            debug() << "got delete for pending entity, argh";
            m_pending[eid] = SACTION_DISCARD;
        } else
            warning() << "got delete for unknown entity " << eid;
    }
}

Entity* View::createEntity(const GameEntity& gent)
{
    TypeInfo* type = getConnection()->getTypeService()->getTypeForAtlas(gent);
    assert(type->isBound());
    
    FactoryStore::const_iterator F = m_factories.begin();
    for (; F != m_factories.end(); ++F) {
        if ((*F)->accept(gent, type)) {
            return (*F)->instantiate(gent, type, this);
        }
    }
    
    return new Eris::Entity(gent->getId(), type, this);
}

#pragma mark -

bool View::isPending(const std::string& eid) const
{
    return m_pending.count(eid);
}

Connection* View::getConnection() const
{
    return m_owner->getConnection();
}

void View::getEntityFromServer(const std::string& eid)
{
    if (isPending(eid)) return;
    
    // don't apply pending queue cap for anoynmous LOOKs
    if (!eid.empty() && (m_pending.size() >= m_maxPendingCount)) {
        m_lookQueue.push_back(eid);
        m_pending[eid] = SACTION_QUEUED;
        return;
    }
    
    sendLookAt(eid);
}

void View::sendLookAt(const std::string& eid)
{
    Look look;
    if (!eid.empty()) {
        if (m_pending.count(eid)) {
            switch (m_pending[eid])
            {
            case SACTION_QUEUED:
                // flip over to default (APPEAR) as normal
                m_pending[eid] = SACTION_APPEAR; break;
                
            case SACTION_DISCARD:
            case SACTION_HIDE:
                if (m_notifySights.count(eid) == 0) {
                    // no-one cares, don't bother to look
                    m_pending.erase(eid);
                    return;
                } // else someone <em>does</em> care, so let's do the look, but
                  // keep SightAction unchanged so it discards / is hidden as
                  // expected.
                break;
                
            default:
                // broken state handling logic
                assert(false);
            }
        } else {
            // no previous entry, default to APPEAR
            m_pending[eid] = SACTION_APPEAR;
        }
        
        // pending map is in the right state, build up the args now
        Root what;
        what->setId(eid);
        look->setArgs1(what);
    }
    
    look->setFrom(m_owner->getId());
    getConnection()->send(look);
}

void View::setTopLevelEntity(Entity* newTopLevel)
{
    if (m_topLevel) {
        if (newTopLevel == m_topLevel) return; // no change!
        
        if (m_topLevel->isVisible() && (m_topLevel->getLocation() == NULL))
            error() << "old top-level entity is visible, but has no location";
    }

    assert(newTopLevel->isVisible());
    assert(newTopLevel->getLocation() == NULL);

    m_topLevel = newTopLevel;
    TopLevelEntityChanged.emit(); // fire the signal
}

void View::entityDeleted(Entity* ent)
{
    assert(m_contents.count(ent->getId()));
    m_contents.erase(ent->getId());
}

void View::dumpLookQueue()
{
    debug() << "look queue:";
    for (unsigned int i=0; i < m_lookQueue.size(); ++i) {
        debug() << "\t" << m_lookQueue[i];
    }
}

} // of namespace Eris
