#ifndef ERIS_VIEW_ENTITY_H_
#define ERIS_VIEW_ENTITY_H_

#include <Eris/Entity.h>

namespace Eris {

/**
 * @brief An entity which is bound to an Eris::View.
 * This subclass of Eris::Entity is intimately bound to a View.
 * As Atlas messages are received from the server these will be routed into
 * an instance of this class.
 *
 * In your client this is the class you would want to work with for any
 * entities which represents entities that are on the server. If you however
 * are working with entities that aren't represented on the server (the
 * typical example would be if you're allowing client side authoring where
 * you want to create entities locally) you would instead want to implement
 * a new class which inherits Eris::Entity.
 */
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

    /**
     * @brief Gets the view to which this entity belongs, if any.
     * @return The view to which this entity belongs, or null if
     * this entity isn't connected to any view.
     */
    virtual View* getView() const;

protected:

    /**
     * @brief The View which owns this Entity.
     */
    View* m_view;

    /**
     * @brief A router instance which routes messages from the view
     * into this entity.
     */
    EntityRouter* m_router;

    virtual void onTalk(const Atlas::Objects::Operation::RootOperation& talk);

    virtual void onSoundAction(
    		const Atlas::Objects::Operation::RootOperation& op);

    virtual void onVisibilityChanged(bool vis);

    virtual void onTaskAdded(Task* task);

    virtual void removeFromMovementPrediction();

    virtual void addToMovementPredition();

    virtual Entity* getEntity(const std::string& id);

    virtual TypeService* getTypeService() const;

    /**
     * @brief Listen to task progress rates updates and send to the view.
     * @param task The task which is changed.
     */
    void task_ProgressRateChanged(Task* task);


};

inline View* ViewEntity::getView() const
{
    return m_view;
}
}

#endif /* VIEWENTITY_H_ */
