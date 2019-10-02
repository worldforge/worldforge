#ifndef ERIS_ENTITY_REF_H
#define ERIS_ENTITY_REF_H

#include <sigc++/trackable.h>
#include <sigc++/signal.h>
#include <string>

namespace Eris
{

class Entity;
class View;

class EntityRef : public sigc::trackable
{
public:
	EntityRef() : m_inner(nullptr)
	{
	}

	EntityRef(View* v, const std::string& eid);
	
    EntityRef(Entity*);
    
	~EntityRef() = default;

	EntityRef(const EntityRef& ref);
    	
	EntityRef& operator=(const EntityRef& ref);
    
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
		return (m_inner != nullptr);
	}
	
	bool operator!() const
	{
		return (m_inner == nullptr);
	}

    bool operator==(const EntityRef& e) const
    {
        return (m_inner == e.m_inner);
    }

    bool operator==(const Entity* e) const
    {
        return (m_inner == e);
    }

    bool operator<(const EntityRef& e) const
    {
        return (m_inner < e.m_inner);
    }

	sigc::signal0<void> Changed;
private:
	void onEntityDeleted();
	void onEntitySeen(Entity* e);
	
	Entity* m_inner;
};

} // of namespace Eris

#endif // of ERIS_ENTITY_REF_H
