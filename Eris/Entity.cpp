#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Entity.h>
#include <Eris/World.h>
#include <Eris/Connection.h>
#include <Eris/Utils.h>
#include <Eris/TypeInfo.h>
#include <Eris/Log.h>
#include <Eris/Exceptions.h>
#include <Eris/entityRouter.h>

#include <wfmath/atlasconv.h>

#include <algorithm>
#include <set> 
#include <cmath>
#include <cassert>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::GameEntity;


namespace Eris {

Entity::Entity(const GameEntity& ge, TypeInfoPtr ty) :
    m_type(ty),
    m_location(NULL),
    m_id(ge->getId()),
    m_stamp(-1.0),
    m_visible(false),
    m_updateLevel(0),
    _hasBBox(false)
{
    m_router = new EntityRouter(this);
}

Entity::~Entity()
{
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

SigC::Connection Entity::observe(const std::string& attr, AttrChangedSlot slot)
{
    // sometimes, I realize how great SigC++
    return m_observers[prop].connect(slot);
}


#pragma mark -

void Entity::setContainer(Entity *cnt)
{
	Recontainered.emit(_container, cnt);
	_container = cnt;
}

void Entity::addMember(Entity *e)
{
    Eris::log(LOG_DEBUG, "adding entity '%s' as member of '%s'",
	e->getName().c_str(), getName().c_str());
    
    assert(e != this);
    
    _members.push_back(e);
    AddedMember.emit(e);
    e->setContainer(this);
}

void Entity::rmvMember(Entity *e)
{
    if (!e)
	throw InvalidOperation("passed NULL pointer to Entity::rmvMember");
    EntityArray::iterator ei = std::find(_members.begin(), _members.end(), e);
    if (ei == _members.end())
	    throw InvalidOperation("Unknown member " + e->getName() + 
		" to remove from " + getName());
    _members.erase(ei);
    RemovedMember.emit(e);
}

Entity* Entity::getMember(unsigned int i)
{
	if (i >= _members.size())
		throw InvalidOperation("Illegal member index");
	return _members[i];
}


#pragma mark -

void Entity::sight(const GameEntity &ge)
{    
    setFromRoot(smart_static_cast<Root>(ge));
}

void Entity::setFromRoot(const Root& obj)
{	
    beginUpdate();
    
    for (Root::const_iterator A = obj.begin(); A != obj.end(); ++A)
    {
        if (A->first == "id") 
        {
            assert(A->second.asString() == m_id);
            continue; // never set ID
        }
        
        // see if the value in the sight matches the exsiting value
        AttributeMap::iterator I = m_attrs.find(A->first);
        if ((I != m_attrs.end()) && (I->second == A->second))
            continue;

        setProperty(A->first, A->second);
    }
    
    endUpdate();
}

void Entity::talk(const Root& talkArgs)
{
    if (!talkArgs.hasAttr("say"))
    {
        error() << "entity " << m_id << " got talk with no 'say' argument";
        return;
    }
    
    // just emit the signal
    Say.emit(talkArgs.getAttr("say").asString());
}

void Entity::moved()
{
    Moved.emit(this);
}

#pragma mark -

void Entity::setAttr(const std::string &s, const Element &val)
{
    beginUpdate();

    if (!nativePropertyChanged(attr, e))
    {
        // add to map
        m_attrs[attr] = e;

        // fire observers
        if (m_observers.count(prop))
            m_observers[attr].emit(e);
    }

    addToUpdate(prop);
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
	WFMath::Point<3> pos;
	pos.fromAtlas(val);
	setPosition(pos);
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
	m_bbox.fromAtlas(val);
        m_hasBBox = true;
        return true;
    }
    else if (attr == "loc")
    {
	std::string loc = v.asString();
	setContainerById(loc);
        return true;
    }
    else if (attr == "contains")
    {
	... implement me .... 
        return true;
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
        error() << "mismatched begin/end update pair on entity"
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

void Entity::setPosition(const WFMath::Point<3>& pt)
{
    _position = pt;
}

void Entity::setContents(const Atlas::Message::Element::ListType &contents)
{
    for (Atlas::Message::Element::ListType::const_iterator
	    C=contents.begin(); C!=contents.end();++C) {
	// check we have it; if yes, ensure it's installed. if not, it will be attached when
	// it arrives.
	Entity *con = _world->lookup(C->asString());
	if (con) {
	    Eris::log(LOG_DEBUG, 
		"already have entity '%s', not setting container",
		con->getName().c_str()
	    );
	}
    }
}

void Entity::setContainerById(const std::string &id)
{
    if ( !_container || (id != _container->getID()) ) {
		
	if (_container) {
	    Eris::log(LOG_DEBUG, "Entity::setContainerById: setting container to NULL");
	    _container->rmvMember(this);
	    _container = NULL;
	}
		
	// have to check this, becuase changes in what the client can see
	// may cause the client's root to change
	if (!id.empty()) {
		Entity *ncr = _world->lookup(id);
		if (ncr) {
			ncr->addMember(this);
			setContainer(ncr);
		} else {
			// setup a redispatch once we have the container
			// sythesises a set, with just the container.
			Atlas::Objects::Operation::Set setc;
				
			Atlas::Message::Element::MapType args;
			args["loc"] = id;
			setc.setArgs(Atlas::Message::Element::ListType(1,args));
			setc.setTo(_id);
			
			Atlas::Objects::Operation::Sight ssc;
			ssc.setArgs(Atlas::Message::Element::ListType(1, setc.asObject()));
			ssc.setTo(_world->getFocusedEntityID());
			ssc.setSerialno(getNewSerialno());
			
			// if we received sets/creates/sights in rapid sucession, this can happen, and is
			// very, very bad
			std::string setid("set_container_" + _id);
			std::string dispatch_id = "op:" + _world->getDispatcherID()
				+ ":sight:entity";

			_world->getConnection()->removeIfDispatcherByPath(
				dispatch_id, setid);
			
			new WaitForDispatch(ssc, dispatch_id, 
				new IdDispatcher(setid, id), _world->getConnection());
		}
	} // of new container is valid
    }	
	
    if (id.empty()) {	// id of "" implies local root
	Eris::log(LOG_DEBUG, "got entity with empty container, assuming it's the world");
	_world->setRootEntity(this);	
    }
}

} // of namespace 
