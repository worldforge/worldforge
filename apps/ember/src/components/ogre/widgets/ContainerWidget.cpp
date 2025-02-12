/*
 Copyright (C) 2020 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include "components/ogre/EmberOgre.h"

#include "ContainerWidget.h"

#include <Atlas/Objects/RootEntity.h>

#include "components/ogre/GUIManager.h"
#include "domain/EmberEntity.h"
#include "EntityIcon.h"
#include "EntityTooltip.h"
#include "components/ogre/World.h"
#include "components/ogre/Avatar.h"
#include "services/server/ServerService.h"
#include <CEGUI/widgets/FrameWindow.h>
#include <Eris/Avatar.h>
#include <Eris/Connection.h>
#include <framework/AutoCloseConnection.h>

namespace Ember::OgreView::Gui {

WidgetPluginCallback ContainerWidget::registerWidget(GUIManager& guiManager) {

	struct State {
		std::map<std::string, std::unique_ptr<ContainerWidget>> containerWidgets;
		std::vector<Ember::AutoCloseConnection> connections;
	};
	auto state = std::make_shared<State>();

	auto connectFn = [state, &guiManager](Eris::Avatar* avatar) {
		auto openedFn = [&](Eris::Entity& entity) {
			try {
				auto widget = std::make_unique<Gui::ContainerWidget>(guiManager, dynamic_cast<EmberEntity&>(entity));
				state->containerWidgets.emplace(entity.getId(), std::move(widget));
			} catch (const std::exception& ex) {
				logger->error("Could not create container widget: {}", ex.what());
			} catch (...) {
				logger->error("Could not create container widget.");
			}
		};

		state->connections.clear();
		state->connections.emplace_back(avatar->ContainerOpened.connect(openedFn));
		state->connections.emplace_back(avatar->ContainerClosed.connect([=](Eris::Entity& entity) {
			state->containerWidgets.erase(entity.getId());
		}));

		//Check if there are any active containers already.
		for (auto& entry: avatar->getActiveContainers()) {
			if (*entry.second) {
				openedFn(**entry.second);
			}
		}
	};

	auto con = ServerService::getSingleton().GotAvatar.connect(connectFn);

	if (ServerService::getSingleton().getAvatar()) {
		connectFn(ServerService::getSingleton().getAvatar());
	}

	//Just hold on to an instance.
	return [state, con]() mutable {
		state->connections.clear();
		state->containerWidgets.clear();
		con.disconnect();
		state.reset();
	};

}


ContainerWidget::ContainerWidget(GUIManager& guiManager, EmberEntity& entity, int slotSize)
		: mGuiManager(guiManager),
		  mSlotSize(slotSize),
		  mWidget(guiManager.createWidget()) {
	mWidget->loadMainSheet("ContainerWidget.layout", "Container_" + entity.getId());
	mContainerView = std::make_unique<ContainerView>(*guiManager.getEntityIconManager(),
													 *guiManager.getIconManager(),
													 guiManager.getEntityTooltip()->getTooltipWindow(),
													 *mWidget->getWindow("IconContainer"),
													 32);
	mContainerView->EventEntityPicked.connect([&guiManager](EmberEntity* pickedEntity) {
		guiManager.EmitEntityAction("pick", pickedEntity);
	});
	mContainerView->EventIconDropped.connect([this](EntityIcon* entityIcon, EntityIconSlot*) {
		auto observedEntity = mContainerView->getObservedEntity();
		if (observedEntity && entityIcon->getEntity()) {
			EmberOgre::getSingleton().getWorld()->getAvatar()->getErisAvatar().place(entityIcon->getEntity(), observedEntity);
		}
	});
	mWidget->getMainWindow()->setText("Container " + entity.getNameOrType());

	//Close containers by sending a "close_container" Use op.
	mWidget->getMainWindow()->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, [this]() {

		auto& erisAvatar = EmberOgre::getSingleton().getWorld()->getAvatar()->getErisAvatar();

		Atlas::Objects::Operation::Use const use;
		use->setFrom(erisAvatar.getId());

		Atlas::Objects::Entity::RootEntity const rootEntity;
		rootEntity->setId(mContainerView->getObservedEntity()->getId());

		Atlas::Objects::Operation::RootOperation const op;
		op->setParent("close_container");
		op->setArgs1(rootEntity);

		use->setArgs1(op);

		erisAvatar.getConnection().send(use);

	});

	mWidget->setIsActiveWindowOpaque(false);
	mContainerView->showEntityContents(&entity);

}

ContainerWidget::~ContainerWidget() {
	mContainerView.reset();
	mGuiManager.removeWidget(mWidget);
}


}