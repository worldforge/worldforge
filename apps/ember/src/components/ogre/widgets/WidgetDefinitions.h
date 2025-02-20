//
// C++ Interface: WidgetDefinitions
//
// Description: 
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2005
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.//
//
#ifndef WIDGETDEFINITIONS_H
#define WIDGETDEFINITIONS_H

#include <boost/dll.hpp>
#include <utility>
#include <set>
#include <map>
#include "WidgetPlugin.h"
#include <filesystem>


namespace Ember::OgreView {
class GUIManager;
namespace Gui {

class Widget;

/**
@author Erik Ogenvik

Utility class for registering Widgets.
If you create a new widget, make sure you add it to this class, else it won't be linked and you cannot create it dynamically.
*/
class WidgetDefinitions {
public:
	WidgetDefinitions();

	~WidgetDefinitions();

	void registerWidgets(GUIManager& guiManager);

	/**
	 * Registers a plugin with the supplied name. The system will try to find it by looking for a shared object in the file system,
	 * where plugins should be installed.
	 * @param guiManager
	 * @param pluginName
	 */
	void registerPluginWithName(GUIManager& guiManager, const std::string& pluginName);

	/**
	 * Registers a plugin from a shared object file.
	 * @param guiManager
	 * @param pluginPath
	 */
	void registerPlugin(GUIManager& guiManager, const std::filesystem::path& pluginPath);

	/**
	 * An entry for a loaded plugin.
	 * The pluginCallback will automatically be called on destruction.
	 */
	struct PluginEntry {
#ifdef WF_USE_WIDGET_PLUGINS

		explicit PluginEntry(std::filesystem::path pluginPath, WidgetPluginFunction dllPluginFn, WidgetPluginCallback callback) :
				path(std::move(pluginPath)),
				pluginFn(std::move(dllPluginFn)),
				pluginCallback(std::move(callback)) {}

#else
		explicit PluginEntry(WidgetPluginCallback callback) : pluginCallback(std::move(callback)) {}
#endif

		PluginEntry(PluginEntry&& rhs) = default;

		~PluginEntry();

		PluginEntry& operator=(PluginEntry&& rhs) = default;

#ifdef WF_USE_WIDGET_PLUGINS
		std::filesystem::path path;
		WidgetPluginFunction pluginFn; //We need to hold on to this, since the dynamic library's lifetime is bound to it.
#endif
		WidgetPluginCallback pluginCallback; //A deregistering function
	};

	static_assert(std::is_move_constructible<PluginEntry>::value, "PluginEntry must be move constructible.");

private:
	std::map<std::filesystem::path, PluginEntry> mPlugins;

	std::set<std::filesystem::path> mDirtyPluginPaths;

};
}
}


#endif
