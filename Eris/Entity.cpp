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
	_container(NULL),
	_bbox(0., 0., 0.),
	_position(ge.GetPos())
{	
	_parents = getParentsAsSet(ge);
	//cerr << "created " << _id << " (" << *(_parents.begin()) << ")" << endl;
	
	recvSight(ge);	
}

Entity::Entity(const std::string &id) :
	_id(id),
	_stamp(-1.0),
	_container(NULL),
	_bbox(0., 0., 0.),
	_position(0., 0., 0.)
{
	;
}

/*
Entity::Entity(const Entity *e)
{
	
	
}
*/

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
	
	std::string containerId = ge.GetAttr("loc").AsString();
	if ( !_container || (containerId != _container->getID()) ) {
		
		if (_container)
			_container->rmvMember(this);
		
		Entity *ncr = World::Instance()->lookup(containerId);
		if (ncr) {
			ncr->addMember(this);
			Recontainered.emit(_container, ncr);
			_container = ncr;
		} else
			throw InvalidOperation("FIXME - support delayed dispatch on parents");
	}

	// bounding box
	if (ge.HasAttr("bounding_box"))
		_bbox = BBox( ge.GetAttr("bounding_box") );
	
	// copy *every* attribute through
	const Atlas::Message::Object::MapType &amp = ge.AsMap();
	for (Atlas::Message::Object::MapType::const_iterator a = amp.begin(); a!=amp.end(); ++a)
		_properties[a->first] = a->second;
	
	// emit the hook
	handleChanged();
	//Changed.emit();
}

void Entity::recvMove(const Atlas::Objects::Operation::Move &mv)
{	
	_stamp = mv.GetSeconds();
	setUnsync("stamp");
	
	setProperty("loc", getArg(mv, "loc"));
	setProperty("pos", getArg(mv, "pos"));
	
/*	
	SetProperty("velocity", GetArg(mv, "velocity"));
*/	
	handleMove();
	//Moved.emit(_position);
}

void Entity::recvSet(const Atlas::Objects::Operation::Set &st)
{
	const Atlas::Message::Object::MapType &attrs = st.GetArgs().front().AsMap();
	
	// blast through the map, setting each property
	for (Atlas::Message::Object::MapType::const_iterator ai = attrs.begin(); ai != attrs.end(); ++ai)
		setProperty(ai->first, ai->second);
	
	handleChanged();
	//Changed.emit();
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
	//Say.emit(m->second.AsString());
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
		if (!_container || (_container->getID() != loc)) {
			
			if (_container)
				_container->rmvMember(this);	
			Entity *nc = World::Instance()->lookup(loc);
			if (nc) {
				nc->addMember(this);
				Recontainered.emit(_container, nc);
				_container = nc;
			} else {
				// setup a redispatch once we have the container
				string nm = "set_container_" /*+ set.GetSerialno() */ ;
		
				// sythesies a new set, with just the container.
				Atlas::Objects::Operation::Set setc = 
					Atlas::Objects::Operation::Set::Instantiate();
				
				Atlas::Message::Object::MapType args;
				args["loc"] = loc;
				setc.SetArgs(Atlas::Message::Object::ListType(1,args));
				
				new WaitForDispatch(setc, "op:ig:sight:entity", new IdDispatcher(nm, loc) );
			}
		}
		
	} else if (s == "pos") {
		_position = Coord(val);
		// Moved.emit(_position);
	}
	
	// check for a signal bound to property 's'; emit if found
	/*	
	PropSignalDict::iterator psi = _propSignals.find(s);
	if (psi != _propSignals.end())
		psi->second->emit(val);
	*/
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
