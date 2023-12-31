#include "ViewEntity.h"

#include "Connection.h"
#include "EntityRouter.h"
#include "View.h"
#include "Avatar.h"
#include "Task.h"

#include <sigc++/bind.h>


namespace Eris {
ViewEntity::ViewEntity(std::string id, TypeInfo* ty, View& view) :
		Entity(std::move(id), ty),
		m_view(view) {
}

ViewEntity::~ViewEntity() = default;

Entity* ViewEntity::getEntity(const std::string& id) {
	auto child = m_view.getEntity(id);
	if (!child || !child->m_visible) {
		// we don't have the entity at all, so request it and skip
		// processing it here; everything will come right when it
		// arrives.
		m_view.getEntityFromServer(id);
	}
	return child;
}

void ViewEntity::onTaskAdded(const std::string& id, Task* task) {
	task->ProgressRateChanged.connect(sigc::bind(sigc::mem_fun(*this, &ViewEntity::task_ProgressRateChanged), task));
	Entity::onTaskAdded(id, task);
	m_view.taskRateChanged(task);
}

void ViewEntity::task_ProgressRateChanged(Task* task) {
	m_view.taskRateChanged(task);
}


}
