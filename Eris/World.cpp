#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <sigc++/object.h>
#include <sigc++/object_slot.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/signal_system.h>
#else
#include <sigc++/signal.h>
#endif
#include <algorithm>
#include <float.h>

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

#include <Eris/World.h>
#include <Eris/Entity.h>
#include <Eris/Connection.h>
#include <Eris/Log.h>
#include <Eris/Factory.h>
#include <Eris/Utils.h>
#include <Eris/Wait.h>
#include <Eris/InvisibleEntityCache.h>
#include <Eris/Player.h>
#include <Eris/Avatar.h>

#include <Eris/ClassDispatcher.h>
#include <Eris/EncapDispatcher.h>
#include <Eris/OpDispatcher.h>
#include <Eris/TypeDispatcher.h>
#include <Eris/IdDispatcher.h>

//using namespace Atlas::Objects;
typedef Atlas::Message::Element::ListType AtlasListType;
typedef Atlas::Message::Element::MapType AtlasMapType;

namespace Eris {
	
World* World::_theWorld = NULL;
	
World::World(Player *p, Connection *c) :
	_con(c),
	_player(p),
	_root(NULL),
	_focused(NULL),
	_avatar(0)
{
	assert(_con);
	assert(_player);
	
	// store the instance pointer  ( "new Entity(..)" already needs this !)
	// To match the old singleton interface, this should be the most recently
	// created instance.
	_theWorld = this;
	_ieCache = new InvisibleEntityCache(10000, 600000);	// 10 sec buckets, 10 minute lifetime

	// setup network callbacks
	_con->Connected.connect(SigC::slot(*this, &World::netConnect));
	// check for auto-firing, etc
	//if (_con->getStatus() == BaseConnection::CONNECTED)
	//	netConnect();
}
	
World::~World()
{
	Destroyed.emit();

	// cascading delete
	if (_root)
		delete _root;
	delete _ieCache;

	if(_theWorld == this)
		_theWorld = 0;
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
		return NULL;
	} else
		return ei->second;
}

void World::tick()
{
	_ieCache->flush();
}

EntityPtr World::getRootEntity()
{
	if (!_root) {
		Eris::log(LOG_WARNING, "called World::getRootEntity before initial world entry, returning NULL");
		return NULL;
	}
	
	return _root;
}

void World::setFocusedEntity(EntityPtr f)
{
	assert(f);
	_focused = f;
	_characterID = f->getID();
	look(NULL);
}

const std::string& World::getFocusedEntityID()
{
	if (_focused)
		return _focused->getID();
	else // so we can call this *early*
		return _characterID;
}

void World::look(const std::string &id)
{
	// if connection is down, do nothing
	// FIXME - buffer these ? a somewhat risky strategy...
	if (!_con->isConnected()) return;
		
	Atlas::Objects::Operation::Look look =
		Atlas::Objects::Operation::Look::Instantiate();

	if (!id.empty()) {
		Atlas::Message::Element::MapType what;
		what["id"] = id;
     		Atlas::Message::Element::ListType args(1,what);
     		look.setArgs(args);
	    /* note we don't set TO here, this is intentional; it's
	    unecessary with a compliant server, and complicates routing in
	    certain cases (editor-avatars)*/
	}
	
	look.setSerialno(getNewSerialno());
	look.setFrom(_characterID);
	// transmit!
	_con->send(look);
}


