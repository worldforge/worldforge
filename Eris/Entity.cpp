#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Entity.h>
#include <Eris/Connection.h>
#include <Eris/TypeInfo.h>
#include <Eris/LogStream.h>
#include <Eris/EntityRouter.h>
#include <Eris/View.h>
#include <Eris/Exceptions.h>

#include <wfmath/atlasconv.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/BaseObject.h>

#include <algorithm>
#include <set> 
#include <cmath>
#include <cassert>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::GameEntity;
using Atlas::Message::Element;
using Atlas::Objects::smart_static_cast;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris {

Entity::Entity(const GameEntity& ge, TypeInfo* ty, View* vw) :
    m_type(ty),
    m_location(NULL),
    m_id(ge->getId()),
    m_stamp(-1.0),
    m_visible(false),
    m_limbo(false),
    m_updateLevel(0),
    m_view(vw),
    m_hasBBox(false)
{
    m_router = new EntityRouter(this);
    m_view->getConnection()->registerRouterForFrom(m_router, m_id);
    sight(ge);
}

Entity::~Entity()
{
    debug() << "deleting entity " << m_id;
    
    for (unsigned int C=0; C < m_contents.size(); ++C) {
        m_contents[C]->setLocation(NULL);
        m_contents[C]->m_limbo = true;
    }
    
    setLocation(NULL);
    m_view->getConnection()->unregisterRouterForFrom(m_router, m_id);
    delete m_router;
}

#pragma mark -

Element Entity::valueOfAttr(const std::string& attr) const
{
    AttrMap::const_iterator A = m_attrs.find(attr);
    if (A == m_attrs.end())
    {
        error() << "did getAttr(" << attr << ") on entity " << m_id << " which has no such attr";
        return Element();
    } else
        return A->second;
}

bool Entity::hasAttr(const std::string& attr) const
{
    return m_attrs.count(attr) > 0;
}

SigC::Connection Entity::observe(const std::string& attr, const AttrChangedSlot& slot)
{
    // sometimes, I realize how great SigC++
    return m_observers[attr].connect(slot);
}

#pragma mark -

void Entity::sight(const GameEntity &ge)
{    
    if (!ge->isDefaultLoc())
        setLocationFromAtlas(ge->getLoc());
    
    setContentsFromAtlas(ge->getContains());
    
    setFromRoot(smart_static_cast<Root>(ge));
}

void Entity::setFromRoot(const Root& obj)
{	
    beginUpdate();
    
    for (Atlas::Objects::RootData::const_iterator A = obj->begin(); A != obj->end(); ++A)
    {
        if ((A->first == "id") || (A->first == "loc") || (A->first == "contains"))
        {
            continue; // never set these on a SET op
        }
        
        // see if the value in the sight matches the exsiting value
        AttrMap::iterator I = m_attrs.find(A->first);
        if ((I != m_attrs.end()) && (I->second == A->second))
            continue;

        setAttr(A->first, A->second);
    }
    
    endUpdate();
}

void Entity::talk(const Root& talkArgs)
{
    if (!talkArgs->hasAttr("say"))
    {
        error() << "entity " << m_id << " got talk with no 'say' argument";
        return;
    }
    
    debug() << "entity " << m_id << " said " << talkArgs->getAttr("say").asString();
    // just emit the signal
    Say.emit(talkArgs->getAttr("say").asString());
}

void Entity::moved()
{
    Moved.emit(this);
}

#pragma mark -

void Entity::setAttr(const std::string &attr, const Element &val)
{
    beginUpdate();

    if (!nativeAttrChanged(attr, val))
    {
        // add to map
        m_attrs[attr] = val;

        // fire observers
        if (m_observers.count(attr))
            m_observers[attr].emit(attr, val);
    }

    addToUpdate(attr);
    endUpdate();

}

bool Entity::nativeAttrChanged(const std::string& attr, const Element& v)
{
    if (attr == "name")
    {
        m_name = v.asString();
        return true;
    }
    else if (attr == "stamp")
    {
        m_stamp = v.asFloat();
        return true;
    }
    else if (attr == "pos")
    {
        m_position.fromAtlas(v);
        return true;
    }
    else if (attr == "velocity")
    {
        m_velocity.fromAtlas(v);
        return true;
    }
    else if (attr == "orientation")
    {
        m_orientation.fromAtlas(v);
        return true;
    }
    else if (attr == "description")
    {
        m_description = v.asString();
        return true;
    }
    else if (attr == "bbox")
    {
	m_bbox.fromAtlas(v);
        m_hasBBox = true;
        return true;
    }
    else if ((attr == "loc") ||(attr == "contains"))
    {
        throw InvalidOperation("tried to set loc or contains via setProperty");
    }

    return false; // not a native property
}

void Entity::beginUpdate()
{
    ++m_updateLevel;
}

void Entity::addToUpdate(const std::string& attr)
{
    assert(m_updateLevel > 0);
    m_modifiedAttrs.insert(attr);
}

void Entity::endUpdate()
{
    if (m_updateLevel < 1)
    {
        error() << "mismatched begin/end update pair on entity";
        return;
    }
        
    if (--m_updateLevel == 0) // unlocking updates
    {
        Changed.emit(this, m_modifiedAttrs);
        
        if (m_modifiedAttrs.count("pos") || 
            m_modifiedAttrs.count("velocity") ||
            m_modifiedAttrs.count("orientation"))
        {
           moved(); // call the hook method, and hence emit the signal
        }
        
        m_modifiedAttrs.clear();
    }
}

#pragma mark -

void Entity::setLocationFromAtlas(const std::string& locId)
{
    assert(!locId.empty());
    
    Entity* newLocation = m_view->getEntity(locId);
    if (!newLocation)
    {
        debug() << "placing entity " << this << "(" << m_id << ") in limbo till we get " << locId;
        m_view->getEntityFromServer(locId);
        
        m_limbo = true;
        setVisible(false); // fire disappearance, VisChanged if necessary
        
        if (m_location)
            removeFromLocation();
        m_location = NULL;
        return;
    }
    
    setLocation(newLocation);
}

void Entity::setLocation(Entity* newLocation)
{
    if (newLocation == m_location) return;
    
    if (m_location)
        removeFromLocation();
    
// do the actual member updating
    bool wasVisible = isVisible();
    
    Entity* oldLocation = m_location;
    m_location = newLocation;
    
    LocationChanged.emit(this, oldLocation);
    
// fire VisChanged and Appearance/Disappearance signals
    updateCalculatedVisibility(wasVisible);
    
    if (m_location)
        addToLocation();
}

void Entity::addToLocation()
{
    assert(!m_location->hasChild(m_id));
    m_location->addChild(this);
}

void Entity::removeFromLocation()
{
    assert(m_location->hasChild(m_id));
    m_location->removeChild(this);
}

#pragma mark -

void Entity::buildEntityDictFromContents(IdEntityMap& dict)
{
    for (unsigned int C=0; C < m_contents.size(); ++C) {
        Entity* child = m_contents[C];
        dict[child->getId()] = child;
    }
}

void Entity::setContentsFromAtlas(const StringList& contents)
{
// convert existing contents into a map, for fast membership tests
    IdEntityMap oldContents;
    buildEntityDictFromContents(oldContents);
  //  m_contents.clear();
   // debug() << "oldContents has size " << oldContents.size();
    
// iterate over new contents
    for (StringList::const_iterator I=contents.begin(); I != contents.end(); ++I)
    {
        Entity* child = NULL;
        
        IdEntityMap::iterator J = oldContents.find(*I);
        if (J != oldContents.end()) {
            child = J->second;
            assert(child->getLocation() == this);
            oldContents.erase(J);
        } else {
            child = m_view->getEntity(*I);
            if (!child)
            {
                // we don't have the entity at all, so request it and skip
                // processing it here; everything will come right when it
                // arrives.
                m_view->getEntityFromServer(*I);
                // m_pendingContents.insert(*I);
                continue;
            }
            
            if (child->m_limbo)
            {
                debug() << "found child " << child << " in limbo";
                child->m_limbo = false;
            }
            else if (child->isVisible())
            {
                // server has gone mad, it has a location, and it's visible
                error() << "got set of contents, specifying child " << child
                    << " which is currently visible in another location";
                continue;
            }
            
            /* we have found the child, update it's location */
            child->setLocation(this);
        }
    
        child->setVisible(true);
    } // of contents list iteration
    
// mark previous contents which are not in new contents as invisible
    for (IdEntityMap::const_iterator J = oldContents.begin(); J != oldContents.end(); ++J) {
        //m_contents.push_back(J->second);
        J->second->setVisible(false);
    }
}

bool Entity::hasChild(const std::string& eid) const
{
    for (EntityArray::const_iterator C=m_contents.begin(); C != m_contents.end(); ++C) {
        if ((*C)->getId() == eid) return true;
    }
    
    return false;
}

void Entity::addChild(Entity* e)
{
    m_contents.push_back(e);
    ChildAdded.emit(this, e);
    assert(e->getLocation() == this);
}

void Entity::removeChild(Entity* e)
{
    assert(e->getLocation() == this);
    
    for (EntityArray::iterator C=m_contents.begin(); C != m_contents.end(); ++C)
    {
        if (*C == e)
        {
            m_contents.erase(C);
            ChildRemoved.emit(this, e);
            return;
        }
    }
        
   error() << "child " << e->getId() << " of entity " << m_id << " not found doing remove";
}

#pragma mark -
// visiblity related methods

void Entity::setVisible(bool vis)
{
    bool wasVisible = isVisible(); // store before we update m_visible
    m_visible = vis;
    
    updateCalculatedVisibility(wasVisible);
}

bool Entity::isVisible() const
{
    if (m_limbo) return false;
    
    if (m_location)
        return m_visible && m_location->isVisible();
    else
        return m_visible;
}

void Entity::updateCalculatedVisibility(bool wasVisible)
{
    bool nowVisible = isVisible();
    if (nowVisible == wasVisible) return;
    
    /* the following code looks odd, so remember that only one of nowVisible and
    wasVisible can ever be true. The structure is necesary so that we fire
    Appearances top-down, but Disappearances bottom-up. */
    
    if (nowVisible) {
        m_view->setEntityVisible(this, true);
        visibilityChanged(true);
    }
    
    for (unsigned int C=0; C < m_contents.size(); ++C)
    {
        /* in case this isn't clear; if we were visible, then child visibility
        was simply it's locally set value; if we were invisible, that the
        child must also have been invisile too. */
        bool childWasVisible = wasVisible ? m_contents[C]->m_visible : false;
        m_contents[C]->updateCalculatedVisibility(childWasVisible);
    }
    
    if (wasVisible) {
        m_view->setEntityVisible(this, false);
        visibilityChanged(false);
    }
}

void Entity::visibilityChanged(bool vis)
{
    VisibilityChanged.emit(this, vis);
}

} // of namespace 
