#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "Person.h"
#include "LogStream.h"
#include "Exceptions.h"
#include "Lobby.h"
#include "Connection.h"
#include "Account.h"

#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

typedef Atlas::Objects::Entity::Account AtlasAccount;
using namespace Atlas::Objects::Operation;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Root;

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
    
    if (acc->isDefaultName()) {
		m_fullName = m_id;
	} else {
		m_fullName = acc->getName();
	}
}

void Person::msg(const std::string &msg)
{
    if (!m_lobby->getConnection()->isConnected())
    {
        error() << "sending private chat, but connection is down";
        return;
    }
	
    Anonymous speech;
    speech->setAttr("say",msg);
	
    Talk t;
    t->setArgs1(speech);
    t->setTo(m_id);
    t->setFrom(m_lobby->getAccount()->getId());
    t->setSerialno(getNewSerialno());
	
    m_lobby->getConnection()->send(t);
}

} // of namespace Eris
