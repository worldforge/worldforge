#ifndef WIDGET_H
#define WIDGET_H


#include "components/ogre/EmberOgrePrerequisites.h"
#include "framework/ConsoleCommandWrapper.h"
#include "CEGUIUtils.h"

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

namespace CEGUI {
class WindowManager;

class Listbox;

class ListboxItem;

class ListboxTextItem;

class Slider;

class Editbox;

class PushButton;

class MultiLineEditbox;

class Combobox;

class RadioButton;

}

namespace Ember::OgreView {

/**
Utility define for binding CEGUI elements to methods. 
*/
#define BIND_CEGUI_EVENT(window, event, method) window->subscribeEvent(event, CEGUI::Event::Subscriber(&method, this));


class GUIManager;

/**
 * @brief Namespace for any GUI related classes or activity.
 */
namespace Gui {

class Widget;


/**

@brief Base class for all widgets.
Put all widget set up code in the buildWidget() method.


NOTE: Perhaps we should provide another base class for widgets that have a single movable window?


When creating a new Widget class, make sure you also add it to WidgetDefinitions.
@see WidgetDefinitions
@author Erik Ogenvik <erik@ogenvik.org>
*/
class Widget :
		public virtual sigc::trackable,
		public ConsoleObject {
public:


	friend class ::Ember::OgreView::GUIManager;

	/**
	 * @brief Ctor.
	 */
	explicit Widget(GUIManager& guiManager);

	/**
	 * @brief Dtor.
	 */
	~Widget() override;


	/**
	 *    Called each frame.
	 * @param evt 
	 */
	void frameStarted(const Ogre::FrameEvent& evt);


	/**
	 *    Reimplements the ConsoleObject::runCommand method
	 * @param command 
	 * @param args 
	 */
	void runCommand(const std::string& command, const std::string& args) override;


	/**
	 *    Show the widget.
	 */
	virtual void show();

	/**
	 *    Hides the widget.
	 */
	virtual void hide();

	/**
	 * @brief If the widget is hidden, it gets shown. If it is visible, it gets hidden.
	 */
	virtual void toggleVisibility();

	/**
	 * @brief Checks if the window is visible using CEGUI isVisible.
	 * @return Visibility status of window.
	 */
	virtual bool isVisible();

	/**
	 * @brief Checks if the window is active.
	 * @return Is the window active.
	 */
	virtual bool isActive();

	/**
	 * @brief Gets the window with the supplied name from the WindowManager. Note that you shouldn't supply the prefix (as defined in loadMainSheet) since that will be added by the method.
	 * @param windowName The name of the window, without the suffix.
	 * @param throwIfNotFound Throw an exception if the window isn't found.
	 * @return A valid Window pointer, or null if no window could be found, and throwIfNotFound is false.
	 */
	CEGUI::Window* getWindow(const std::string& windowName, bool throwIfNotFound = false);

	template<typename T>
	T& getWindow(const std::string& windowName);

	/**
	Creates a new window of the supplied type, giving it an autogenerated, unique name.
	*/
	CEGUI::Window* createWindow(const std::string& windowType);

	/**
	Creates a new window of the supplied type with the supplied name.
	*/
	CEGUI::Window* createWindow(const std::string& windowType, const std::string& windowName);


	/**
	 *    Call this method upon creation of the widget (for example in buildWidget) to enable the close button and wire it to the correct methods.
	 */
	void enableCloseButton();

	/**
	 *    Call this method upon creation of the widget (for example in buildWidget) to register show and hide commands with the console.
	 *    The command you choose will be prefixed by "show_" and "hide_". So if you have a widget which shows a map and you call this method with the
	 *    parameter "map" the commands "show_map" and "hide_map" will be registered with the console.
	 * @param commandSuffix a string to be prefixed by "show_" and "hide_"
	 */
	void registerConsoleVisibilityToggleCommand(const std::string& commandSuffix);

	/**
	accessor to the main sheet of the gui system
	*/
	CEGUI::Window* getMainSheet();

	/**
	 *    Accessor to the main window of the widget.
	 * @return 
	 */
	CEGUI::Window* getMainWindow();

	/**
	 *    Loads a widget definition from a file and sets the main sheet
	 * @param filename The name of the file to load
	 * @param prefix The prefix to use
	 * @return
	 */
	CEGUI::Window* loadMainSheet(const std::string& filename, const std::string& prefix);

	/**
	 *    Gets the prefix used in the widget definition
	 * @return 
	 */
	const std::string& getPrefix() const;

	/**
	 *    Gets whether the window when being activated should become fully opaque, to return to it's preset alpha value when being deactivated.
	 Defaults to true.
	 * @return 
	 */
	bool getIsActiveWindowOpaque() const;

	/**
	 *    Sets whether the window when being activated should become fully opaque, to return to it's preset alpha value when being deactivated.
	 Defaults to true.
	 * @param isOpaque 
	 */
	void setIsActiveWindowOpaque(bool isOpaque);

	/**
	 *    Gets the name of the default scheme used (such as "EmberLook" or "EmberLook")
	 * @return 
	 */
	const std::string& getDefaultScheme() const;


	/**
	 *    Adds a window to the tab order. Remember to call closeTabGroup() to close the tab group.
	 * @param window 
	 */
	void addTabbableWindow(CEGUI::Window* window);

	/**
	 * Adds a window to the "enter" list. If the "Enter" key is pressed within any of the tabbable windows, any "enter" window that is visible will have a MouseClick event sent to it. Use this to enable default enter behaviour in forms.
	 */
	void addEnterButton(CEGUI::Window* window);


	/**
	 *    Closes the current tab so that the last window connects to the first window.
	 */
	void closeTabGroup();

	GUIManager& getGUIManager() {
		return mGuiManager;
	}

	/**
	 * @brief Emitted each time a frame is started.
	 */
	sigc::signal<void(float)> EventFrameStarted;

	/**
	 * @brief Emitted the first time the window is shown.
	 */
	sigc::signal<void()> EventFirstTimeShown;

protected:

	bool MainWindow_CloseClick(const CEGUI::EventArgs& args);

	bool MainWindow_Activated(const CEGUI::EventArgs& args);

	bool MainWindow_Deactivated(const CEGUI::EventArgs& args);

	bool MainWindow_MouseButtonDown(const CEGUI::EventArgs& args);

	/**
	 * @brief Listen to the window being shown, so that we can emit the EventFirstTimeShown the first time the window is shown.
	 * @param args 
	 * @return 
	 */
	bool MainWindow_Shown(const CEGUI::EventArgs& args);

	/**
	 * @brief Called when the EventFirstTimeShown signal should be emitted.
	 */
	virtual void onEventFirstTimeShown();


	/**
	* @brief The suffixed used by registerConsoleVisibilityToggleCommand
	* @see registerConsoleVisibilityToggleCommand
	*/
	std::string mCommandSuffix;


	/**
	* @brief The main window for the widget.
	*/
	UniqueWindowPtr<CEGUI::Window> mMainWindow;

	GUIManager& mGuiManager;

	/**
	The original alpha value of the window.
	*/
	float mOriginalWindowAlpha;

	/**
	If true, when activated the window will become fully opaque.
	*/
	bool mActiveWindowIsOpaque;

	typedef std::map<CEGUI::Window*, CEGUI::Window*> WindowMap;
	typedef std::vector<CEGUI::Window*> WindowStore;
	WindowMap mTabOrder;
	WindowStore mEnterButtons;
	CEGUI::Window* mFirstTabWindow, * mLastTabWindow;

	bool TabbableWindow_KeyDown(const CEGUI::EventArgs& args);

	/**
	 * @brief True if the window has been shown once.
	 * This is used for making sure that the EventFirstTimeShown signal is emitted the first time the window is shown.
	 */
	bool mWindowHasBeenShown;


private:
	std::string mPrefix;

};

inline const std::string& Widget::getPrefix() const { return mPrefix; }

template<typename T>
T& Widget::getWindow(const std::string& windowName) {
	if (!mMainWindow) {
		logger->warn("Trying to get a window ({}) on widget that has no main sheet loaded ({}).", windowName, mPrefix);
		throw std::runtime_error("No main sheet loaded.");
	}
	auto window = dynamic_cast<T*>(mMainWindow->getChildRecursive(windowName));
	if (!window) {
		logger->warn("The window with id {}, located under widget {} does not exist.", windowName, mPrefix);
		throw std::runtime_error("No window found.");
	}
	return *window;
}

}


}


#endif // WIDGET_H
