#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <algorithm>
#include <assert.h>
#include <set> 
#include <cmath>

#include <Atlas/Objects/Entity/RootEntity.h>
#include <Atlas/Objects/Entity/GameEntity.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Sight.h>

#include <wfmath/atlasconv.h>

#include "Entity.h"
#include "World.h"
#include "Connection.h"
#include "Utils.h"
#include "Wait.h"
#include "Property.h"
#include "TypeInfo.h"
#include "OpDispatcher.h"
#include "IdDispatcher.h"
#include "Log.h"

using namespace Atlas::Message;

namespace Eris {

Entity::Entity(const Atlas::Objects::Entity::GameEntity &ge) :
	_id(ge.GetId()),
	_stamp(-1.0),
	_visible(true),
	_container(NULL),
	_position(ge.GetPos()),
	_velocity(ge.GetVelocity()),
	_orientation(1.0, 0., 0., 0.),
	_inUpdate(false),
	_hasBBox(false)
{	
	_parents = getParentsAsSet(ge);
	recvSight(ge);	
}

Entity::Entity(const std::string &id) :
	_id(id),
	_stamp(-1.0),
	_visible(true),
	_container(NULL),
	_position(0., 0., 0.),
	_velocity(0., 0., 0.),
	_orientation(1.0, 0., 0., 0.),
	_inUpdate(false)
{
	;
}

Entity::~Entity()
{
	Connection *con = World::Instance()->getConnection();
	
	while (!_localDispatchers.empty()) {
		con->removeDispatcherByPath("op:sight:op",_localDispatchers.front());
		_localDispatchers.pop_front();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

bool Entity::hasProperty(const std::string &p) const
{
    PropertyMap::const_iterator pi =
	_properties.find(p);
    return (pi != _properties.end());
}

const Atlas::Message::Object& Entity::getProperty(const std::string &p)
{
    PropertyMap::iterator pi = _properties.find(p);
    if (pi == _properties.end())
	throw InvalidOperation("Unknown property " + p);

    return pi->second->getValue();
}

void Entity::observeProperty(const std::string &nm, 
    SigC::Slot1<void, const Atlas::Message::Object&> slot)
{
    PropertyMap::iterator pi = _properties.find(nm);
    if (pi == _properties.end())
	throw InvalidOperation("Unknown property " + nm);
    
    pi->second->Set.connect(slot);
}

WFMath::Point<3> Entity::getPosition() const
{
	return _position;
}

WFMath::Vector<3> Entity::getVelocity() const
{
	return _velocity;
}

WFMath::AxisBox<3> Entity::getBBox() const
{
	return _bbox;
}

WFMath::Quaternion Entity::getOrientation() const
{
    return _orientation;
}

TypeInfo* Entity::getType() const
{
    assert(!_parents.empty());
    return Eris::TypeInfo::find(*_parents.begin());
}

void Entity::setVisible(bool vis)
{
	bool wasVisible = _visible;
	_visible = vis;
  
	// recurse onz children
	for (EntityArray::iterator E=_members.begin();E!=_members.end();++E)
		(*E)->setVisible(vis);
	
	if (!wasVisible && _visible) {
		//move back to actice
		World::Instance()->markVisible(this);
	}
	
	if (wasVisible && !_visible) {
		// move to the cache ... keep around for 'a while'
		World::Instance()->markInvisible(this);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Entity::recvSight(const Atlas::Objects::Entity::GameEntity &ge)
{    
    beginUpdate();
    
    Atlas::Message::Object::MapType amp = ge.AsObject().AsMap();
    for (Atlas::Message::Object::MapType::iterator A = amp.begin(); A!=amp.end(); ++A) {
	if (A->first == "id") continue;
	setProperty(A->first, A->second);
    }

    endUpdate();
}

void Entity::recvMove(const Atlas::Objects::Operation::Move &mv)
{	
    beginUpdate();
    
    const Atlas::Message::Object::MapType &args = 
	mv.GetArgs().front().AsMap();
    for (Atlas::Message::Object::MapType::const_iterator A = args.begin(); 
	    A != args.end(); ++A) {
	setProperty(A->first, A->second);
    }

    endUpdate();	// emit 'Changed' too
    handleMove();
}

void Entity::recvSet(const Atlas::Objects::Operation::Set &st)
{
    const Atlas::Message::Object::MapType &attrs = st.GetArgs().front().AsMap();
    beginUpdate();
    // blast through the map, setting each property
    for (Atlas::Message::Object::MapType::const_iterator ai = attrs.begin(); ai != attrs.end(); ++ai) {
	if (ai->first=="id") continue;	// important
	setProperty(ai->first, ai->second);
    }
    endUpdate();
}

void Entity::recvSound(const Atlas::Objects::Operation::Sound &/*snd*/)
{
	// FIXME - decide upon a clever way to handle these. 
}

void Entity::recvTalk(const Atlas::Objects::Operation::Talk &tk)
{
	const Atlas::Message::Object &obj = getArg(tk, 0);
	Atlas::Message::Object::MapType::const_iterator m = obj.AsMap().find("say");
	if (m == obj.AsMap().end())
		throw IllegalObject(tk, "No sound object in arg 0");
	
	handleTalk(m->second.AsString());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// default implementions of handle simple emit the corresponding signal

void Entity::handleTalk(const std::string &msg)
{
	Say.emit(msg);
}

void Entity::handleMove()
{
	Moved.emit(_position);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FIXME
// this really needs a global switch from strings to integer Atoms to be effective;
// once that is done the if ( == "") tests can be replaced with a nice efficent switch

void Entity::setProperty(const std::string &s, const Atlas::Message::Object &val)
{
    /* we allow mapping of attributes to different internal values; use this with
    caution */
    std::string mapped(s);
	
    if (s == "name")
	_name = val.AsString();
    else if (s == "stamp")
	_stamp = val.AsFloat();
    else if (s == "loc") {
	std::string loc = val.AsString();
	setContainerById(loc);
    } else if (s == "pos") {
	WFMath::Point<3> pos;
	pos.fromAtlas(val);
	setPosition(pos);
    } else if (s == "velocity")
	_velocity.fromAtlas(val);
    else if (s == "orientation")
	_orientation.fromAtlas(val);
    else if (s == "face") {
	mapped = "orientation";
	WFMath::Point<3> face(val); // Should this be Point<3> or Vector<3>?
	// build the quaternion from rotation about z axis
	_orientation.rotation(2, atan2(face[1], face[0]));	
    } else if (s == "description")
	_description = val.AsString();
    else if (s == "bbox") {
	_bbox.fromAtlas(val);
        _hasBBox = true;
    } else if (s == "contains") {
	setContents(val.AsList());
    }
    
    PropertyMap::iterator P=_properties.find(mapped);
    if (P == _properties.end()) {
	P = _properties.insert(P, 
	    PropertyMap::value_type(mapped, new Property())
	);
    }
    
    P->second->setValue(val);
    
    if (_inUpdate) {
	// add to modified set
	_modified.insert(mapped);
    } else {
	// generate a Changed single with a temporary set
	StringSet ms;
	ms.insert(mapped);
	Changed.emit(ms);
    }
}


void Entity::beginUpdate()
{
    if (_inUpdate)
	throw InvalidOperation("Entity::beingUpdate called inside of property update");
    assert(_modified.empty());
    _inUpdate = true;
}

void Entity::endUpdate()
{
    if (!_inUpdate)
	throw InvalidOperation("Entity::endUpdate called outside of property update");
    _inUpdate = false;
    Changed.emit(_modified);
    _modified.clear();
}

void Entity::innerOpFromSlot(Dispatcher *s)
{
	// build the unique dispathcer name
	std::string efnm = "from." + _id;
	
	// check if the dispatcher already exists
	Connection *con = World::Instance()->getConnection(); 
	Dispatcher *ds = con->getDispatcherByPath("op:sight:op"),
		*efd = ds->getSubdispatch(efnm);
	
	if (!efd) {
		efd = new OpFromDispatcher(efnm, _id);
		_localDispatchers.push_back(efnm);
		ds->addSubdispatch(efd);
	}
	
	// attach the operation dispatcher
	efd->addSubdispatch(s);
}

void Entity::innerOpToSlot(Dispatcher *s)
{
	// build the unique dispathcer name
	std::string etnm = "to." + _id;
	
	// check if the dispatcher already exists
	Connection *con = World::Instance()->getConnection(); 
	Dispatcher *ds = con->getDispatcherByPath("op:sight:op"),
		*etd = ds->getSubdispatch(etnm);
	
	if (!etd) {
		etd = new OpToDispatcher(etnm, _id);
		_localDispatchers.push_back(etnm);
		ds->addSubdispatch(etd);
	}
	
	// attach the operation dispatcher
	etd->addSubdispatch(s);
}

void Entity::setPosition(const WFMath::Point<3>& pt)
{
    _position = pt;
}

void Entity::setContents(const Atlas::Message::Object::ListType &contents)
{
    for (Atlas::Message::Object::ListType::const_iterator
	    C=contents.begin(); C!=contents.end();++C) {
	// check we have it; if yes, ensure it's installed. if not, it will be attached when
	// it arrives.
	Entity *con = World::Instance()->lookup(C->AsString());
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
		Entity *ncr = World::Instance()->lookup(id);
		if (ncr) {
			ncr->addMember(this);
			setContainer(ncr);
		} else {
			// setup a redispatch once we have the container
			// sythesises a set, with just the container.
			Atlas::Objects::Operation::Set setc = 
				Atlas::Objects::Operation::Set::Instantiate();
				
			Atlas::Message::Object::MapType args;
			args["loc"] = id;
			setc.SetArgs(Atlas::Message::Object::ListType(1,args));
			setc.SetTo(_id);
			
			Atlas::Objects::Operation::Sight ssc =
				Atlas::Objects::Operation::Sight::Instantiate();
			ssc.SetArgs(Atlas::Message::Object::ListType(1, setc.AsObject()));
			ssc.SetTo(World::Instance()->getFocusedEntityID());
			ssc.SetSerialno(getNewSerialno());
			
			// if we received sets/creates/sights in rapid sucession, this can happen, and is
			// very, very bad
			std::string setid("set_container_" + _id);
			Connection::Instance()->removeIfDispatcherByPath(
				"op:ig:sight:entity", setid);
			
			new WaitForDispatch(ssc, "op:ig:sight:entity", 
				new IdDispatcher(setid, id) );
		}
	} // of new container is valid
    }	
	
    if (id.empty()) {	// id of "" implies local root
	Eris::log(LOG_DEBUG, "got entity with empty container, assuming it's the world");
	World::Instance()->setRootEntity(this);	
    }
}

#ifdef HAVE_CPPUNIT
class EntityTest : public TestCase
{
public:
    
    void runTest()
    {
	Entity testA("foo");	
	
	Atlas::Objects::Operation::Move mv;
	mv.SetLocation("foo");
	Coord v1(2.0, 3.0, -4.0);
	mv.SetVelocity(v1.asObject());
	mv.SetAttr("face", 120.0);
	
	testA.recvMove(mv);
	
	CPPUNIT_ASSERT("foo" == test.getContainer());
	CPPUNIT_ASSERT(120.0 == test.getOrientation().asEuler().z);
    }

private:
};
#endif

} // of namespace 
