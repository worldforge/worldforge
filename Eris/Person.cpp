#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Person.h>

#include <Eris/Exceptions.h>
#include <Eris/Lobby.h>
#include <Eris/Utils.h>

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>

typedef Atlas::Objects::Entity::Account AtlasAccount;
using namespace Atlas::Objects::Operation;

namespace Eris
{
	
Person::Person(Lobby *l, const AtlasAccount &acc) :
    m_id(acc->getId()),
    m_fullName(acc->getName()),
    m_lobby(l)
{
    
}
	
void Person::sight(const AtlasAccount &acc)
{
    if (acc->getId() != m_id)
    {
        error() << "person got sight(account) with mismatching Ids";
        return;
    }
    
    if (acc->isDefaultName())
        m_name = m_id;
    else
        m_name = acc->getName();
}

void Person::msg(const std::string &msg)
{
    if (!m_lobby->getConnection()->isConnected())
    {
        error() << "sending private chat, but connection is down";
        return;
    }
	
    Atlas::Message::Element::MapType speech;
    speech["say"] = tk;
	
    Talk t;
    t.setArgs(Atlas::Message::Element::ListType(1, speech));
    t->setTo(m_id);
    t->setFrom(m_lobby->getAccountID());
    t->setSerialno(getNewSerialno());
	
    m_lobby->getConnection()->send(t);
}

} // of namespace Eris
