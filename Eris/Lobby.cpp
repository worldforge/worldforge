#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Lobby.h>
#include <Eris/Connection.h>
#include <Eris/Log.h>
#include <Eris/Person.h>
#include <Eris/Account.h>
#include <Eris/Redispatch.h>

#include <sigc++/slot.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Anonymous.h>

#include <algorithm> 
#include <cassert>

#pragma warning(disable: 4068)  //unknown pragma

typedef Atlas::Objects::Entity::Account AtlasAccount; 
using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::smart_static_cast;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

namespace Eris {

class OOGRouter : public Router
{
public:
    OOGRouter(Lobby* l) :
        m_lobby(l),
        m_anonymousLookSerialno(0)
    {;}
        
    void setAnonymousLookSerialno(int serial)
    {
        m_anonymousLookSerialno = serial;
    }
    
protected:
    RouterResult handleOperation(const RootOperation& op)
    {
        const std::vector<Root>& args = op->getArgs();

        if (op->instanceOf(APPEARANCE_NO)) {
            for (unsigned int A=0; A < args.size(); ++A)
                m_lobby->recvAppearance(args[A]);

            return HANDLED;
        }

        if (op->instanceOf(DISAPPEARANCE_NO)) {
            for (unsigned int A=0; A < args.size(); ++A)
                m_lobby->recvDisappearance(args[A]);
            
            return HANDLED;
        }

        // note it's important we match exactly on sight here, and not derived ops
        // like appearance and disappearance
        if (op->getClassNo() == Atlas::Objects::Operation::SIGHT_NO) {
            assert(!args.empty());
            AtlasAccount acc = smart_dynamic_cast<AtlasAccount>(args.front());
            if (acc.isValid()) {
                m_lobby->sightPerson(acc);
                return HANDLED;
            }
            
            if (op->getRefno() == m_anonymousLookSerialno) {
                RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
                m_lobby->recvInitialSight(ent);
                return HANDLED;
            }
            
            Imaginary im = smart_dynamic_cast<Imaginary>(args.front());
            if (im.isValid())
                return m_lobby->recvImaginary(im);
        } // of sight op
        
        Sound sound = smart_dynamic_cast<Sound>(op);
        if (sound.isValid())
        {
            assert(!args.empty());
            Talk talk = smart_dynamic_cast<Talk>(args.front());
            if (talk.isValid())
                return m_lobby->recvTalk(talk);
        }
        
        return IGNORED;
    }
    
private:
    TypeService* typeService()
    {
        // none of these can ever be NULL, honest
        return m_lobby->getConnection()->getTypeService();
    }

