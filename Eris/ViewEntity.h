#ifndef ERIS_VIEW_ENTITY_H_
#define ERIS_VIEW_ENTITY_H_

#include <Eris/Entity.h>

namespace Eris {

class ViewEntity : public Entity {
friend class EntityRouter;
public:

	/**
	 * @brief Ctor.
	 * @param id The id of the entity.
	 * @param ty Type info for the entity.
	 * @param view The view to which the entity belongs.
	 */
	ViewEntity(const std::string& id, TypeInfo* ty, View* view);

	virtual ~ViewEntity();

    virtual void shutdown();

protected:
    View* m_view;   ///< the View which owns this Entity
    EntityRouter* m_router;

    virtual void onTalk(const Atlas::Objects::Operation::RootOperation& talk);

    virtual void onSoundAction(const Atlas::Objects::Operation::RootOperation& op);

    virtual void onVisibilityChanged(bool vis);

    virtual void removeFromMovementPrediction();

    virtual void addToMovementPredition();

    virtual Entity* getEntity(const std::string& id);

    virtual TypeService* getTypeService() const;

    virtual View* getView() const;

};

inline View* ViewEntity::getView() const
{
    return m_view;
}
}

#endif /* VIEWENTITY_H_ */
