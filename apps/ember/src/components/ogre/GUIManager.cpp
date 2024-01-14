/*
 Copyright (C) 2004  Miguel Guzman (Aganor)
 Copyright (C) 2005  Erik Ogenvik <erik@ogenvik.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "GUIManager.h"
#include "EmberOgre.h"
#include "domain/EmberEntity.h"
#include "framework/MainLoopController.h"
#include "framework/ConsoleBackend.h"
#include "domain/EntityTalk.h"
#include "GUICEGUIAdapter.h"
#include "World.h"

#include "widgets/EntityTooltip.h"
#include "widgets/Widget.h"
#include "widgets/QuickHelp.h"
#include "widgets/WidgetDefinitions.h"
#include "widgets/icons/IconManager.h"
#include "widgets/EntityIconManager.h"
#include "widgets/ActionBarIconManager.h"
#include "widgets/WorldLoadingScreen.h"

#include "camera/MainCamera.h"
#include "gui/ActiveWidgetHandler.h"
#include "gui/CursorWorldListener.h"

#include "components/cegui/CEGUILogger.h"
#include "components/cegui/ColouredRenderedStringParser.h"
#include "components/cegui/SDLNativeClipboardProvider.h"
#include "components/cegui/CEGUISetup.h"
#include "components/ogre/widgets/HitDisplayer.h"


#include "services/config/ConfigService.h"
#include "services/server/ServerServiceSignals.h"
#include "services/sound/SoundService.h"
#include "services/sound/SoundSample.h"

#include "framework/IResourceProvider.h"

#include <Eris/View.h>
#include <Eris/Avatar.h>
#include <Eris/TypeInfo.h>

#include <OgreRoot.h>
#include <OgreTextureManager.h>

#include <CEGUI/WindowManager.h>
#include <CEGUI/SchemeManager.h>
#include "CEGUIOgreRenderer/ResourceProvider.h"
#include "CEGUIOgreRenderer/ImageCodec.h"
#include <CEGUI/widgets/MultiLineEditbox.h>
#include <CEGUI/widgets/Editbox.h>
#include <fmt/ostream.h>

#ifdef _WIN32
#include "platform/platform_windows.h"
#endif

template<>
struct fmt::formatter<Eris::Entity> : ostream_formatter {
};
template<>
struct fmt::formatter<Ember::EmberEntity> : ostream_formatter {
};

using namespace CEGUI;
using namespace Ember::OgreView::Gui;
using namespace Ember;

namespace Ember::OgreView {

unsigned long GUIManager::msAutoGenId(0);

GUIManager::GUIManager(Cegui::CEGUISetup& ceguiSetup,
					   ConfigService& configService,
					   ServerServiceSignals& serverSignals,
					   MainLoopController& mainLoopController) :
		ToggleInputMode("toggle_inputmode", this, "Toggle the input mode."),
		ReloadGui("reloadgui", this, "Reloads the gui."),
		ToggleGui("toggle_gui", this, "Toggle the gui display"),
		mCeguiSetup(ceguiSetup),
		mConfigService(configService),
		mMainLoopController(mainLoopController),
		mGuiCommandMapper("gui", "key_bindings_gui"),
		mSheet(nullptr),
		mWindowManager(nullptr),
		mEnabled(true),
		mRenderedStringParser(std::make_unique<Cegui::ColouredRenderedStringParser>()),
		mQuickHelp(std::make_unique<Gui::QuickHelp>()),
		mEntityTooltip(nullptr),
		mNativeClipboardProvider(std::make_unique<Ember::Cegui::SDLNativeClipboardProvider>()),
		mWidgetDefinitions(std::make_unique<WidgetDefinitions>()) {


	mGuiCommandMapper.restrictToInputMode(Input::IM_GUI);

	serverSignals.GotView.connect(sigc::mem_fun(*this, &GUIManager::server_GotView));


	try {

		logger->info("Starting CEGUI");
		mDefaultScheme = "EmberLook";
		logger->debug("Setting default scheme to {}", mDefaultScheme);

		mCeguiSetup.getSystem().getClipboard()->setNativeProvider(mNativeClipboardProvider.get());

		mCeguiSetup.getSystem().setDefaultCustomRenderedStringParser(mRenderedStringParser.get());
		mWindowManager = &CEGUI::WindowManager::getSingleton();

		EntityTooltip::registerFactory();

		mSheet.reset(mWindowManager->createWindow("DefaultWindow", "root_wnd"));
		mCeguiSetup.getSystem().getDefaultGUIContext().setRootWindow(mSheet.get());

		mSheet->activate();
		mSheet->moveToBack();
		mSheet->setDistributesCapturedInputs(false);

		mWorldLoadingScreen = std::make_unique<WorldLoadingScreen>();

		logger->info("CEGUI system set up");

		getInput().EventKeyPressed.connect(sigc::mem_fun(*this, &GUIManager::pressedKey));
		getInput().setInputMode(Input::IM_GUI);

		//add adapter for CEGUI, this will route input event to the gui
		mCEGUIAdapter = std::make_unique<GUICEGUIAdapter>(mCeguiSetup.getSystem(), mCeguiSetup.getRenderer());
		getInput().addAdapter(mCEGUIAdapter.get());

		mGuiCommandMapper.bindToInput(getInput());

		//connect to the creation of the avatar, since we want to switch to movement mode when that happens
		EmberOgre::getSingleton().EventCreatedAvatarEntity.connect(sigc::mem_fun(*this, &GUIManager::EmberOgre_CreatedAvatarEntity));
		EmberOgre::getSingleton().EventWorldCreated.connect(sigc::mem_fun(*this, &GUIManager::EmberOgre_WorldCreated));
		EmberOgre::getSingleton().EventWorldBeingDestroyed.connect(sigc::mem_fun(*this, &GUIManager::EmberOgre_WorldBeingDestroyed));

		mActiveWidgetHandler = std::make_unique<Gui::ActiveWidgetHandler>(*this);


		mIconManager = std::make_unique<Gui::Icons::IconManager>();
		mEntityIconManager = std::make_unique<Gui::EntityIconManager>(*this);
		mActionBarIconManager = std::make_unique<Gui::ActionBarIconManager>(*this);

		Ogre::Root::getSingleton().addFrameListener(this);

		setupUISounds();

	} catch (const CEGUI::Exception& ex) {
		logger->error("GUIManager - error when creating gui: {}", ex.what());
		throw Exception(ex.getMessage().c_str());
	}

	if (chdir(configService.getEmberDataDirectory().generic_string().c_str())) {
		logger->warn("Failed to change to the data directory '{}'.", configService.getEmberDataDirectory().string());
	}

	mWidgetDefinitions->registerWidgets(*this);


}

GUIManager::~GUIManager() {
	logger->info("Shutting down GUI manager.");
	mCeguiSetup.getSystem().setDefaultCustomRenderedStringParser(nullptr);

	mWorldLoadingScreen.reset();
	if (mCEGUIAdapter) {
		getInput().removeAdapter(mCEGUIAdapter.get());
	}
	mWidgetDefinitions.reset();
	mWidgets.clear();
	CEGUI::WindowManager::getSingleton().cleanDeadPool();
	EntityTooltip::deregisterFactory();

	Ogre::Root::getSingleton().removeFrameListener(this);


}

void GUIManager::render() const {
	if (mEnabled) {
		CEGUI::System::getSingleton().renderAllGUIContexts();
	}
}

void GUIManager::server_GotView(Eris::View* view) {
	//The View has a shorter lifespan than ours, so we don't need to store references to the connections.
	view->EntitySeen.connect([view, this](Eris::Entity* entity) {
		//It's safe to cast to EmberEntity, since all entities in the system are guaranteed to be of this type.
		auto* emberEntity = dynamic_cast<EmberEntity*>(entity);
		entity->Say.connect([emberEntity, this](const Atlas::Objects::Root& op) {
			auto entityTalk = EntityTalk::parse(op);

			if (!entityTalk.sound.empty()) {
				logger->debug("Entity {} makes the sound: \"{}\"", *emberEntity, entityTalk.sound);
			} else if (!entityTalk.message.empty()) {
				logger->debug("Entity {} says: \"{}\"", *emberEntity, entityTalk.message);
			}
			AppendIGChatLine.emit(entityTalk, emberEntity);
		});

		emberEntity->Noise.connect([emberEntity](const Atlas::Objects::Root& op, const Eris::TypeInfo& typeInfo) {
			std::string message = emberEntity->getNameOrType() + " emits a " + op->getParent() + ".";
			ConsoleBackend::getSingletonPtr()->pushMessage(message, "info");
			logger->debug("Entity: {} ({}) sound action: {}", emberEntity->getId(), emberEntity->getName(), op->getParent());
		});
		emberEntity->Acted.connect([emberEntity](const Atlas::Objects::Operation::RootOperation& act, const Eris::TypeInfo& typeInfo) {
			if (typeInfo.isA("activity")) {
				std::string message = emberEntity->getNameOrType() + " performs a " + act->getParent() + ".";
				ConsoleBackend::getSingletonPtr()->pushMessage(message, "info");

				logger->debug("Entity: {} ({}) action: {}", emberEntity->getId(), emberEntity->getName(), act->getParent());
			}
		});

		emberEntity->Hit.connect([emberEntity, view](const Atlas::Objects::Operation::Hit& act) {
			std::string message;
			if (!act->getArgs().empty()) {
				auto& arg = act->getArgs().front();
				EmberEntity* actingEntity = nullptr;
				if (!arg->isDefaultId()) {
					actingEntity = dynamic_cast<EmberEntity*>(view->getEntity(arg->getId()));
				}
				if (arg->hasAttr("damage")) {
					auto damageElem = arg->getAttr("damage");
					if (damageElem.isNum()) {
						std::stringstream ss;
						ss.precision(2);
						ss << damageElem.asNum();
						auto damageString = ss.str();
						if (actingEntity) {
							message = emberEntity->getNameOrType() + " is hit by " + actingEntity->getNameOrType() + " for " + damageString + " damage.";
						} else {
							message = emberEntity->getNameOrType() + " is hit for " + damageString + " damage.";
						}
						ConsoleBackend::getSingletonPtr()->pushMessage(message, "info");
					}
				}
			}

		});

		emberEntity->Emote.connect(sigc::bind(sigc::mem_fun(*this, &GUIManager::entity_Emote), emberEntity));

	});
}

void GUIManager::entity_Emote(const std::string& description, EmberEntity* entity) {
	//TODO: extract this into something which is aware of the View
	//If it's our own entity we should just print what it says.
//	if (entity == entity->getView()->getAvatar().getEntity()) {
	if (false) {
		AppendAvatarImaginary(description);
	} else {
		AppendAvatarImaginary(entity->getName() + " " + description);
	}
}

void GUIManager::EmitEntityAction(const std::string& action, EmberEntity* entity) {
	EventEntityAction.emit(action, entity);
}

CEGUI::Window* GUIManager::createWindow(const std::string& windowType) {
	std::stringstream ss;
	ss << "_autoWindow_" << (msAutoGenId++);
	return createWindow(windowType, ss.str());
}

CEGUI::Window* GUIManager::createWindow(const std::string& windowType, const std::string& windowName) {
	try {
		CEGUI::Window* window = mWindowManager->createWindow(windowType, windowName);
		return window;
	} catch (const CEGUI::Exception& ex) {
		logger->error("Error when creating new window of type {} with name {}.\n{}", windowType, windowName, ex.getMessage().c_str());
		return nullptr;
	} catch (const std::exception& ex) {
		logger->error("Error when creating new window of type {} with name {}: {}", windowType, windowName, ex.what());
		return nullptr;
	}
}


Widget* GUIManager::createWidget() {
	try {
		auto widget = new Widget(*this);
		mWidgets.emplace_back(std::unique_ptr<Widget>(widget));
		return widget;
	} catch (const std::exception& e) {
		logger->error("Error when loading widget: {}", e.what());
		return nullptr;
	}
}

void GUIManager::destroyWidget(Widget* widget) {
	if (!widget) {
		logger->warn("Trying to destroy null widget.");
		return;
	}
	removeWidget(widget);
}

CEGUI::Texture& GUIManager::createTexture(Ogre::TexturePtr& ogreTexture, std::string name) {
	if (name.empty()) {
		name = ogreTexture->getName();
	}
	return mCeguiSetup.getRenderer().createTexture(name, ogreTexture);
}

Input& GUIManager::getInput() const {
	return Input::getSingleton();
}

CEGUI::Window* GUIManager::getMainSheet() const {
	return mSheet.get();
}

void GUIManager::removeWidget(Widget* widget) {
	auto I = std::find_if(mWidgets.begin(), mWidgets.end(), [widget](const std::unique_ptr<Widget>& item) { return item.get() == widget; });
	if (I != mWidgets.end()) {
		mWidgets.erase(I);
	}
	mMainLoopController.getEventService().runOnMainThread([]() {
		CEGUI::WindowManager::getSingleton().cleanDeadPool();
	});
}

void GUIManager::addWidget(Widget* widget) {
}

bool GUIManager::frameStarted(const Ogre::FrameEvent& evt) {
	try {
		CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);
		CEGUI::System::getSingleton().getDefaultGUIContext().injectTimePulse(evt.timeSinceLastFrame);
	} catch (const CEGUI::Exception& ex) {
		logger->warn("Error in CEGUI: {}", ex.what());
	}

	//iterate over all widgets and send them a frameStarted event
	auto I = mWidgets.begin();
	auto I_end = mWidgets.end();

	for (; I != I_end; ++I) {
		auto& aWidget = *I;
		try {
			aWidget->frameStarted(evt);
		} catch (const CEGUI::Exception& ex) {
			logger->warn("Error in CEGUI: {}", ex.what());
		}
	}

	EventFrameStarted.emit(evt.timeSinceLastFrame);

	return true;

}

bool GUIManager::isInMovementKeysMode() const {
	return mSheet->isCapturedByThis() || !isInGUIMode();
}

bool GUIManager::isInGUIMode() const {
	return getInput().getInputMode() == Input::IM_GUI;
}

void GUIManager::pressedKey(const SDL_Keysym& key, Input::InputMode inputMode) {
	if (((key.mod & KMOD_CTRL) || (key.mod & KMOD_LCTRL) || (key.mod & KMOD_RCTRL))) {
		if (key.sym == SDLK_c) {
			mCeguiSetup.getSystem().getDefaultGUIContext().injectCopyRequest();
		} else if (key.sym == SDLK_x) {
			mCeguiSetup.getSystem().getDefaultGUIContext().injectCutRequest();
		} else if (key.sym == SDLK_v) {
			mCeguiSetup.getSystem().getDefaultGUIContext().injectPasteRequest();
		}
	}
}

void GUIManager::runCommand(const std::string& command, const std::string& args) {
	if (command == ToggleInputMode.getCommand()) {
		getInput().toggleInputMode();
	} else if (command == ToggleGui.getCommand()) {

		logger->debug("Toggle Gui Initiated -- {}", fmt::underlying(getInput().getInputMode()));

		if (mEnabled) {
			logger->info("Disabling GUI");
			mEnabled = false;

			getInput().removeAdapter(mCEGUIAdapter.get());

		} else {
			logger->info("Enabling GUI");
			mEnabled = true;

			getInput().addAdapter(mCEGUIAdapter.get());
		}
	} else if (command == ReloadGui.getCommand()) {
		Ogre::TextureManager* texMgr = Ogre::TextureManager::getSingletonPtr();
		Ogre::ResourcePtr resource = texMgr->getByName("cegui/" + getDefaultScheme() + ".png", "UI");
		if (resource) {
			resource->reload();
		}
	}
}

void GUIManager::EmberOgre_CreatedAvatarEntity(EmberEntity& entity) {
	//switch to movement mode, since it appears most people don't know how to change from gui mode
	varconf::Variable var;
	if (!ConfigService::getSingleton().getValue("input", "automovementmode", var)
		|| (var.is_bool() && (bool) var)) {
		getInput().setInputMode(Input::IM_MOVEMENT);
	}
}

void GUIManager::EmberOgre_WorldCreated(World& world) {
	UniqueWindowPtr<EmberEntityTooltipWidget> tooltipWindow(dynamic_cast<EmberEntityTooltipWidget*>(mWindowManager->createWindow("EmberLook/EntityTooltip", "EntityTooltip")));
	mEntityTooltip = std::make_unique<EntityTooltip>(world, std::move(tooltipWindow), *mIconManager);
	mCursorWorldListener = std::make_unique<CursorWorldListener>(mMainLoopController, *mSheet, world);

	UniqueWindowPtr<CEGUI::Window> labelWindow(WindowManager::getSingleton().loadLayoutFromFile(GUIManager::getLayoutDir() + std::string("Hit.layout")));

	mHitDisplayer = std::make_unique<HitDisplayer>(*mSheet, labelWindow, world.getMainCamera().getCamera(), world.getView(), world.getSceneManager());
}

void GUIManager::EmberOgre_WorldBeingDestroyed() {
	mHitDisplayer.reset();
	mEntityTooltip.reset();
	mCursorWorldListener.reset();
}

Gui::EntityTooltip* GUIManager::getEntityTooltip() const {
	return mEntityTooltip.get();
}

std::string GUIManager::getLayoutDir() {
	return "cegui/datafiles/layouts/";
}

const std::string& GUIManager::getDefaultScheme() const {
	return mDefaultScheme;
}

Gui::Icons::IconManager* GUIManager::getIconManager() const {
	return mIconManager.get();
}

Gui::EntityIconManager* GUIManager::getEntityIconManager() const {
	return mEntityIconManager.get();
}

Gui::ActionBarIconManager* GUIManager::getActionBarIconManager() const {
	return mActionBarIconManager.get();
}

CEGUI::Renderer* GUIManager::getGuiRenderer() const {
	return &mCeguiSetup.getRenderer();
}

void GUIManager::setupUISounds() {

	auto sharedMediaPath = ConfigService::getSingleton().getSharedDataDirectory();


	//Hardcoded for now. If need arises we'll put it into config.
	std::map<std::string, std::vector<std::string>> uiSounds = {
			{"ember/sounds/448086__breviceps__normal-click.ogg", {"PushButton/Clicked", "TabButton/Clicked", "Combobox/DropListDisplayed"}}};

	for (const auto& entry: uiSounds) {

		auto filePath = sharedMediaPath / "data" / "ui" / entry.first;

		std::shared_ptr<SoundSample> sample = SoundSample::create(filePath);

		if (sample) {
			for (const auto& event: entry.second) {
				logger->debug("Registering UI event '{}' to play sound '{}'.", event, entry.first);
				CEGUI::GlobalEventSet::getSingleton().subscribeEvent(event, [sample](const EventArgs&) {
					sample->reset();
					SoundService::SoundGroup soundGroup{.sounds={SoundService::Sound{.soundSample = sample}}, .repeating=false};
					SoundService::getSingleton().playSound(soundGroup);
					return true;
				});
			}
		} else {
			logger->error("Failed to create sound sample for {}.", entry.first);
		}
	}

}

}


