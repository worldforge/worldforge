#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <algorithm>
#include <set>

#include <Atlas/Objects/Entity/RootEntity.h>
#include <Atlas/Objects/Entity/GameEntity.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Sight.h>

#include "Entity.h"
#include "World.h"
#include "Connection.h"
#include "Utils.h"
#include "Wait.h"

#include "OpDispatcher.h"
#include "IdDispatcher.h"

namespace Eris {

Entity::Entity(const Atlas::Objects::Entity::GameEntity &ge) :
	_id(ge.GetId()),
	_stamp(-1.0),
	_visible(true),
	_container(NULL),
	_position(ge.GetPos()),
	_velocity(ge.GetVelocity())
{	
	_parents = getParentsAsSet(ge);
	recvSight(ge);	
}

Entity::Entity(const std::string &id) :
	_id(id),
	_stamp(-1.0),
	_container(NULL),
	_position(0., 0., 0.)
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
	_members.push_back(e);
	AddedMember.emit(e);
	e->setContainer(this);
}

void Entity::rmvMember(Entity *e)
{
	EntityArray::iterator ei = std::find(_members.begin(), _members.end(), e);
	if (ei == _members.end())
		throw InvalidOperation("Unknown member to remove");
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
	Atlas::Message::Object::MapType::const_iterator pi =
		_properties.find(p);
	return (pi != _properties.end());
}

const Atlas::Message::Object& Entity::getProperty(const std::string &p)
{
	Atlas::Message::Object::MapType::const_iterator pi =
		_properties.find(p);
	if (pi == _properties.end())
		throw InvalidOperation("Unknown property " + p);
	
	// FIXME
	// re-synchronise if necessary; note this is assumed to
	// keep the iterator valid, which is fine for maps, but NOT for hash_maps
	// so you (yes you!) the person optimising this to hell and back, 
	// watch out.
	
	if (!isFullySynched()) {
		StringSet rs;
		rs.insert(p);
		resync(rs);	
		assert(rs.empty());	// sweet mother of moses, I hope this never happens...
	}
	
	return pi->second;
}

Coord Entity::getPosition() const
{
	return _position;
}

BBox Entity::getBBox() const
{
	return _bbox;
}

void Entity::setVisible(bool vis)
{
	bool wasVisible = _visible;
	_visible = vis;
  
	// recurse on children
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
	if (ge.HasAttr("stamp"))
		_stamp = ge.GetAttr("stamp").AsFloat();
	
	_name = ge.GetName();
	
	if (ge.HasAttr("description"))
		_description = ge.GetAttr("description").AsString();
	
	// parenting
	const Atlas::Message::Object::ListType& parents = ge.GetParents();
	//attrs.erase("parents");
	
	// FIXME - deal with multiple parents
	std::string type = parents.front().AsString();
	
	std::string containerId = ge.GetLoc();
	setContainerById(ge.GetLoc());

	_position = ge.GetPos();
	_velocity = ge.GetVelocity();
	
	// bounding box
	if (ge.HasAttr("bbox"))
		_bbox = BBox( ge.GetAttr("bbox") );
	
	if (ge.HasAttr("bmedian")) {
		// convert acorn/cyphesis bbox to our kind
		Coord med(ge.GetAttr("bmedian"));
		_bbox.offset(med.x, med.y, med.z);
	}
	
	// copy *every* attribute through
	const Atlas::Message::Object::MapType &amp = ge.AsMap();
	for (Atlas::Message::Object::MapType::const_iterator a = amp.begin(); a!=amp.end(); ++a)
		_properties[a->first] = a->second;
	
	// emit the hook
	handleChanged();
}

void Entity::recvMove(const Atlas::Objects::Operation::Move &mv)
{	
	_stamp = mv.GetSeconds();
	setUnsync("stamp");
	
	setProperty("loc", getArg(mv, "loc"));
	setProperty("pos", getArg(mv, "pos"));
	//setProperty("velocity", getArg(mv, "velocity"));

	handleMove();
}

void Entity::recvSet(const Atlas::Objects::Operation::Set &st)
{
	const Atlas::Message::Object::MapType &attrs = st.GetArgs().front().AsMap();
	
	// blast through the map, setting each property
	for (Atlas::Message::Object::MapType::const_iterator ai = attrs.begin(); ai != attrs.end(); ++ai)
		setProperty(ai->first, ai->second);
	
	handleChanged();
}

void Entity::recvSound(const Atlas::Objects::Operation::Sound &snd)
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

void Entity::handleTalk(const string &msg)
{
	Say.emit(this, msg);
}

void Entity::handleChanged()
{
	Changed.emit(this);
}

void Entity::handleMove()
{
	Moved.emit(this, _position);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FIXME
// this really needs a global switch from strings to integer Atoms to be effective;
// once that is done the if ( == "") tests can be replaced with a nice efficent switch

void Entity::setProperty(const std::string &s, const Atlas::Message::Object &val)
{
	//if (!hasProperty(s))
	//	throw UnknownProperty(s, "on entity " + _id);
	
	// none of the following cases call Unsync(), becuase we always copy the value
	// to the property map here
	_properties[s] = val;
	
	if (s == "stamp") {
		_stamp = val.AsFloat();
	} else if (s == "loc") {
		string loc = val.AsString();
		setContainerById(loc);
	} else if (s == "pos") {
		_position = Coord(val);
	} else if (s == "velocity") {
		_velocity = Coord(val);
	}
	
	// check for a signal bound to property 's'; emit if found
	/*	
	PropSignalDict::iterator psi = _propSignals.find(s);
	if (psi != _propSignals.end())
		psi->second->emit(val);
	*/
	
	handleChanged();
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

void Entity::setContainerById(const std::string &id)
{
	if ( !_container || (id != _container->getID()) ) {
		
		if (_container) {
			_container->rmvMember(this);
			_container = NULL;
		}
		
		// have to check this, becuase changes in what the client can see
		// may cause the client's root to change
		if (!id.empty()) {
		
			Entity *ncr = World::Instance()->lookup(id);
			if (ncr) {
				ncr->addMember(this);
				Recontainered.emit(_container, ncr);
				_container = ncr;
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
		
		}
	}	
	
	if (id.empty())	// id of "" implies local root
		World::Instance()->setRootEntity(this);	
}

/*
Note to madmen, hackers  and Irishmen : this code is not exactly elegant, and
neither is it fast. However, it does get the job done correctly for the relevant
attributes, which is all that matters right now. At present, I do not expect Resyncs()
to be that common, even assuming worst case scripting requirements (eg a Python
radar requiring a Resync() on position 4 times a second). If Resyncs are going to
be common, and dispersed, then the whole mechansim needs to be rethought,
switching from O(ln N) operations to O(k) ones [i.e hashes and bitsets] 

You have been warned.
*/

void Entity::resync(StringSet &attrs)
{
	StringSet::iterator a;
	
	a = attrs.find("stamp");
	if (a != attrs.end()) {
		_properties["stamp"] = _stamp;
		attrs.erase(a);
	}
	
	a = attrs.find("pos");
	if (a != attrs.end()) {
		_properties["pos"] = _position.asObject();
		attrs.erase(a);
	}
	
	a = attrs.find("loc");
	if (a != attrs.end()) {
		_properties["loc"] = _container->getID();
		attrs.erase(a);
	}
	
	a = attrs.find("description");
	if (a != attrs.end()) {
		_properties["description"] = _description;
		attrs.erase(a);
	}
	
	a = attrs.find("name");
	if (a != attrs.end()) {
		_properties["name"] = _name;
		attrs.erase(a);
	}
}

}; // of namespace 
