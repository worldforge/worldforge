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
    IdEntityMap::iterator E = m_visible.find(eid);
    if (E != m_visible.end())
    {
        error() << "duplicate appear for entity " << eid;
        return;
    }
    
// is it in the invisible pool?
    E = m_invisible.find(eid);
    if (E != m_invisible.end())
    {
        if (stamp > E->second->getStamp())
        {
            // re-LOOK, but should we appear with the old data anyway?
        }
        
        E->second->setVisible(true);
        Apperance.emit(E->second);
        return;
    }
    
    getEntityFromServer(eid);
    
// build a trivial Appearance to send once we have the Entity in sight
    Root what;
    what->setId(eid);
    
    Appearance appear;
    appear->setTo(m_avatar->getId());
    appear->setFrom(eid);
    appear->setArgs1(what);
    
// and hook it in
    SightEntityRedispatch* ser = new SightEntityRedispatch(eid, getConnection(), appear);
    InitialSightEntity.connect(SigC::slot(*ser, &SightEntityRedispatch::onSightEntity));
}

void View::disappear(const std::string& eid)
{
    IdEntityMap::iterator E = m_visible.find(eid);
    if (E == m_visible.end())
    {
        if (m_pendingEntitySet.count(eid))
        {
            debug() << "got disappear for pending entity, argh";
            cancelPendingSight(eid);
        } else
            error() << "got disappear for non-visible entity, ignoring";
        
        return;
    }
    
    E->second->setVisible(false);
    Disappearance.emit(E->second);
    m_invisible.insert(m_invisible.end(), *E);
    m_visible.erase(E);
}

void View::initialSight(const GameEntity& gent)
{    
    IdEntityMap::iterator E = m_visible.find(gent->getId());
    if (E != m_visible.end())
    {
        error() << "got 'initial' sight of entity " << gent->getId() << " which View already has";
        return;
    }
    
    if (m_cancelledSightSet.count(gent->getId()))
    {
        debug() << "got initial sight for cancelled entity " << gent->getId() << ", supressing";
        m_cancelledSightSet.erase(gent->getId());
        return; 
    }

    Entity* location = NULL;
    if (!gent->isDefaultLoc())
    {
        IdEntityMap::iterator L = m_visible.find(gent->getLoc());
        if (L == m_visible.end())
        {
        // uh-oh, we don't have the location entity yet : retrieve it and try
        // again when we do
            getEntityFromServer(gent->getLoc());
            
            Sight st;
            st->setTo(m_avatar->getId());
            st->setFrom(gent->getId());
            st->setArgs1(gent);
            
            SightEntityRedispatch* ser = new SightEntityRedispatch(gent->getLoc(), getConnection(), st);
            InitialSightEntity.connect(SigC::slot(*ser, &SightEntityRedispatch::onSightEntity));
            return;
        } else
            location = L->second; // we already have it, easy
    }
    
    // this will work even if ent isn't in the pending set
    m_pendingEntitySet.erase(gent->getId());
    
    // run the factory method ...
    Entity* ent = Factory::builtEntity(gent);
    if (location)
        ent->setLocation(location);
        
    if (!location)
        m_topLevel = ent;
    
    InitalSightEntity.emit(ent);
}

void View::create(const GameEntity& gent)
{
    Entity* location = NULL;
    if (!gent->isDefaultLoc())
    {
        IdEntityMap::iterator L = m_visible.find(gent->getLoc());
        if (L == m_visible.end())
        {
            getEntityFromServer(gent->getLoc());
            
            Create cr;
            cr->setArgs1(gent);
            
            Sight stcr;
            stcr->setTo(m_avatar->getId());
            stcr->setFrom(gent->getId());
            stcr->setArgs1(cr);
            
            SightEntityRedispatch* ser = new SightEntityRedispatch(gent->getLoc(), getConnection(), stcr);
            InitialSightEntity.connect(SigC::slot(*ser, &SightEntityRedispatch::onSightEntity));
            return;
        } else
            location = L->second; // we already have it, easy
    }
    
    // build it
    Entity* ent = Factory::builtEntity(gent);
    if (location)
        ent->setLocation(location);

    EntityCreated.emit(eny);
}

void View::deleteEntity(const std::string& eid)
{
    IdEntityMap::iterator E = m_visible.find(eid);
    if (E == m_visible.end())
    {
        if (m_pendingEntitySet.count(eid))
        {
            debug() << "got delete for pending entity, argh";
            cancelPendingSight(eid);
        } else
            error() << "got delete for non-visible entity " << eid;
        
        return;
    }
    
    EntityDeleted.emit(E->second);
    delete E->second; // actually kill it off
    m_visible.erase(E);
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