EntityPtr World::create(const Atlas::Objects::Entity::GameEntity &ge)
{
	Entity *e = NULL;
	std::string id = ge.getId();
	
	EntityIDMap::iterator I = _lookup.find(id);
	if (I != _lookup.end())
		throw InvalidOperation("called World::create() for entity that already exists");
	
	// test factories
	// note that since the default comparisom (for ints) is less<>, we use a reverse
	// iterator here and get the desired result (higher priorty values are tested first)
	for (FactoryMap::reverse_iterator fi = _efactories.rbegin(); 
		fi != _efactories.rend(); ++fi) {
		if (fi->second->accept(ge, this)) {
			// call the factory
			e = fi->second->instantiate(ge, this);
			break;
		}
	}
	
	// use generic
	if (!e) {
		e = new Entity(ge, this);
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

Avatar*
World::createAvatar(long refno, const std::string& id)
{
    if(_avatar)
	throw InvalidOperation("World already has an Avatar");
	
    _avatar = new Avatar(this, refno, id);
    _player->LogoutComplete.connect(SigC::slot(*_avatar, &Avatar::slotLogout));
    _con->Disconnected.connect(SigC::slot(*_avatar, &Avatar::slotDisconnect));

    return _avatar;
}

///////////////////////////////////////////////////////////////////////////////////////////

// Need to get an id without the ':' character
static std::string get_ig_dispatch_id(const std::string &name)
{
  std::string id = "ig_";
  std::string::const_iterator I;

  for(I = name.begin(); I != name.end(); ++I) {
    switch(*I) {
      case ':':
        id += "#!";
        break;
      case '#':
        id += "##";
        break;
      default:
        id += *I;
        break;
    }
  }

  return id;
}

void  World::registerCallbacks()
{
	// build the root of the in-game dispatch tree; notably a selector on the character ID
	// the filter out anything not in-game; followed by 'sight' and 'sound' decoders, with
	// child entity / operation selectors on those.

	// we will need this to be able to manipulate dispatchers later
	_igID = get_ig_dispatch_id(_characterID);
	
	Dispatcher *d = _con->getDispatcherByPath("op");
	Dispatcher *igd = d->addSubdispatch(new OpToDispatcher(_igID, _characterID));
	
	Dispatcher *igclass = igd->addSubdispatch(ClassDispatcher::newAnonymous(_con));
	Dispatcher *sightd = igclass->addSubdispatch(new EncapDispatcher("sight"), "sight");
	
	Dispatcher *ed = sightd->addSubdispatch(new TypeDispatcher("entity", "object"));
	// sight of game-entities (rather important this!)
	ed->addSubdispatch(new SignalDispatcher2<Atlas::Objects::Operation::Sight,
		Atlas::Objects::Entity::GameEntity>("world", 
		SigC::slot(*this, &World::recvSightObject)
	));
	
	Dispatcher *od = sightd->addSubdispatch(new TypeDispatcher("op", "op"));
	Dispatcher *opclass = od->addSubdispatch(ClassDispatcher::newAnonymous(_con));
	
	// sight of create operations; this is 2-level decoder; becuase we have SIGHT encapsulating
	// the CREATE which encapsulates the entity.
	//Dispatcher *cr = od->addSubdispatch(new ClassDispatcher("create", "create"));
	Dispatcher *cr = opclass->addSubdispatch(new EncapDispatcher("create"), "create");
	cr->addSubdispatch( new SignalDispatcher2<Atlas::Objects::Operation::Create,
		Atlas::Objects::Entity::GameEntity>("world", 
		SigC::slot(*this, &World::recvSightCreate)
	));
	
	// sight of delete operations
	opclass->addSubdispatch( new SignalDispatcher<Atlas::Objects::Operation::Delete>("delete", 
		SigC::slot(*this, &World::recvSightDelete)),
		"delete"
	);
	
	// sight of set operations
	opclass->addSubdispatch( new SignalDispatcher<Atlas::Objects::Operation::Set>("set", 
		SigC::slot(*this, &World::recvSightSet)),
		"set"
	);
	
	// sight of move operations
	opclass->addSubdispatch( new SignalDispatcher<Atlas::Objects::Operation::Move>("move", 
		SigC::slot(*this, &World::recvSightMove)),
		"move"
	);
	
	Dispatcher *soundd = igclass->addSubdispatch(new EncapDispatcher("sound"), "sound");
	soundd = soundd->addSubdispatch(ClassDispatcher::newAnonymous(_con));
	soundd->addSubdispatch( new SignalDispatcher2<Atlas::Objects::Operation::Sound,
		Atlas::Objects::Operation::Talk>("world",
		SigC::slot(*this, &World::recvSoundTalk)),
		"talk"
	);
	
// appearance . disappearance : note these inherit from sight so need to be careful
	igclass->addSubdispatch(new SignalDispatcher<Atlas::Objects::Operation::Disappearance>("disappear",
		SigC::slot(*this, &World::recvDisappear)),
		"disappearance"
	);
	
	igclass->addSubdispatch(new SignalDispatcher<Atlas::Objects::Operation::Appearance>("appear",
		SigC::slot(*this, &World::recvAppear)),
		"appearance"
	);
}

////////////////////////////////////////////////////////////////////////////////////////////
// callbacks

void World::recvSightObject(const Atlas::Objects::Operation::Sight &sight,
	const Atlas::Objects::Entity::GameEntity &ent)
{
	std::string id = ent.getId();
	
	// FIXME - work around a bug in Stage RIM Chat; the entity ID is not set, so
	// Atlas-C++ defaults to 'game_entity'. The work around is to used to 'FROM'
	// attribute of the SIGHT op. An additional complication is that objects are
	// passed in as const referenced, so we need a nasty cast to make this go through
	
	if (id == "game_entity") {
		Eris::log(LOG_WARNING, "ID not set on entity");
		id = sight.getFrom();
		(const_cast<Atlas::Objects::Entity::GameEntity&>(ent)).setId(id);
	}
	
	EntityIDMap::iterator ei = _lookup.find(id);
	if (ei == _lookup.end()) {
		// new entity, invoke the factories
		
		Entity *e = create(ent);
		assert(e);
		
		if (e->getID() == _characterID) {
			_focused = e;
			
			// signal entry into the world; we delay this (from the INFO)
			// until the character and the root entity are both valid
			if (_initialEntry && _root) {
				Entered.emit(e);
				_initialEntry = false;	
				Eris::log(LOG_VERBOSE, "did IG entry after sight of character");
			}
		}
	
		StringSet::iterator I = _pendingInitialSight.find(e->getID());
		if (I != _pendingInitialSight.end()) {
			Appearance.emit(e);
			e->setVisible(true);
			_pendingInitialSight.erase(I);
		} else
			// if it wasn't pending, we are either aggressivley pulling or got a disappearance
			// before the SIGHT came through .. hence this
			e->setVisible(false);
		
	} else {
		if (getParentsAsSet(ent) != ei->second->getInherits()) {
			// FIXME - handle mutations
			
			StringSet speced = getParentsAsSet(ent);
			for (StringSet::iterator I=speced.begin(); I!=speced.end();++I)
				std::cerr << *I << std::endl;
			
			std::cerr << "----" << std::endl;
			
			StringSet actual = ei->second->getInherits();
			for (StringSet::iterator I=actual.begin(); I!=actual.end();++I)
				std::cerr << *I << std::endl;
			
			throw InvalidOperation("Mutations currently unsupported!");
		} else
			ei->second->recvSight(ent);
	}
	
	// check for new contained entities
	const Atlas::Message::Element::ListType& contained = ent.getContains();
	for (Atlas::Message::Element::ListType::const_iterator ci = contained.begin();
			ci != contained.end(); ++ci)
	{
		// just pull it in
		lookup(ci->asString());
	}
}

void World::recvSightCreate(const Atlas::Objects::Operation::Create &cr,
	const Atlas::Objects::Entity::GameEntity &ge)
{	
	Atlas::Objects::Operation::Sight st = 
		Atlas::Objects::Operation::Sight::Instantiate();
	
	st.setFrom(cr.getFrom());
	_pendingInitialSight.insert(ge.getId());
    
	recvSightObject(st, ge);
}

void World::recvSightDelete(const Atlas::Objects::Operation::Delete &del)
{
	std::string id = getArg(del, "id").asString();	
	EntityIDMap::iterator ei = _lookup.find(id);	
	
	if (ei == _lookup.end()) {
		Eris::log(LOG_ERROR, "Unknown entity %s from DELETE", id.c_str());
		return;
	}
	
	Entity *victim = ei->second,
	    *vcontainer = victim->getContainer();
	WFMath::Vector<3> victimOffset = victim->getPosition() - WFMath::Point<3>(0,0,0);
	
	// fix container elements
	for (unsigned int E=0; E<victim->getNumMembers();E++) {
	    Entity *child = victim->getMember(E);
	    // protected method, is this okay?
	    child->setContainer(vcontainer);
	    child->setPosition(child->getPosition() + victimOffset);
	}
	
	// emit some general deletion signal? (flush will if ever requried)
	flush(victim);
	delete victim;
}

void World::recvSightMove(const Atlas::Objects::Operation::Move &mv)
{
    if (!hasArg(mv, "id")) {
	Eris::log(LOG_ERROR, "received SIGHT(MOVE) with no ID argument");
	return;
    }
    
    std::string id = getArg(mv, "id").asString();
    Entity *e = lookup(id);
	
    if (!e) {
		if ( isPendingInitialSight(mv.getFrom()) ) {
			// this is 'safe'; we discard the move becuase we will
			// get the position with the LOOK, and there is no useful
			// display before that point.
		} else
			throw UnknownEntity("Unknown entity at move", mv.getFrom());
	} else
		e->recvMove(mv);
}

void World::recvSoundTalk(const Atlas::Objects::Operation::Sound &snd,
	const Atlas::Objects::Operation::Talk &tk)
{
	Entity *e = lookup(snd.getFrom());
	if (!e) {
		if ( isPendingInitialSight(snd.getFrom()) ) {
			
			// FIXME - ensure name uniqueness
			std::string nm = "talk_";
			int sno = snd.getSerialno();
			for(; sno != 0; sno >>= 4) {
			    nm += ('a' + (sno & 0xf));
			}
			std::cout << "TALK: " << snd.getSerialno() << " " << nm << std::endl << std::flush;
			new WaitForDispatch(snd, 
				"op:" + _igID + ":sight:entity", 
				new IdDispatcher(nm, snd.getFrom()),
				_con
			); 
		} else
			throw UnknownEntity("Unknown entity at sound", snd.getFrom());
	
	} else
		e->recvTalk(tk);
}

void World::recvInfoCharacter(const Atlas::Objects::Operation::Info &/*ifo*/,
	const Atlas::Objects::Entity::GameEntity &character)
{
	log(LOG_DEBUG, "Setting up World for character %s", character.getName().c_str());

	_characterID = character.getId();
	
	// now the _characterID is valid, register all other callbacks (which are
	// parented to an OpTo selector to we only get In-Game (IG) ops.
	registerCallbacks();
	
	_initialEntry = true;
	
	// get the character, to glue things up
	lookup(_characterID);
	
	CharacterSuccess.emit();
	
	// get the local root for us
	look("");
}

void World::recvAppear(const Atlas::Objects::Operation::Appearance &ap)
{
	const AtlasListType &args = ap.getArgs();
	for (AtlasListType::const_iterator A=args.begin();A!=args.end();++A) {
		const AtlasMapType &app = A->asMap();
		AtlasMapType::const_iterator V(app.find("id"));
		std::string id(V->second.asString());
		Entity *e = lookup(id);
		if (!e) continue;
	
		// wunderbar, we have it already
		e->setVisible(true);
		Appearance.emit(e);
		
		float stamp(FLT_MAX);
		// but it might be out of data - check the stamp [NULL indicates no stamping]
		if ((V = app.find("stamp")) != app.end())
			stamp = V->second.asFloat();
		
		if (stamp > e->getStamp()) {
			Eris::log(LOG_DEBUG, "Issuing re-look for existing APPEARED entity %s with new stamp",
				id.c_str());
			look(id);
		}
	}
}

void World::recvDisappear(const Atlas::Objects::Operation::Disappearance &ds)
{
	const AtlasListType &args = ds.getArgs();
	for (AtlasListType::const_iterator A=args.begin();A!=args.end();++A) {
		const AtlasMapType &app = A->asMap();
		AtlasMapType::const_iterator V(app.find("id"));
		std::string id(V->second.asString());
		Entity *e = lookup(id);
		if (e) {
			e->setVisible(false);
			Disappearance.emit(e);
		} else
			// suppress the Appearance signal
			_pendingInitialSight.erase(id);
	}
}

void World::recvSightSet(const Atlas::Objects::Operation::Set &set)
{
	Eris::log(LOG_DEBUG, "processing IG sight(set)");
	
	Entity *e = lookup(set.getTo());
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
	std::string id = getArg(lk, "id").asString();
}

void World::netConnect()
{
	// update things
	look("");
}

void World::setRootEntity(Entity* rt)
{
	assert(rt);
	assert(rt->getContainer() == NULL);
	
	// note the order here is important, so slots can call getRootEntity to get the
	// previous value for rootEntity
	RootEntityChanged.emit(rt);
	_root = rt;
	
	if (_initialEntry) {
		Entity *character = lookup(_characterID);
		if (character) {
			Entered.emit(character);
			_initialEntry = false;
			Eris::log(LOG_VERBOSE, "did IG entry after setRootEntity");
		} // else still waiting for the character
	}
}

void World::markInvisible(Entity *e)
{
    if (e != _root)
	_ieCache->add(e);
	// remove from lookup? not for now
}

void World::markVisible(Entity *e)
{
    if (e != _root)
	_ieCache->remove(e);
}

/** callback from the entity cache, when it decided an entity can be completely deleted */
void World::flush(Entity *e)
{
    assert(e);
    Entity * r = e->getContainer();
    if (r != NULL) {
        r->rmvMember(e);
    }
    EntityIDMap::iterator E = _lookup.find(e->getID());
    assert(E != _lookup.end());
    _lookup.erase(E);
}

} // of namespace Eris
