#ifndef ERIS_ENTITY_REF_H
#define ERIS_ENTITY_REF_H

#include <sigc++/trackable.h>
#include <sigc++/connection.h>
#include <sigc++/signal.h>

namespace Eris
{

class Entity;
class View;

class EntityRef : public sigc::trackable
{
public:
	EntityRef() : m_inner(NULL)
	{
	}

	EntityRef(View* v, const std::string& eid);
	
    EntityRef(Entity*);
    
	~EntityRef()
	{
	}
	
	EntityRef(const EntityRef& ref) :
		m_inner(ref.m_inner)
	{
		m_deleteSlot = ref.m_deleteSlot;
		m_seenSlot = ref.m_seenSlot;
	}
	
	EntityRef& operator=(const EntityRef& ref)
	{	
		bool changed = (m_inner != ref.m_inner);
		m_inner = ref.m_inner;
		m_deleteSlot = ref.m_deleteSlot;
		m_seenSlot = ref.m_seenSlot;
		if (changed) Changed.emit();
		return *this;
	}

	const Entity& operator*() const
	{
		return *m_inner;
	}

	Entity& operator*()
	{
		return *m_inner;
	}

	const Entity* operator->() const
	{
		return m_inner;
	}

	Entity* operator->()
	{
		return m_inner;
	}

	Entity* get() const
	{
		return m_inner;
	}
    operator bool() const
	{
		return (m_inner != NULL);
	}
	
	bool operator!() const
	{
		return (m_inner == NULL);
	}

	sigc::signal0<void> Changed;
private:
	void onEntityDeleted();
	void onEntitySeen(Entity* e);
	
	Entity* m_inner;
	sigc::connection m_deleteSlot, 
		m_seenSlot;
};

} // of namespace Eris

#endif // of ERIS_ENTITY_REF_H
