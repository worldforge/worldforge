/*
 Copyright (C) 2004  Miguel Guzman (Aglanor)
 Copyright (C) 2006  Erik Ogenvik <erik@ogenvik.org>

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

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include "EmberOgrePrerequisites.h"
#include "widgets/CEGUIUtils.h"
#include "services/input/InputCommandMapper.h"
#include "framework/Singleton.h"
#include "framework/ConsoleObject.h"
#include "framework/ConsoleCommandWrapper.h"

#include <OgreFrameListener.h>
#include <sigc++/trackable.h>
#include <SDL3/SDL_keyboard.h>


namespace CEGUI {
class Window;

class OgreResourceProvider;

class OgreImageCodec;

class OgreRenderer;

class Renderer;

class Texture;

class System;

class WindowManager;

class NativeClipboardProvider;
}

namespace Eris {
class View;

class Entity;
}

namespace Ember {
struct IScriptingProvider;

class Input;

class ConfigService;

class ServerServiceSignals;

class MainLoopController;

class EmberEntity;

struct EntityTalk;
namespace Cegui {
class ColouredRenderedStringParser;

class CEGUILogger;

class CEGUISetup;
}
namespace OgreView {

class GUICEGUIAdapter;

class MovementController;

class World;

namespace Gui {
class QuickHelp;

class Widget;

class EntityIconManager;

class ActionBarIconManager;

class ActiveWidgetHandler;

class EntityTooltip;

class CursorWorldListener;

class WorldLoadingScreen;

class HitDisplayer;

class WidgetDefinitions;

namespace Icons {
class IconManager;
}
}

/**
 * @brief Responsible for all gui handling.

 This acts as the base and hub for all gui handling. It's responsible for both handling all widgets, setting up the CEGUI system and routing input event to it.

 It's a singleton so it can be accessed from all parts of the system.
 The root window of the CEGUI system can be accessed by the @see getMainSheet method.

 */
class GUIManager : public Singleton<GUIManager>, Ogre::FrameListener, public virtual sigc::trackable, public ConsoleObject {
public:

	static const std::string SCREENSHOT;
	static const std::string TOGGLEINPUTMODE;

	/**
	 * @brief Ctor.
	 * @param window The main window which will house the GUI. Normally the RenderWindow which makes up the complete application.
	 * @param configService The config service.
	 * @param serverSignals Server signals which signals when a new View has been created.
	 * @param mainLoopController The main loop controller for the system.
	 */
	GUIManager(Cegui::CEGUISetup& ceguiSetup, ConfigService& configService, ServerServiceSignals& serverSignals, MainLoopController& mainLoopController);

	/**
	 * @brief Dtor.
	 */
	~GUIManager() override;

	sigc::signal<void(const EntityTalk&, EmberEntity*)> AppendIGChatLine;
	sigc::signal<void(const std::string&, EmberEntity*)> AppendOOGChatLine;
	sigc::signal<void(const std::string&)> AppendAvatarImaginary;

	sigc::signal<void(const std::string&, EmberEntity*)> EventEntityAction;

	/**
	 @brief Emitted every frame.
	 */
	sigc::signal<void(float)> EventFrameStarted;

	/**
	 * @brief Renders the GUI.
	 *
	 * This should normally only be called from the main loop.
	 */
	void render() const;

	/**
	 * @brief Emits an action for a certain entity.
	 *    An action could be something like "touch" or "inspect". This is a rather undefined mechanism for easily letting different parts of the gui interact and react on common entity actions.
	 * @param action The name of the action.
	 * @param entity The entity for which we want to perform a certain action.
	 */
	void EmitEntityAction(const std::string& action, EmberEntity* entity);

	/**
	 *    Removed a widget from the system.
	 * @param widget
	 */
	void removeWidget(Gui::Widget* widget);

	/**
	 *    Adds a new widget to the system. This means it will receive FrameStarted events.
	 * @param widget
	 */
	void addWidget(Gui::Widget* widget);

	/**
	 *    Called by Ogre each frame.
	 * @param evt
	 * @return
	 */
	bool frameStarted(const Ogre::FrameEvent& evt) override;

	/**
	 * @brief Gets the root sheet of the CEGUI windowing system.
	 * @return
	 */
	CEGUI::Window* getMainSheet() const;

	/**
	 *    true if we're in GUI mode, which means that input events will be sent to the CEGUI system instead of the "world"
	 * @return
	 */
	bool isInGUIMode() const;

	/**
	 *    true if keyboard input should make the avatar move
	 *    this happens when wither 1) we're not in gui mode 2) the background sheet has the input control (thus, when something else, like an edit box has input control, that edit box will receive key strokes
	 * @return
	 */
	bool isInMovementKeysMode() const;

	/**
	 *    accessor for the Input instance object
	 * @return
	 */
	Input& getInput() const;

	/**
	 * @brief Gets the GUI renderer instance.
	 * @return The GUI renderer instance used by CEGUI.
	 */
	CEGUI::Renderer* getGuiRenderer() const;

	/**
	 *    Reimplements the ConsoleObject::runCommand method
	 * @param command
	 * @param args
	 */
	void runCommand(const std::string& command, const std::string& args) override;

	/**
	 *    returns the path to the directory where all layouts are stored
	 * @return
	 */
	static std::string getLayoutDir() ;

	/**
	 Creates a new window of the supplied type, giving it an autogenerated, unique name.
	 */
	CEGUI::Window* createWindow(const std::string& windowType);

	/**
	 Creates a new window of the supplied type with the supplied name.
	 */
	CEGUI::Window* createWindow(const std::string& windowType, const std::string& windowName);

