#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/View.h>
#include <Eris/ViewEntity.h>
#include <Eris/LogStream.h>
#include <Eris/Connection.h>
#include <Eris/Exceptions.h>
#include <Eris/Avatar.h>
#include <Eris/Factory.h>
#include <Eris/TypeService.h>
#include <Eris/TypeInfo.h>
#include <Eris/Task.h>

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

#pragma warning(disable: 4068)  //unknown pragma

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
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
    if (m_topLevel) {
        m_topLevel->shutdown();
        delete m_topLevel;
        if (!m_contents.empty()) {
            warning() << "top level entity is not empty on view destruction";
        }
    }
    // note that errors that occurr very early during world entry, may
    // cause a view to be deleted with no top-level entity; in that case we
    // leak a few entities here.
    
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

sigc::connection View::notifyWhenEntitySeen(const std::string& eid, const EntitySightSlot& slot)
{
    if (m_contents.count(eid)) {
        error() << "notifyWhenEntitySeen: entity " << eid << " already in View";
        return sigc::connection();
    }
    
    sigc::connection c = m_notifySights[eid].connect(slot);
    getEntityFromServer(eid);
    return c;
}

#pragma mark -

void View::update()
{
    WFMath::TimeStamp t(WFMath::TimeStamp::now());
    
    // run motion prediction for each moving entity
    for (EntitySet::iterator it=m_moving.begin(); it != m_moving.end(); ++it)
        (*it)->updatePredictedState(t);
    
    typedef std::set<Task*> TaskSet;
    
    // for first call to update, dt will be zero.
    if (!m_lastUpdateTime.isValid()) m_lastUpdateTime = t;
    WFMath::TimeDiff dt = t - m_lastUpdateTime;
    
    for (TaskSet::iterator it=m_progressingTasks.begin(); it != m_progressingTasks.end(); ++it)
    {
        (*it)->updatePredictedProgress(dt);
    }
    
    m_lastUpdateTime = t;
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

void View::taskRateChanged(Task* t)
{
    if (t->m_progressRate > 0.0)
    {
        m_progressingTasks.insert(t);
    } else {
        m_progressingTasks.erase(t);
    }
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

    if (ent->m_recentlyCreated)
    {
        EntityCreated.emit(ent);
        ent->m_recentlyCreated = false;
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
            //debug() << "got disappearance for pending " << eid;
            m_pending[eid] = SACTION_HIDE;
        } else
            warning() << "got disappear for unknown entity " << eid;
    }
}

void View::sight(const RootEntity& gent)
{
    bool visible = true;
    std::string eid = gent->getId();
    PendingSightMap::iterator pending = m_pending.find(eid);
    
// examine the pending map, to see what we should do with this entity
    if (pending != m_pending.end()) {
        switch (pending->second)
        {
        case SACTION_APPEAR:
            visible = true;
            break;

        case SACTION_DISCARD:
            m_pending.erase(pending);
            issueQueuedLook();
            return;

        case SACTION_HIDE:
            visible = false;
            break;

        case SACTION_QUEUED:
            error() << "got sight of queued entity " << eid << " somehow";
            eraseFromLookQueue(eid);
            break;
    
        default:
            throw InvalidOperation("got bad pending action for entity");
        }
    
         m_pending.erase(pending);
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
        
    if (gent->isDefaultLoc()) { // new top level entity
        setTopLevelEntity(ent);
    }
    
    ent->setVisible(visible);
    issueQueuedLook();
}

Entity* View::initialSight(const RootEntity& gent)
{
    Entity* ent = createEntity(gent);
    
    assert(m_contents.count(gent->getId()) == 0);
    m_contents[gent->getId()] = ent;
    ent->init(gent, false);
    
    InitialSightEntity.emit(ent);
 
    NotifySightMap::iterator it = m_notifySights.find(gent->getId());
    if (it != m_notifySights.end()) {
        it->second.emit(ent);
        m_notifySights.erase(it);
    }
    
    return ent;
}

void View::create(const RootEntity& gent)
{
    std::string eid(gent->getId());
    if (m_contents.count(eid))
    {
        // already known locally, just emit the signal
        EntityCreated.emit( m_contents[eid] );
        return;
    }
    
    bool alreadyAppeared = false;
    PendingSightMap::iterator pending = m_pending.find(eid);
    if (pending != m_pending.end())
    {
        // already being retrieved, but we have the data now
        alreadyAppeared = (pending->second == SACTION_QUEUED) || 
            (pending->second == SACTION_APPEAR);
        pending->second = SACTION_DISCARD; // when the SIGHT turns up
    }
    
    Entity* ent = createEntity(gent);
    m_contents[eid] = ent;
    ent->init(gent, true);
    
    if (gent->isDefaultLoc()) setTopLevelEntity(ent);

    InitialSightEntity.emit(ent);
    
    // depends on relative order that sight(create) and appear are recieved in
    if (alreadyAppeared)
    {
        ent->setVisible(true);
        EntityCreated.emit(ent);
    }
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
        ent->shutdown();
        delete ent; // actually kill it off
    } else {
        if (isPending(eid)) {
            //debug() << "got delete for pending entity, argh";
            m_pending[eid] = SACTION_DISCARD;
        } else
            warning() << "got delete for unknown entity " << eid;
    }
}

