#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/EntityRef.h>
#include <Eris/Entity.h>
#include <Eris/View.h>
#include <Eris/Log.h>

#include <sigc++/slot.h>

#include <cassert>

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

EntityRef::EntityRef(const EntityRef& ref) :
    m_inner(ref.m_inner)
{
    if (m_inner)
    {
        m_inner->BeingDeleted.connect(sigc::mem_fun(this, &EntityRef::onEntityDeleted));
    }
}

EntityRef& EntityRef::operator=(const EntityRef& ref)
{	
    bool changed = (m_inner != ref.m_inner);
    m_inner = ref.m_inner;
    
    if (m_inner)
    {
        m_inner->BeingDeleted.connect(sigc::mem_fun(this, &EntityRef::onEntityDeleted));
    }
    
    if (changed) Changed.emit();
    return *this;
}
    
void EntityRef::onEntityDeleted()
{
	m_inner = NULL;
	Changed();
}

void EntityRef::onEntitySeen(Entity* e)
{
    assert(e);
    m_inner = e;
    m_inner->BeingDeleted.connect(sigc::mem_fun(this, &EntityRef::onEntityDeleted));	
	Changed();
}
	
} // of namespace Eris

