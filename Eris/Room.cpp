#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/Room.h>
#include <Eris/Lobby.h>
#include <Eris/Connection.h>
#include <Eris/Person.h>
#include <Eris/Log.h>
#include <Eris/Exceptions.h>
#include <Eris/Account.h>

#include <sigc++/slot.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

#pragma warning(disable: 4068)  //unknown pragma

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris
{
	
Room::Room(Lobby *l, const std::string& id) :
    m_roomId(id),
    m_entered(false),
    m_lobby(l)
{
    if (!id.empty())
        m_lobby->getConnection()->registerRouterForFrom(this, id);
}

Room::~Room()
{
    if (!m_roomId.empty())
        m_lobby->getConnection()->unregisterRouterForFrom(this, m_roomId);
}

#pragma mark -
// public command-issue wrappers

void Room::say(const std::string &tk)
{
    if (!m_lobby->getConnection()->isConnected())
    {
        error() << "talking in room " << m_roomId << ", but connection is down";
        return;
    }
	
    Anonymous speech;
    speech->setAttr("say", tk);
    speech->setAttr("loc", m_roomId);
	
    Talk t;
    t->setArgs1(speech);
    t->setTo(m_roomId);
    t->setFrom(m_lobby->getAccount()->getId());
    t->setSerialno(getNewSerialno());
	
    m_lobby->getConnection()->send(t);
}

void Room::emote(const std::string &em)
{
    if (!m_lobby->getConnection()->isConnected())
    {
        error() << "emoting in room " << m_roomId << ", but connection is down";
        return;
    }
	
    Imaginary im;
	
    Anonymous emote;
    emote->setId("emote");
    emote->setAttr("loc", m_roomId);
    emote->setAttr("description", em);
    
    im->setArgs1(emote);
    im->setTo(m_roomId);
    im->setFrom(m_lobby->getAccount()->getId());
    im->setSerialno(getNewSerialno());
	
    m_lobby->getConnection()->send(im);
}

void Room::leave()
{
    if (!m_lobby->getConnection()->isConnected())
    {
        error() << "leaving room " << m_roomId << ", but connection is down";
        return;
    }

    Move part;
    part->setFrom(m_lobby->getAccount()->getId());
    part->setSerialno(getNewSerialno());
    
    Anonymous args;
    args->setAttr("loc", m_roomId);
    args->setAttr("mode", "part");
    part->setArgs1(args);

    m_lobby->getConnection()->send(part);
}

Room* Room::createRoom(const std::string &name)
{
    if (!m_lobby->getConnection()->isConnected())
    {
        error() << "creating room in room  " << m_roomId << ", but connection is down";
        return NULL;
    }

    
    Create cr;
    cr->setFrom(m_lobby->getAccount()->getId());
    cr->setTo(m_roomId);
    cr->setSerialno(getNewSerialno());
    
    RootEntity room;
    room->setName(name);
    room->setParents(StringList(1, "room"));
    
    cr->setArgs1(room);
    m_lobby->getConnection()->send(cr);
    
    return NULL;
}

Person* Room::getPersonByUID(const std::string& uid)
{
    return m_lobby->getPerson(uid);
}

std::vector<Person*> Room::getPeople() const
{
    std::vector<Person*> people;
    
    for (IdPersonMap::const_iterator P=m_members.begin(); P != m_members.end(); ++P)
    {    
        if (P->second)
            people.push_back(P->second);
    }
    
    return people;
}

#pragma mark -

Router::RouterResult Room::handleOperation(const RootOperation& op)
{
    if (op->getTo() != m_lobby->getAccount()->getId()) {
        error() << "Room recived op TO account " << op->getTo() << ", not the account ID";
        return IGNORED;
    }

    const std::vector<Root>& args = op->getArgs();
    
    if (op->instanceOf(APPEARANCE_NO)) {
        for (unsigned int A=0; A < args.size(); ++A)
            appearance(args[A]->getId());

        return HANDLED;
    }

    if (op->instanceOf(DISAPPEARANCE_NO)) {
        for (unsigned int A=0; A < args.size(); ++A)
            disappearance(args[A]->getId());
        
        return HANDLED;
    }

    if (op->instanceOf(SIGHT_NO)) {
        assert(!args.empty());
        RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
            
        if (ent.isValid() && (ent->getId() == m_roomId)) {
            sight(ent);
            return HANDLED;
        }
    }

    return IGNORED;
}

void Room::sight(const RootEntity &room)
{
    if (m_entered)
        warning() << "got SIGHT of entered room " << m_roomId;
        
    m_name = room->getName();
    if (room->hasAttr("topic"))
        m_topic = room->getAttr("topic").asString();
    
    m_lobby->SightPerson.connect(sigc::mem_fun(this, &Room::notifyPersonSight));
    
    if (room->hasAttr("people"))
    {
        const Atlas::Message::ListType& people = room->getAttr("people").asList();
        for (Atlas::Message::ListType::const_iterator P=people.begin(); P!=people.end(); ++P)
            appearance(P->asString());
    }
    
    checkEntry();
    
    if (room->hasAttr("rooms"))
    {
        const Atlas::Message::ListType& rooms = room->getAttr("rooms").asList();
        for (Atlas::Message::ListType::const_iterator R=rooms.begin(); R!=rooms.end(); ++R)
        {
            m_subrooms.push_back(new Room(m_lobby, R->asString()));
        }
    }
}

void Room::handleSoundTalk(Person* p, const std::string& speech)
{
    assert(p);
    
    if (m_members.count(p->getAccount()) == 0) {
        error() << "room " << m_roomId << " got sound(talk) from non-member account";
        return;
    }
    
    Speech.emit(this, p, speech);
}

void Room::handleEmote(Person* p, const std::string& description)
{
    assert(p);
    
    if (m_members.count(p->getAccount()) == 0) {
        error() << "room " << m_roomId << " got sight(imaginary) from non-member account";
        return;
    }

    Emote.emit(this, p, description);
}

#pragma mark -
// room membership updates

void Room::appearance(const std::string& personId)
{
    IdPersonMap::iterator P = m_members.find(personId);
    if (P != m_members.end()) {
        error() << "duplicate appearance of person " << personId << " in room " << m_roomId;
        return;
    }
    
    Person* person = m_lobby->getPerson(personId);
    if (person)
    {
        m_members[personId] = person;
        if (m_entered)
            Appearance.emit(this, person);
    } else {
        m_members[personId] = NULL; // we know the person is here, but that's all
        // we'll find out more when we get the SightPerson signal from Lobby
    }
}

void Room::disappearance(const std::string& personId)
{
    IdPersonMap::iterator P = m_members.find(personId);
    if (P == m_members.end())
    {
        error() << "during disappearance, person " << personId << " not found in room " << m_roomId;
        return;
    }
    
    if (P->second) // don't emit if never got sight
        Disappearance.emit(this, P->second);
    
    m_members.erase(P);
}

void Room::notifyPersonSight(Person *p)
{
    assert(p);
    IdPersonMap::iterator P = m_members.find(p->getAccount());
    // for the moment, all rooms get spammed with sights of people, to avoid
    // the need for a counting / disconnect from SightPerson scheme
    if (P == m_members.end()) return;
    
    if (P->second == NULL) {
        P->second = p;
        
        if (m_entered)
            Appearance.emit(this, p);
        else
            checkEntry();
    } else {
        // fairly meaningless case, but I'm paranoid
        // could fire a 'changed' signal here, eg if they renamed?
        assert (P->second == p);
    }
}

void Room::checkEntry()
{
    assert(m_entered == false);
    
    bool anyPending = false;
    for (IdPersonMap::const_iterator P = m_members.begin(); P != m_members.end(); ++P)
        if (P->second == NULL) anyPending = true;
       
    if (!anyPending)
    {
        Entered.emit(this);
        m_entered = true;
    }
}

} // of Eris namespace
