//
// C++ Interface: Help
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
#ifndef EMBEROGREHELP_H
#define EMBEROGREHELP_H

#include "Widget.h"
#include "WidgetPlugin.h"
#include "framework/ConsoleCommandWrapper.h"

namespace Ember {
class EmberEntity;
namespace OgreView {
class GUIManager;
namespace Gui {
class Widget;


/**
@author Erik Ogenvik
*/
class Help : public Widget {
public:
	static WidgetPluginCallback registerWidget(Ember::OgreView::GUIManager& guiManager);

	explicit Help(GUIManager& guiManager);

	~Help() override;

	void runCommand(const std::string& command, const std::string& args) override;

	const ConsoleCommandWrapper HelpCommand;

protected:

	void show() override;

	/**
	 *    hooked to OgreView::EventCreatedAvatarEntity, show a help blurb about the movement mode
	 * @param entity
	 */
	void EmberOgre_CreatedAvatarEntity(EmberEntity& entity);

};
}
}
}
#ifdef WF_USE_WIDGET_PLUGINS

BOOST_DLL_ALIAS(
		Ember::OgreView::Gui::Help::registerWidget,
		registerWidget
)
#endif
#endif
