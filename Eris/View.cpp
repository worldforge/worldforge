#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/view.h>
#include <Eris/avatar.h>
#include <Eris/redispatch.h>

using namespace Atlas::Objects::Operation;

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
        if (ent->getID() == m_id)
            post(); // KA-ching!
    }
    
private:
    std::string m_id;
};

#pragma mark -

View::View(Avatar* av) :
    m_avatar(av)
{

}

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
    Entity* ent = Factory::builtEntity(gent);
    
    if (m_cancelledSightSet.count(gent->getId()))
    {
        debug() << "got initial sight for cancelled entity " << gent->getId() << ", supressing";
        m_cancelledSightSet.erase(gent->getId());
    } else
        ent->setVisible(true);
    
    if (location)
        ent->setLocation(location); // will set visibility and fire appearances
        
    if (gent->isDefaultLoc())
        m_topLevel = ent;
    
    InitalSightEntity.emit(ent);
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
    Entity* ent = Factory::builtEntity(gent);
    if (location)
        ent->setLocation(location);

    ent->setVisible(true);
    EntityCreated.emit(eny);
}

void View::deleteEntity(const std::string& eid)
{
    Entity* ent = getExistingEntity(eid);
    if (ent)
    {
        EntityDeleted.emit(E->second);
        delete E->second; // actually kill it off
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
    return m_avatar->getConnection();
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
    look->setFrom(m_avatar->getId());
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

} // of namespace Eris