    Lobby* m_lobby;
    int m_anonymousLookSerialno;
};


#pragma mark -

Lobby::Lobby(Account* a) :
    Room(this, std::string()),
    m_account(a)
{
    m_router = new OOGRouter(this);
    assert(a);
    
    if (m_account->isLoggedIn())
        onLoggedIn();
    else
        m_account->LoginSuccess.connect(sigc::mem_fun(this, &Lobby::onLoggedIn));
        
    m_account->LogoutComplete.connect(sigc::mem_fun(this, &Lobby::onLogout));
}
	
Lobby::~Lobby()
{
    for (IdRoomMap::const_iterator R = m_rooms.begin(); R != m_rooms.end(); ++R) {
        if (R->second == this) continue; // that would really be bad
        delete R->second;
    }
    
    for (IdPersonMap::const_iterator P = m_people.begin(); P != m_people.end(); ++P)
        delete P->second;
        
    delete m_router;
}

void Lobby::look(const std::string &id)
{
    if (!m_account->isLoggedIn()) {
        error() << "Lobby trying look while not logged in";
        return;
    }
    
    Look look;
    look->setFrom(m_account->getId());
    look->setSerialno(getNewSerialno());
    
    if (!id.empty()) {
        Anonymous what;
        what->setId(id);
        look->setArgs1(what);
    }
    
    if (id.empty())
        m_router->setAnonymousLookSerialno(look->getSerialno());
    
    getConnection()->send(look);
}

Room* Lobby::join(const std::string& roomId)
{
    if (!m_account->isLoggedIn())
    {
        error() << "Lobby trying join while not logged in";
        return NULL;
    }
	
    Anonymous what;
    what->setAttr("loc", roomId);
    what->setAttr("mode", "join");

    Move join;
    join->setFrom(m_account->getId());
    join->setSerialno(getNewSerialno());
    join->setArgs1(what);
    getConnection()->send(join);
	
    IdRoomMap::iterator R = m_rooms.find(roomId);
    if (R == m_rooms.end()) {
        Room *nr = new Room(this, roomId);
        R = m_rooms.insert(R, IdRoomMap::value_type(roomId, nr));
    }
	
    return R->second;
}

Connection* Lobby::getConnection() const
{
    return m_account->getConnection();
}

Person* Lobby::getPerson(const std::string &acc)
{
    IdPersonMap::iterator P = m_people.find(acc);
    if (P == m_people.end())
    {
        look(acc);
        // create a NULL entry (indicates we are doing the look)
        P = m_people.insert(P, IdPersonMap::value_type(acc, NULL));
    }
    
    return P->second;
}

Room* Lobby::getRoom(const std::string &id)
{
    IdRoomMap::iterator R = m_rooms.find(id);
    if (R == m_rooms.end()) {
        error() << "called getRoom with unknown ID " << id;
        return NULL;
    }
	
    return R->second;
}

#pragma mark -

void Lobby::sightPerson(const AtlasAccount &ac)
{
    IdPersonMap::iterator P = m_people.find(ac->getId());
    if (P == m_people.end()) {
        error() << "got un-requested sight of person " << ac->getId();
        return;
    }
	
    if (P->second)
        P->second->sight(ac);
    else {
        // install the new Person object
        P->second = new Person(this, ac);
    }
    
    // emit the signal; this lets rooms waiting on this player's info update
    SightPerson.emit(P->second);
}

void Lobby::recvInitialSight(const RootEntity& ent)
{
    // we only hit this path when we get the anonymous LOOK response
    // for the Lobby. We need to do the work normally done in Room's ctor
    if (!m_roomId.empty()) return;

    m_roomId = ent->getId();
    m_rooms[m_roomId] = this;
    m_account->getConnection()->registerRouterForFrom(this, m_roomId);
    Room::sight(ent);
}

/** helper to buffer operations when waiting on sight of a person. Used by
private chat in the first instance, and potentially more later. */
class SightPersonRedispatch : public Redispatch

{
public:
    SightPersonRedispatch(Connection* con, const std::string& pid, const Root& obj) :
        Redispatch(con, obj),
        m_person(pid)
    {;}
    
