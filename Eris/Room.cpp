#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/Room.h>
#include <Eris/Lobby.h>
#include <Eris/Connection.h>
#include <Eris/Utils.h>
#include <Eris/Person.h>
#include <Eris/Log.h>
#include <Eris/Exceptions.h>

#include <sigc++/object_slot.h>

#include <cassert>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;

namespace Eris
{
	
Room::Room(Lobby *l, const std::string& id) :
    m_roomId(id),
    m_entered(false),
    m_lobby(l)
{
    if (!id.empty())
        m_lobby->getConnection()->registerRouterForFrom(id, this);
}

Room::~Room()
{
    if (!m_roomId.empty())
        m_lobby->getConnection()->unregisterRouterForFrom(id, this);
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
	
	Atlas::Message::Element::MapType speech;
	speech["say"] = tk;
	speech["loc"] = _id;
	
    Talk t;
	t.setArgs(Atlas::Message::Element::ListType(1, speech));
    t->setTo(m_roomId);
    t->setFrom(m_lobby->getAccountID());
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
	
	Atlas::Message::Element::MapType emote;
	emote["id"] = "emote";
	emote["description"] = em;
	emote["loc"] = _id;
	
	im.setArgs(Atlas::Message::Element::ListType(1, emote));
	im.setTo(_id);
	im.setFrom(_lobby->getAccountID());
	im.setSerialno(getNewSerialno());
	
    m_lobby->getConnection()->send(im);
}

void Room::leave()
{
    if (!m_lobby->getConnection()->isConnected())
    {
        error() << "leaving room " << m_roomId << ", but connection is down";
        return;
    }

    
	Atlas::Objects::Operation::Move part;
	part.setFrom(_lobby->getAccountID());
	part.setSerialno(getNewSerialno());
	
	Message::Element::MapType args;
	args["loc"] = _id;
	args["mode"] = "part";
	part.setArgs(Message::Element::ListType(1, args));
	
	c->send(part);
	// FIXME - confirm the part somehow?
	_parted = true;
}

Room* Room::createRoom(const std::string &name)
{
    if (!m_lobby->getConnection()->isConnected())
    {
        error() << "creating room in room  " << m_roomId << ", but connection is down";
        return;
    }

    
    Atlas::Objects::Operation::Create cr;
    cr.setFrom(_lobby->getAccountID());
    cr.setTo(_id);
    int serial = getNewSerialno();
    cr.setSerialno(serial);
    
    Message::Element::MapType args;
    args["parents"] = Message::Element::ListType(1, "room");
    args["name"] = name;
    
    cr.setArgs(Message::Element::ListType(1, args));
    c->send(cr);
    
    Room *r = new Room(_lobby);
    // this lets the lobby do stitch up as necessary
    _lobby->addPendingCreate(r, serial);
    r->_name = name;	// I'm setting this since we have it already, and it might
    // help someone identify the room prior to the ID going valid.
    return r;
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

RouterResult Room::handleOperation(const RootOperation& op)
{
    if (op->getTo() != m_account->getAccountId())
    {
        error() << "Room recived op TO account " << op->getTo() << ", not the account ID";
        return IGNORED;
    }

    const std::vector<Root>& args = op.getArgs();

    if (op->instanceOf(SIGHT_NO))
    {
        assert(!args.empty());
        Atlas::Objects::Entity::RootEntity ent = 
            smart_dynamic_cast<Atlas::Objects::Entity::RootEntity>(args.front());
            
        if (ent && (ent->getId() == m_roomId))
        {
            sight(ent);
            return HANDLED;
        }
        
        Imaginary img = smart_dynamic_cast<Imaginary>(args.front());
        if (img)
        {
            handleSightImaginary(img);
            return HANDLED;
        }
    }

    if (op->instanceOf(APPEARANCE_NO))
    {
        for (unsigned int A=0; A < args.size(); ++A)
            apperance(args[A]->getId());

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
        if (tk)
        {
            handleSoundTalk(tk);
            return HANDLED;
        }
    }

    return IGNORED;
}

void Room::sight(const Atlas::Objects::Entity::RootEntity &room)
{
    if (m_entered)
        warning() << "got SIGHT of entered room " << m_roomId;
        
    m_name = room->getName();
    if (room->hasAttr("topic")
        m_topic = room->getAttr("topic").toString();
		
    if (room->hasAttr("people"))
    {
        Atlas::Message::ListType people = room->getAttr("people").asList();
        for (Atlas::Message::ListType::const_iterator P=people.begin(); P!=people.end(); ++P)
            appearance(P->asString());
    }
    
    checkEntry();
    
    if (room->hasAttr("rooms"))
    {
        Atlas::Message::ListType rooms = room->getAttr("rooms").asList();
        for (Atlas::Message::ListType::const_iterator R=rooms.begin(); R!=rooms.end(); ++R)
        {
            m_rooms.push_back(new Room(m_lobby, R->asString());
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
        
    const std::vector<Root>& args = tk.getArgs();
    if (args.empty())
    {
        warning() << "room " << m_roomId << " recieved sound(talk) with no args";
        return;
    }
    
    if (!args.front()->hasAttr("say"))
        return;
    
    std::string description = args.front()->getAttr("say").asString();
    Talk.emit(this, P->second, say);
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

    const std::vector<Root>& args = im.getArgs();
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
            Apperance.emit(this, person);
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
    for (P = m_members.begin(); P != m_members.end(); ++P)
        if (P->second == NULL) anyPending = true;
       
    if (!anyPending)
    {
        Entered.emit(this);
        m_entered = true;
    }
}

} // of Eris namespace