Entity* View::createEntity(const RootEntity& gent)
{
    TypeInfo* type = getConnection()->getTypeService()->getTypeForAtlas(gent);
    assert(type->isBound());
    
    FactoryStore::const_iterator F = m_factories.begin();
    for (; F != m_factories.end(); ++F) {
        if ((*F)->accept(gent, type)) {
            return (*F)->instantiate(gent, type, this);
        }
    }
    
    return new ViewEntity(gent->getId(), type, this);
}

void View::unseen(const std::string& eid)
{
    Entity* ent = getEntity(eid);
    if (!ent) return; // unseen for non-local, ignore
    
    ent->shutdown();
    delete ent; // is that all?
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
        PendingSightMap::iterator pending = m_pending.find(eid);
        if (pending != m_pending.end()) {
            switch (pending->second)
            {
            case SACTION_QUEUED:
                // flip over to default (APPEAR) as normal
                pending->second = SACTION_APPEAR; break;
                
            case SACTION_DISCARD:
            case SACTION_HIDE:
                if (m_notifySights.count(eid) == 0) {
                    // no-one cares, don't bother to look
                    m_pending.erase(pending);
                    issueQueuedLook();
                    return;
                } // else someone <em>does</em> care, so let's do the look, but
                  // keep SightAction unchanged so it discards / is hidden as
                  // expected.
                break;
                
            case SACTION_APPEAR:
                // this can happen if a queued entity disappears and then
                // re-appears, all while in the look queue. we can safely fall
                // through.
                break;
                
            default:
                // broken state handling logic
                assert(false);
            }
        } else {
            // no previous entry, default to APPEAR
            m_pending.insert(pending, std::make_pair(eid, SACTION_APPEAR));
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

    assert(newTopLevel->getLocation() == NULL);
    m_topLevel = newTopLevel;
    TopLevelEntityChanged.emit(); // fire the signal
}

void View::entityDeleted(Entity* ent)
{
    assert(m_contents.count(ent->getId()));
    m_contents.erase(ent->getId());
}

void View::issueQueuedLook()
{
    if (m_lookQueue.empty()) return;
    std::string eid = m_lookQueue.front();
    m_lookQueue.pop_front();
    sendLookAt(eid);
}

void View::dumpLookQueue()
{
    debug() << "look queue:";
    for (unsigned int i=0; i < m_lookQueue.size(); ++i) {
        debug() << "\t" << m_lookQueue[i];
    }
}

void View::eraseFromLookQueue(const std::string& eid)
{
    std::deque<std::string>::iterator it;
    for (it = m_lookQueue.begin(); it != m_lookQueue.end(); ++it) {
        if (*it == eid) {
            m_lookQueue.erase(it);
            return;
        }
    }
    
    error() << "entity " << eid << " not present in the look queue";
}

} // of namespace Eris
