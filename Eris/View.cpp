#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/View.h>
#include <Eris/Redispatch.h>
#include <Eris/Entity.h>
#include <Eris/LogStream.h>
#include <Eris/Factory.h>
#include <Eris/Connection.h>
#include <Eris/Exceptions.h>
#include <Eris/Avatar.h>

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

View::View(Avatar* av) :
    m_owner(av),
    m_topLevel(NULL)
{
    
}

View::~View()
{
    for (IdEntityMap::iterator E = m_contents.begin(); E != m_contents.end(); ++E)
        delete E->second;
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
    if (vis)
        Apperance.emit(ent);
    else
        Disappearance.emit(ent);
}

#pragma mark -
// Atlas operation handlers

void View::appear(const std::string& eid, float stamp)
{
    Entity* ent = getEntity(eid);
    if (!ent)
    {
        getEntityFromServer(eid);
        return; // everything else will be done once the SIGHT arrives
    }

    if (ent->isVisible())
    {
        error() << "server sent an appearance for entity " << eid << " which thinks it is already visible.";
        return;
    }

    if (stamp > ent->getStamp())
    {
        // local data is out of data, re-look
        getEntityFromServer(eid);
    } else
        ent->setVisible(true);

}

void View::disappear(const std::string& eid)
{
    Entity* ent = getEntity(eid);
    if (ent)
    {
        ent->setVisible(false); // will ultimately cause disapeparances
    } else {
        if (isPending(eid))
        {
            m_pending[eid] = SACTION_HIDE;
        } else
            error() << "got disappear for unknown entity " << eid;
    }
}

void View::sight(const GameEntity& gent)
{
    bool visible = true;
    
    std::string eid = gent->getId();
    debug() << "got sight of " << eid;
    
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
    if (ent)
    {
        // existing entity, update in place
        ent->sight(gent);
    } else {
        ent = initialSight(gent);
        EntitySeen.emit(ent);
    }
    
    ent->setVisible(visible);
    
    if (gent->isDefaultLoc()) // new top level entity
        setTopLevelEntity(ent);
}

Entity* View::initialSight(const GameEntity& gent)
{
    Entity* ent = Factory::createEntity(gent, this);
    m_contents[gent->getId()] = ent;

    InitialSightEntity.emit(ent);
    return ent;
}

void View::create(const GameEntity& gent)
{
    Entity* ent = Factory::createEntity(gent, this);
    m_contents[gent->getId()] = ent;

    if (gent->isDefaultLoc())
        setTopLevelEntity(ent);

    ent->setVisible(true);
    EntityCreated.emit(ent);
}

void View::deleteEntity(const std::string& eid)
{
    Entity* ent = getEntity(eid);
    if (ent)
    {
        EntityDeleted.emit(ent);
        #warning entity deletion in view is suspect
        m_contents.erase(eid);
        delete ent; // actually kill it off
    } else {
        if (isPending(eid))
        {
            debug() << "got delete for pending entity, argh";
            m_pending[eid] = SACTION_DISCARD;
        } else
            error() << "got delete for non-visible entity " << eid;
    }
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
    if (isPending(eid))
    {
        // we force the action back to SACTION_APPEAR in a minute
        debug() << "duplicate getEntityFromServer for entity " << eid;
    }
    
    Look look;
    if (!eid.empty())
    {
        m_pending[eid] = SACTION_APPEAR;
	Root what;
        what->setId(eid);
        look->setArgs1(what);
    }
    
    look->setSerialno(getNewSerialno());
    look->setFrom(m_owner->getId());
    
    getConnection()->send(look);
}

void View::setTopLevelEntity(Entity* newTopLevel)
{
    debug() << "setting new top-level entity to " << newTopLevel->getId();

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

} // of namespace Eris