	/**
	 *    Creates a new Widget
	 * @return
	 */
	Gui::Widget* createWidget();

	/**
	 * Destroys a widget previously created by createWidget
	 * @param widget The widget to destroy.
	 */
	void destroyWidget(Gui::Widget* widget);

	/**
	 * @brief Creates a new CEGUI texture from an existing Ogre texture.
	 * @param ogreTexture An existing Ogre texture.
	 * @param name The name of the new texture.
	 * @return A new CEGUI texture.
	 */
	CEGUI::Texture& createTexture(Ogre::TexturePtr& ogreTexture, std::string name = "");

	/**
	 *    Gets the name of the default scheme used (such as "EmberLook" or "WindowsLook")
	 * @return
	 */
	const std::string& getDefaultScheme() const;

	/**
	 Command for toggling between the input modes.
	 */
	const ConsoleCommandWrapper ToggleInputMode;

	/**
	 Command for reloading the gui.
	 */
	const ConsoleCommandWrapper ReloadGui;

	/**
	 * Command for Hiding/Showing the gui
	 */
	const ConsoleCommandWrapper ToggleGui;

	/**
	 *    Accessor for the IconManager which handles low level icons.
	 * @return The main IconManager
	 */
	Gui::Icons::IconManager* getIconManager() const;

	/**
	 *    Accessor for the EntityIconManager, which handles entity icons and slots.
	 * @return The main EntityIconManager
	 */
	Gui::EntityIconManager* getEntityIconManager() const;

	/**
	 *    Accessor for the ActionBarIconManager, which handles ActionBar icons and slots.
	 * @return The main ActionBarIconManager
	 */
	Gui::ActionBarIconManager* getActionBarIconManager() const;

	/**
	 * @brief Accessor for the entity tooltip instance, if such an instance has been created.
	 *
	 * @note The entity tooltip instance will only be available while a World instance is present in the system.
	 *
	 * @returns The entity tooltip instance if such exists, else null.
	 */
	Gui::EntityTooltip* getEntityTooltip() const;

protected:

	/**
	 Counter for autonaming of windows.
	 */
	static unsigned long msAutoGenId;

	Cegui::CEGUISetup& mCeguiSetup;

	/**
	 * @brief The config service.
	 */
	ConfigService& mConfigService;

	MainLoopController& mMainLoopController;

	InputCommandMapper mGuiCommandMapper;

	Gui::UniqueWindowPtr<CEGUI::Window> mSheet;
	CEGUI::WindowManager* mWindowManager;

	std::string mDefaultScheme;


	std::unique_ptr<Gui::CursorWorldListener> mCursorWorldListener;

	std::unique_ptr<Gui::HitDisplayer> mHitDisplayer;

	/**
	 Adapter for CEGUI which will send input events to CEGUI
	 */
	std::unique_ptr<GUICEGUIAdapter> mCEGUIAdapter;

	/**
	 * @brief Sets whether the GUI is enabled; i.e. will be rendered each frame.
	 * Defaults to true.
	 */
	bool mEnabled;


	std::unique_ptr<Gui::Icons::IconManager> mIconManager;
	std::unique_ptr<Gui::EntityIconManager> mEntityIconManager;
	std::unique_ptr<Gui::ActionBarIconManager> mActionBarIconManager;

	/**
	 all loaded widgets are stored here
	 */
	std::vector<std::unique_ptr<Gui::Widget>> mWidgets;

	/**
	 * @brief An instance of our own CEGUI RenderedStringParser which will better handle coloured strings.
	 *
	 * Owner by this instance.
	 */
	std::unique_ptr<Cegui::ColouredRenderedStringParser> mRenderedStringParser;
	std::unique_ptr<Gui::ActiveWidgetHandler> mActiveWidgetHandler;

	/**
	 * @brief Responsible for the help system
	 */
	std::unique_ptr<Gui::QuickHelp> mQuickHelp;

	/**
	 * @brief The entity tooltip instance, created when a World has been created, and destroyed along with it.
	 */
	std::unique_ptr<Gui::EntityTooltip> mEntityTooltip;

	/**
	 * @brief Bridges the CEGUI clipboard and the system one.
	 */
	std::unique_ptr<CEGUI::NativeClipboardProvider> mNativeClipboardProvider;

	/**
	 * @brief Provides loading screen to handle transitions
	 */
	std::unique_ptr<Gui::WorldLoadingScreen> mWorldLoadingScreen;

	std::unique_ptr<Gui::WidgetDefinitions> mWidgetDefinitions;

	struct InputCaptureWindow {
		CEGUI::Window* window = nullptr;
		std::vector<CEGUI::Event::Connection> connections;
	};

	InputCaptureWindow mInputCaptureWindow;

	/**
	 *    hooked to OgreView::EventCreatedAvatarEntity, switches the input mode to movement mode
	 * @param entity
	 */
	void EmberOgre_CreatedAvatarEntity(EmberEntity& entity);

	void EmberOgre_WorldCreated(World& world);

	void EmberOgre_WorldBeingDestroyed();

	void pressedKey(const SDL_KeyboardEvent& key, Input::InputMode inputMode);

	/**
	 * @brief Called when a new View instance is created.
	 *
	 * We'll wire up the view_EntityCreated() method call here.
	 *
	 * @param view The new view instance.
	 */
	void server_GotView(Eris::View* view);

	void entity_Emote(const std::string& description, EmberEntity* entity);

	/**
	 * Setup sounds that the UI make. Which is mostly click sounds.
	 */
	void setupUISounds();

};

}
}

#endif
