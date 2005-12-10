#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/EntityRef.h>
#include <Eris/Entity.h>
#include <Eris/View.h>

#include <sigc++/slot.h>

namespace Eris
{

EntityRef::EntityRef(View* v, const std::string& eid) :
	m_inner(NULL)
{
	if (eid.empty()) return;
	
	assert(v);
	m_inner = v->getEntity(eid);
	if (m_inner)
	{
		m_inner->BeingDeleted.connect(sigc::mem_fun(this, &EntityRef::onEntityDeleted));	
	} else {
		// retrieve from the server, tell us when that happens
		v->notifyWhenEntitySeen(eid, sigc::mem_fun(this, &EntityRef::onEntitySeen));
	}
}

EntityRef::EntityRef(Entity* e) :
    m_inner(e)
{
    if (m_inner)
	{
		m_inner->BeingDeleted.connect(sigc::mem_fun(this, &EntityRef::onEntityDeleted));	
	}
}

void EntityRef::onEntityDeleted()
{
	m_inner = NULL;
	Changed();
}

void EntityRef::onEntitySeen(Entity* e)
{
	// assert(e->id() == m_id);
	m_inner = e;
	Changed();
}
	
} // of namespace Eris

