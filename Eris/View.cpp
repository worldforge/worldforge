#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/view.h>
#include <Eris/avatar.h>
#include <Eris/redispatch.h>
#include <Eris/Entity.h>
#include <Eris/logStream.h>
#include <Eris/Factory.h>
#include <Eris/Connection.h>

#include <Atlas/Objects/Entity.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::GameEntity;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris
{

class SightEntityRedispatch : public Redispatch
{
public:
    SightEntityRedispatch(const std::string& eid, Connection* con, const Root& obj) :
        Redispatch(con, obj),
        m_id(eid)
    {;}
    
    void onSightEntity(Entity* ent)
    {
        if (ent->getId() == m_id) post(); // KA-ching!
    }
    
private:
    std::string m_id;
};

#pragma mark -

View::View(Avatar* av, const GameEntity& gent) :
    m_owner(av)
{
    assert(gent->getId() == av->getId());
    
}

View::~View()
{
    // delete them all!
    for (IdEntityMap::iterator E = m_visible.begin(); E != m_visible.end(); ++E)
        delete E->second;
        
    for (IdEntityMap::iterator E = m_invisible.begin(); E != m_invisible.end(); ++E)
        delete E->second;
}

Entity* View::getTopLevel() const
{
    if (!m_topLevel)
    {
        // issue an anonymous LOOK
        getEntityFromServer("");
    }
    
    return m_topLevel;
}

#pragma mark -

void View::appear(const std::string& eid, float stamp)
{
    Entity* ent = getExistingEntity(eid);
    if (ent)
    {
        ent->setVisible(true); // will ultimately cause appearances
    } else {
        getEntityFromServer(eid);
        // once it arrives, it'll get bound in and Appearance fired if it's
        // visible
    }
}

void View::disappear(const std::string& eid)
{
    Entity* ent = getExistingEntity(eid);
    if (ent)
    {
        ent->setVisible(false); // will ultimately cause disapeparances
    } else {
        if (isPending(eid)) cancelPendingSight(eid);
    }
}

void View::initialSight(const GameEntity& gent)
{    
    if (getExistingEntity(gent->getId()))
    {
        error() << "got 'initial' sight of entity " << gent->getId() << " which View already has";
        return;
    }

    Entity* location = NULL;
    if (!gent->isDefaultLoc())
    {
        location = getExistingEntity(gent->getLoc());
        if (!location)
        {
            /* get location entity from server; when we get it's sight, this
            entity will be in it's contents, and we'll get looked up */
            getEntityFromServer(gent->getLoc());
        }
    }
    
    // this will work even if ent isn't in the pending set
    m_pendingEntitySet.erase(gent->getId());
    
    // run the factory method ...
    Entity* ent = Factory::createEntity(gent);
    
    if (m_cancelledSightSet.count(gent->getId()))
    {
        debug() << "got initial sight for cancelled entity " << gent->getId() << ", supressing";
        m_cancelledSightSet.erase(gent->getId());
    } else
        ent->setVisible(true);
    
    if (location)
        ent->setLocation(location); // will set visibility and fire appearances
        
    if (gent->isDefaultLoc()) // new top level entity
        setTopLevelEntity(ent);
    
    InitialSightEntity.emit(ent);
}

void View::create(const GameEntity& gent)
{
    Entity* location = NULL;
    if (!gent->isDefaultLoc())
    {
        location = getExistingEntity(gent->getLoc());
        if (!location)
        {
            /* get location entity from server; when we get it's sight, this
            entity will be in it's contents, and we'll get looked up */
            getEntityFromServer(gent->getLoc());
        }
    }
    
    // build it
    Entity* ent = Factory::createEntity(gent);
    if (location)
        ent->setLocation(location);

    ent->setVisible(true);
    EntityCreated.emit(ent);
}

void View::deleteEntity(const std::string& eid)
{
    Entity* ent = getExistingEntity(eid);
    if (ent)
    {
        EntityDeleted.emit(ent);
        #warning entity deletion in view is suspect
        if (ent->isVisible())
            m_visible.erase(ent->getId());
        else
            m_invisible.erase(ent->getId());
        delete ent; // actually kill it off
    } else {
        if (isPending(eid))
        {
            debug() << "got delete for pending entity, argh";
            cancelPendingSight(eid);
        } else
            error() << "got delete for non-visible entity " << eid;
    }
}

Entity* View::getExistingEntity(const std::string& eid) const
{
    IdEntityMap::const_iterator I = m_visible.find(eid);
    if (I != m_visible.end())
        return I->second;
        
    I = m_invisible.find(eid);
    if (I != m_invisible.end())
        return I->second;
        
    return NULL;
}

bool View::isPending(const std::string& eid) const
{
    return m_pendingEntitySet.count(eid);
}

#pragma mark -

void View::setEntityVisible(Entity* ent, bool vis)
{
    assert(ent->isVisible() == vis);
    
    if (vis)
    {
        assert(m_visible.count(ent->getId()) == 0);
        assert(m_invisible.erase(ent->getId()) == 1);
        m_visible[ent->getId()] = ent;
        
    } else {
        assert(m_visible.erase(ent->getId()) == 1);
        assert(m_invisible.count(ent->getId()) == 0);
        m_invisible[ent->getId()] = ent;
    }
}

#pragma mark -

Connection* View::getConnection() const
{
    return m_owner->getConnection();
}

void View::getEntityFromServer(const std::string& eid)
{
    if (m_pendingEntitySet.count(eid))
    {
        debug() << "suppressing duplicate getEntityFromServer for entity " << eid;
        return;
    }
    
    m_pendingEntitySet.insert(eid);
    assert(m_cancelledSightSet.count(eid) == 0);
    
    Look look;
    if (!eid.empty())
    {
	Root what;
        what->setId(eid);
    }
	
    look->setSerialno(getNewSerialno());
    look->setFrom(m_owner->getId());
    getConnection()->send(look);
}

void View::cancelPendingSight(const std::string& eid)
{
    if (!m_pendingEntitySet.count(eid))
    {
        error() << "asked to cancel pending sight for entity " << eid << ", but it's not pending!";
        return;
    }

    m_cancelledSightSet.insert(eid);
    m_pendingEntitySet.erase(eid);
    // cancel outstanding redispatches ...
}

void View::setTopLevelEntity(Entity* newTopLevel)
{
    debug() << "setting new top-level entity";
    
    if (m_topLevel)
    {
        if (m_topLevel->isVisible() && (m_topLevel->getLocation() == NULL))
            error() << "old top-level entity is visible, but has no location";
    }
    
    assert(newTopLevel->isVisible());
    assert(newTopLevel->getLocation() == NULL);
    
    m_topLevel = newTopLevel;
    TopLevelEntityChanged.emit(); // fire the signal
}

} // of namespace Eris