    void onSightPerson(Person* p)
    {
        if ( p->getAccount() == m_person) post();
    }
private:
    std::string m_person;
};

Router::RouterResult Lobby::recvTalk(const Talk& tk)
{
    if (tk->isDefaultFrom()) {
        return IGNORED;
    }
    IdPersonMap::const_iterator P = m_people.find(tk->getFrom());
    if ((P == m_people.end()) || (P->second == NULL)) {
        getPerson(tk->getFrom()); // force a LOOK if necessary
        debug() << "creating sight-person-redispatch for " << tk->getFrom();
        
        Sight sight;
        sight->setArgs1(tk);
        sight->setTo(getAccount()->getId());
        
        SightPersonRedispatch *spr = new SightPersonRedispatch(getConnection(), tk->getFrom(), sight);
        SightPerson.connect(sigc::mem_fun(spr, &SightPersonRedispatch::onSightPerson));
        
        return WILL_REDISPATCH;
    }
    
    const std::vector<Root>& args = tk->getArgs();
    if (args.empty() || !args.front()->hasAttr("say")) {
        error() << "recieved sound(talk) with no / bad args";
        return HANDLED;
    }

    std::string speech = args.front()->getAttr("say").asString();
    
    if (args.front()->hasAttr("loc")) {
        std::string loc = args.front()->getAttr("loc").asString();
        IdRoomMap::const_iterator room = m_rooms.find(loc);
        
        if (room != m_rooms.end()) {
            room->second->handleSoundTalk(P->second, speech);
        } else {
            warning() << "lobby got sound(talk) with unknown loc: " << loc;
        }
    } else {
        // no location, hence assume it's one-to-one chat
        PrivateTalk.emit(P->second, speech);
    }

    return HANDLED;
}

void Lobby::recvAppearance(const Atlas::Objects::Root& obj)
{
    if (!obj->hasAttr("loc")) {
        error() << "lobby got appearance arg without loc: " << obj;
        return;
    }
    
    std::string loc = obj->getAttr("loc").asString();
    IdRoomMap::const_iterator room = m_rooms.find(loc);
        
    if (room != m_rooms.end()) {
        room->second->appearance(obj->getId());
    } else
        warning() << "lobby got appearance with unknown loc: " << loc;
}

Router::RouterResult Lobby::recvImaginary(const Imaginary& im)
{
    const std::vector<Root>& args = im->getArgs();
    if (args.empty() || !args.front()->hasAttr("description")) {
        warning() << "recieved sight(imaginary) with no/bad args: " << im;
        return HANDLED;
    }
        
    std::string description = args.front()->getAttr("description").asString();
    
    if (im->isDefaultFrom()) {
        return IGNORED;
    }
    IdPersonMap::const_iterator P = m_people.find(im->getFrom());
    if ((P == m_people.end()) || (P->second == NULL)) {
        getPerson(im->getFrom()); // force a LOOK if necessary
        debug() << "creating sight-person-redispatch for " << im->getFrom();
        
        Sight sight;
        sight->setArgs1(im);
        sight->setTo(getAccount()->getId());
        
        SightPersonRedispatch *spr = new SightPersonRedispatch(getConnection(), im->getFrom(), sight);
        SightPerson.connect(sigc::mem_fun(spr, &SightPersonRedispatch::onSightPerson));
        
        return WILL_REDISPATCH;
    }

    if (args.front()->hasAttr("loc")) {
        std::string loc = args.front()->getAttr("loc").asString();
        IdRoomMap::const_iterator room = m_rooms.find(loc);
        
        if (room != m_rooms.end()) {
            room->second->handleEmote(P->second, description);
        } else
            error() << "lobby got sight(imaginary) with unknown loc: " << loc;
    } else
        warning() << "recieved imaginary with no loc set:" << im;
        
    return HANDLED;
}


void Lobby::recvDisappearance(const Atlas::Objects::Root& obj)
{
    if (!obj->hasAttr("loc")) {
        error() << "lobby got disappearance arg without loc: " << obj;
        return;
    }
    
    std::string loc = obj->getAttr("loc").asString();
    IdRoomMap::const_iterator room = m_rooms.find(loc);
        
    if (room != m_rooms.end()) {
        room->second->disappearance(obj->getId());
    } else
        error() << "lobby got disappearance with unknown loc: " << loc;
}

/*
void Lobby::processRoomCreate(const Atlas::Objects::Operation::Create &cr,
	const Atlas::Objects::Entity::RootEntity &ent)
{
    log(LOG_DEBUG, "recieved sight of room creation");
	
    PendingCreateMap::iterator P = _pendingCreate.find(cr.getSerialno());
    if (P != _pendingCreate.end()) {
	// it was requested locally, so we already have the Room object
	P->second->_id = ent.getId(); // set the ID
	P->second->setup();		// now we can call setup safely
	P->second->sight(ent);	// finally slam the data in
	_pendingCreate.erase(P);	// get rid of the request
    }
    
    // find the containing room and update it's subrooms
    // note that we may not even know about it's containing room either!
    std::string containingRoom = ent.getAttr("loc").asString();
    if (_roomDict.find(containingRoom) == _roomDict.end())
	return; // we can't see it, so we don't care [we'll get the rooms anyway if we ever join the containing room]
    
    Room *container = _roomDict[containingRoom];
    container->_subrooms.insert(ent.getId());	// jam it in
    
    StringSet strset;
    strset.insert("rooms");
    container->Changed.emit(strset);
}
*/

#pragma mark -
// signal handlers for various things

void Lobby::onLoggedIn()
{
    assert(m_account->isLoggedIn());
    getConnection()->registerRouterForTo(m_router, m_account->getId());
    look(""); // do initial anonymous look
}

void Lobby::onLogout(bool clean)
{
    getConnection()->unregisterRouterForTo(m_router, m_account->getId());
}

} // of namespace
