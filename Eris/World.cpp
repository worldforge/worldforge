#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <sigc++/signal_system.h>
#include <algorithm>

// various atlas headers we need
#include <Atlas/Bridge.h>
#include <Atlas/EncoderBase.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>


#include <Atlas/Objects/Entity/RootEntity.h>
#include <Atlas/Objects/Entity/GameEntity.h>

#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Info.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Move.h>

#include "World.h"
#include "Entity.h"
#include "Connection.h"
#include "Factory.h"
#include "Utils.h"
#include "Wait.h"

#include "ClassDispatcher.h"
#include "EncapDispatcher.h"
#include "OpDispatcher.h"
#include "TypeDispatcher.h"
#include "IdDispatcher.h"

//using namespace Atlas::Objects;

namespace Eris {
	
World* World::_theWorld = NULL;
	
World::World(Player *p, Connection *c) :
	_con(c),
	_player(p)
{
	assert(_con);
	assert(_player);
	
	// create the root entity
	Atlas::Objects::Entity::GameEntity rootDef = 
		Atlas::Objects::Entity::GameEntity::Instantiate();
	
	rootDef.SetId("__eris_root__");
	_root = new Entity(rootDef);

// info operation
	Dispatcher *d = _con->getDispatcherByPath("op:info");
	assert(d);
	
	d->addSubdispatch( new SignalDispatcher<Atlas::Objects::Operation::Info>("world", 
		SigC::slot(this, &World::recvInfoOp)
	));
	
	// store the instance pointer
	_theWorld = this;
	
	// setup network callbacks
	_con->Connected.connect(SigC::slot(this, &World::netConnect));
}
	
World::~World()
{
	// cascading delete
	delete _root;
}

Entity* World::lookup(const std::string &id)
{
	EntityIDMap::iterator ei = _lookup.find(id);
	if (ei == _lookup.end()) {
		// don't duplicate!
		if (isPendingInitialSight(id)) return NULL;
		
		// FIXME - this allows spurious 'pulls', and also returns NULL
		// better solution would be to create an Entity temporarily, and add to
		// a 'partial' list; when a sight arrives for that entity, need to merge
		// the data in.
		look(id);
		
		_pendingInitialSight.insert(id);
		
		//return Entity(id);
		return NULL;
	} else
		return ei->second;
}


EntityPtr World::getRootEntity()
{
	assert(_root);
	return _root;
}

/*
Atlas::Bridge* World::GetEncoder() const
{
	if (!_con || (_con->GetStatus() != Connection::CONNECTED))
		throw InvalidOperation("Connection not ready to access encoder");
	// fast path
	return _con->GetEncoder();
}
*/
void World::setFocusedEntity(EntityPtr f)
{
	assert(f);
	_characterID = f->getID();
	look(NULL);
}

void World::look(const std::string &id)
{
	// if connection is down, do nothing
	// FIXME - buffer these ? a somewhat risky strategy...
	if (!_con->isConnected()) return;
	
	//cerr << "doing LOOK on " << id << endl;
	
	Atlas::Objects::Operation::Look look =
		Atlas::Objects::Operation::Look::Instantiate();

	if (!id.empty()) {
		Atlas::Message::Object::MapType what;
		what["id"] = id;
     		Atlas::Message::Object::ListType args(1,what);
     		look.SetArgs(args);
		
		// unecessary according to spec, but Stage needs it <sigh>
		look.SetTo(id);
	}
	
	look.SetSerialno(getNewSerialno());
	look.SetFrom(_characterID);
	// transmit!
	_con->send(look);
}


EntityPtr World::create(const Atlas::Objects::Entity::GameEntity &ge)
{
	Entity *e = NULL;
	string id = ge.GetId();
	
	// test factories
	// note that since the default comparisom (for ints) is less<>, we use a reverse
	// iterator here and get the desired result (higher priorty values are tested first)
	for (FactoryMap::reverse_iterator fi = _efactories.rbegin(); 
		fi != _efactories.rend(); ++fi) {
		if (fi->second->accept(ge)) {
			// call the factory
			e = fi->second->instantiate(ge);
			break;
		}
	}
	
	// use generic
	if (!e) {
		e = new Entity(ge);
	}
	
	// bind into the database
	_lookup[e->getID()] = e;
	
	// and emit the signal
	EntityCreate.emit(e);
	
	return e;
}

void World::registerFactory(Factory *f, unsigned int priority)
{
	if (!f)
		throw InvalidOperation("NULL factory passed to World::registerFactory");
	
	_efactories.insert(_efactories.begin(),
		FactoryMap::value_type(priority, f)
	);
}

void World::unregisterFactory(Factory *f)
{
	if (!f)
		throw InvalidOperation("NULL factory passed to World::unregisterFactory");
	
	FactoryMap::iterator F;
	for (F = _efactories.begin(); F != _efactories.end(); ++F) {
		if (F->second == f) {
			_efactories.erase(F);
			return;
		}
	}
	
	throw InvalidOperation("Factory not registered in World::unregisterFactory");
}

World* World::Instance()
{
	return _theWorld;
}

///////////////////////////////////////////////////////////////////////////////////////////

void  World::registerCallbacks()
{
	// build the root of the in-game dispatch tree; notably a selector on the character ID
	// the filter out anything not in-game; followed by 'sight' and 'sound' decoders, with
	// child entity / operation selectors on those.
	
	Dispatcher *d = _con->getDispatcherByPath("op");
	Dispatcher *igd = d->addSubdispatch(new OpToDispatcher("ig", _characterID));
	
	// the sight dispatcher, which de-encapsulates the object within
	Dispatcher *sightd = igd->addSubdispatch(new EncapDispatcher("sight", "sight"));
	
	Dispatcher *ed = sightd->addSubdispatch(new TypeDispatcher("entity", "object"));
	Dispatcher *od = sightd->addSubdispatch(new TypeDispatcher("op", "op"));
	
	// sight of game-entities (rather important this!)
	
	// FIXME  -  need to make inheritance work here

	ed->addSubdispatch(new SignalDispatcher2<Atlas::Objects::Operation::Sight,
		Atlas::Objects::Entity::GameEntity>("world", 
		SigC::slot(this, &World::recvSightObject)
	));
	
	// sight of create operations; this is 2-level decoder; becuase we have SIGHT encapsulating
	// the CREATE which encapsulats the entity.
	//Dispatcher *cr = od->addSubdispatch(new ClassDispatcher("create", "create"));
	
	Dispatcher *cr = od->addSubdispatch(new EncapDispatcher("create", "create"));
	cr->addSubdispatch( new SignalDispatcher2<Atlas::Objects::Operation::Create,
		Atlas::Objects::Entity::GameEntity>("world", 
		SigC::slot(this, &World::recvSightCreate)
	));
	
	// sight of delete operations
	Dispatcher *del = od->addSubdispatch(new ClassDispatcher("delete", "delete"));
	del->addSubdispatch( new SignalDispatcher<Atlas::Objects::Operation::Delete>("world", 
		SigC::slot(this, &World::recvSightDelete)
	));
	
	// sight of set operations
	Dispatcher *set = od->addSubdispatch(new ClassDispatcher("set", "set"));
	set->addSubdispatch( new SignalDispatcher<Atlas::Objects::Operation::Set>("world", 
		SigC::slot(this, &World::recvSightSet)
	));
	
	// sight of move oprations
	Dispatcher *mv = od->addSubdispatch(new ClassDispatcher("move", "move"));
	mv->addSubdispatch( new SignalDispatcher<Atlas::Objects::Operation::Move>("world", 
		SigC::slot(this, &World::recvSightMove)
	));
	
	Dispatcher *soundd = igd->addSubdispatch(new EncapDispatcher("sound", "sound"));
	
	Dispatcher *tkd = soundd->addSubdispatch(new ClassDispatcher("talk", "talk"));
	tkd->addSubdispatch( new SignalDispatcher2<Atlas::Objects::Operation::Sound,
		Atlas::Objects::Operation::Talk>("world",
		SigC::slot(this, &World::recvSoundTalk)
	));
}

////////////////////////////////////////////////////////////////////////////////////////////
// callbacks

void World::recvSightObject(const Atlas::Objects::Operation::Sight &sight,
	const Atlas::Objects::Entity::GameEntity &ent)
{
	string id = ent.GetId();
	
	// FIXME - work around a bug in Stage RIM Chat; the entity ID is not set, so
	// Atlas-C++ defaults to 'game_entity'. The work around is to used to 'FROM'
	// attribute of the SIGHT op. An additional complication is that objects are
	// passed in as const referenced, so we need a nasty cast to make this go through
	
	if (id == "game_entity") {
		cerr << "WARNING: ID not set on entity" << endl;
		id = sight.GetFrom();
		(const_cast<Atlas::Objects::Entity::GameEntity&>(ent)).SetId(id);
	}
	
	EntityIDMap::iterator ei = _lookup.find(id);
	if (ei == _lookup.end()) {
		// new entity, invoke the factories
		
		Entity *e = create(ent);
		assert(e);
		
		// signal entry into the world; we delay this (from the INFO)
		// until the character goes valid. Potentially, we could delay this
		// even futher, waiting until the full inital set has been expanded
		if (_initialEntry && (e->getID() == _characterID)) {
			Entered.emit(e);
			_initialEntry = false;	
		}
		
		StringSet::iterator I = _pendingInitialSight.find(e->getID());
		if (I != _pendingInitialSight.end()) {
			// FIXME - this effectively generates synthetic appearance ops,
			// which is necessry for clients, but should be the server's problem.
			Appearance.emit(e);
			_pendingInitialSight.erase(I);
		}
		
	} else {
		if (getParentsAsSet(ent) != ei->second->getInherits()) {
			// FIXME - handle mutations
			
			StringSet speced = getParentsAsSet(ent);
			for (StringSet::iterator I=speced.begin(); I!=speced.end();++I)
				cerr << *I << endl;
			
			cerr << "----" << endl;
			
			StringSet actual = ei->second->getInherits();
			for (StringSet::iterator I=actual.begin(); I!=actual.end();++I)
				cerr << *I << endl;
			
			throw InvalidOperation("Mutations currently unsupported!");
		} else
			ei->second->recvSight(ent);
	}
	
	// check for new children (oh! how exciting!)
	const Atlas::Message::Object::ListType& children = ent.GetContains();
	for (Atlas::Message::Object::ListType::const_iterator ci = children.begin();
			ci != children.end(); ++ci)
	{
		// just pull it in
		lookup(ci->AsString());
	}
}

void World::recvSightCreate(const Atlas::Objects::Operation::Create &cr,
	const Atlas::Objects::Entity::GameEntity &ge)
{	
	create(ge);
}

void World::recvSightDelete(const Atlas::Objects::Operation::Delete &del)
{
	string id = getArg(del, "id").AsString();	
	EntityIDMap::iterator ei = _lookup.find(id);	
	
	if (ei == _lookup.end())
		throw UnknownEntity("Unknown entity at delete", id);
	// emit some general deletion signal?
	
	delete ei->second;
	_lookup.erase(ei);
}

void World::recvSightMove(const Atlas::Objects::Operation::Move &mv)
{
	Entity *e = lookup(mv.GetFrom());
	if (!e) {
		if ( isPendingInitialSight(mv.GetFrom()) ) {
			// this is 'safe'; we discard the move becuase we will
			// get the position with the LOOK, and there is no useful
			// display before that point.
		} else
			throw UnknownEntity("Unknown entity at move", mv.GetFrom());
	} else
		e->recvMove(mv);
}

void World::recvSoundTalk(const Atlas::Objects::Operation::Sound &snd,
	const Atlas::Objects::Operation::Talk &tk)
{
	cerr << "got sound talk" << endl;
	
	Entity *e = lookup(snd.GetFrom());
	if (!e) {
		if ( isPendingInitialSight(snd.GetFrom()) ) {
			
			// FIXME - ensure name uniqueness
			string nm = "talk_" /* + snd.GetSerialno() */ ;
			new WaitForDispatch(snd, 
				"op:ig:sight:entity", 
				new IdDispatcher(nm, snd.GetFrom())
			); 
		} else
			throw UnknownEntity("Unknown entity at sound", snd.GetFrom());
	
	} else
		e->recvTalk(tk);
}

void World::recvInfoOp(const Atlas::Objects::Operation::Info &ifo)
{
	Atlas::Message::Object::MapType character = 
		ifo.GetArgs().front().AsMap();
	       
	_characterID = character["id"].AsString();
	
	// now the _characterID is valid, register all other callbacks (which are
	// parented to an OpTo selector to we only get In-Game (IG) ops.
	registerCallbacks();
	
	_initialEntry = true;
	
	// get the character, to glue things up
	lookup(_characterID);
	
	// get the local root for us
	look("");
	
	// this prevents us recieving any more info ops
	_con->removeDispatcherByPath("op:info", "world");
}

void World::recvAppear(const Atlas::Objects::Operation::Appearance &ap)
{
	string id = getArg(ap, "id").AsString();
	Entity *e = lookup(id);
	
	if (e) {
		// wunderbar, we have it already
		Appearance.emit(e);
		
		// but it might be out of data - check the stamp [NULL indicates no stamping]
		float stamp = getArg(ap, "stamp").AsFloat();
		if ((stamp == 0) || (stamp > e->getStamp()))
			look(id);
	}
}

void World::recvDisappear(const Atlas::Objects::Operation::Disappearance &ds)
{
	string id = getArg(ds, "id").AsString();
	Entity *e = lookup(id);
	
	if (e)
		Disappearance.emit(e);
	else {
		// suppress the Appearance signal
		_pendingInitialSight.erase(id);
	}
}

void World::recvSightSet(const Atlas::Objects::Operation::Set &set)
{
	Entity *e = lookup(set.GetTo());
	if (!e) {
		// no need to re-dispatch; we'll get the set value anyway as part
		// of the SIGHT
		return;
	}
	
	e->recvSet(set);
}

void World::recvErrorLook(const Atlas::Objects::Operation::Look &lk)
{
	// probably, this means the entity ID is bollocks
	string id = getArg(lk, "id").AsString();
}

void World::netConnect()
{
	cerr << "got (re)connection from network" << endl;
	// update things
	look("");
}

}; // of namespace Eris
