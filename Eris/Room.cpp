#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/Room.h>
#include <Eris/Lobby.h>
#include <Eris/Connection.h>
#include <Eris/Person.h>
#include <Eris/Log.h>
#include <Eris/Exceptions.h>
#include <Eris/Player.h>

#include <sigc++/object_slot.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/RootEntity.h>

#include <cassert>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
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
	
    Root speech;
    speech->setAttr("say", tk);
    speech->setAttr("loc", m_roomId);
	
    Talk t;
    t->setArgs1(speech);
    t->setTo(m_roomId);
    t->setFrom(m_lobby->getAccount()->getID());
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
	
    Root emote;
    emote->setId("emote");
    emote->setAttr("loc", m_roomId);
    emote->setAttr("description", em);
    
    im->setArgs1(emote);
    im->setTo(m_roomId);
    im->setFrom(m_lobby->getAccount()->getID());
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
    part->setFrom(m_lobby->getAccount()->getID());
    part->setSerialno(getNewSerialno());
    
    Root args;
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
    cr->setFrom(m_lobby->getAccount()->getID());
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
    if (op->getTo() != m_lobby->getAccount()->getID())
    {
        error() << "Room recived op TO account " << op->getTo() << ", not the account ID";
        return IGNORED;
    }

    const std::vector<Root>& args = op->getArgs();

    if (op->instanceOf(SIGHT_NO))
    {
        assert(!args.empty());
        RootEntity ent = smart_dynamic_cast<RootEntity>(args.front());
            
        if (ent.isValid() && (ent->getId() == m_roomId))
        {
            sight(ent);
            return HANDLED;
        }
        
        Imaginary img = smart_dynamic_cast<Imaginary>(args.front());
        if (img.isValid())
        {
            handleSightImaginary(img);
            return HANDLED;
        }
    }

    if (op->instanceOf(APPEARANCE_NO))
    {
        for (unsigned int A=0; A < args.size(); ++A)
            appearance(args[A]->getId());

        return HANDLED;
    }

    if (op->instanceOf(DISAPPEARANCE_NO))
    {
        for (unsigned int A=0; A < args.size(); ++A)
            disappearance(args[A]->getId());
        
        return HANDLED;
    }

    if (op->instanceOf(SOUND_NO))
    {
        Talk tk = smart_dynamic_cast<Talk>(args.front());
        if (tk.isValid())
        {
            handleSoundTalk(tk);
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

void Room::handleSoundTalk(const Talk &tk)
{
    IdPersonMap::const_iterator P = m_members.find(tk->getFrom());
    if (P == m_members.end())
    {
        error() << "room " << m_roomId << " got sound(talk) from non-member account";
        return;
    }
    
    if (P->second == NULL)
        return; // consume but ignore till we have sight
        
    const std::vector<Root>& args = tk->getArgs();
    if (args.empty())
    {
        warning() << "room " << m_roomId << " recieved sound(talk) with no args";
        return;
    }
    
    if (!args.front()->hasAttr("say"))
        return;
    
    std::string description = args.front()->getAttr("say").asString();
    Speech.emit(this, P->second, description);
}

void Room::handleSightImaginary(const Imaginary& im)
{
    IdPersonMap::const_iterator P = m_members.find(im->getFrom());
    if (P == m_members.end())
    {
        error() << "room " << m_roomId << " got sight(imaginary) from non-member account";
        return;
    }
    
     if (P->second == NULL)
        return; // consume but ignore till we have sight

    const std::vector<Root>& args = im->getArgs();
    if (args.empty())
    {
        warning() << "room " << m_roomId << " recieved sight(imaginary) with no args";
        return;
    }
    
    if (!args.front()->hasAttr("description"))
        return;
    
    std::string description = args.front()->getAttr("description").asString();
    Emote.emit(this, P->second, description);
}

#pragma mark -
// room membership updates

void Room::appearance(const std::string& personId)
{
    IdPersonMap::iterator P = m_members.find(personId);
    if (P != m_members.end())
    {
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
        m_lobby->SightPerson.connect(SigC::slot(*this, &Room::notifyPersonSight));
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
    if (P == m_members.end())
        return; // this could happen if we got appear and disappear before the SIGHT
    
    if (P->second != NULL)
    {
        if (P->second != p)
        {
            error() << "duplicate Person objects exist for account " << P->first;
            return;
        }
    }
        
    if (m_entered)
        Appearance.emit(this, p);
    else
        checkEntry();
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
