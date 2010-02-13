#include "ViewEntity.h"

#include "Connection.h"
#include "EntityRouter.h"
#include "View.h"
#include "Avatar.h"



namespace Eris {
ViewEntity::ViewEntity(const std::string& id, TypeInfo* ty, View* view) :
	Entity::Entity(id, ty), m_view(view) {
	m_router = new EntityRouter(this);
	m_view->getConnection()->registerRouterForFrom(m_router, id);

}

ViewEntity::~ViewEntity() {
}

void ViewEntity::shutdown() {
	m_view->getConnection()->unregisterRouterForFrom(m_router, m_id);
	m_view->entityDeleted(this); // remove ourselves from the View's content map
	delete m_router;
	Entity::shutdown();

}

TypeService* ViewEntity::getTypeService() const {
	return m_view->getAvatar()->getConnection()->getTypeService();
}

void ViewEntity::onTalk(const Atlas::Objects::Operation::RootOperation& talk) {
	Entity::onTalk(talk);
	m_view->getAvatar()->Hear.emit(this, talk);
}

void ViewEntity::onSoundAction(
		const Atlas::Objects::Operation::RootOperation& op) {
	Entity::onSoundAction(op);
	m_view->getAvatar()->Hear.emit(this, op);
}

void ViewEntity::onVisibilityChanged(bool vis)
{
    m_view->setEntityVisible(this, vis);
	Entity::onVisibilityChanged(vis);
}

void ViewEntity::removeFromMovementPrediction() {
	m_view->removeFromPrediction(this);
}

void ViewEntity::addToMovementPredition() {
	m_view->addToPrediction(this);
}

Entity* ViewEntity::getEntity(const std::string& id) {
	Entity* child = m_view->getEntity(id);
	if (!child) {
		// we don't have the entity at all, so request it and skip
		// processing it here; everything will come right when it
		// arrives.
		m_view->getEntityFromServer(id);
	}
	return child;
}



}
