//
// C++ Interface: MapAdapter
//
// Description: 
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2007
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
#ifndef EMBEROGRE_GUI_ADAPTERS_ATLASMAPADAPTER_H
#define EMBEROGRE_GUI_ADAPTERS_ATLASMAPADAPTER_H

#include "AdapterBase.h"


namespace CEGUI {
class Window;
}


namespace Ember::OgreView::Gui::Adapters::Atlas {


/**
	@author Erik Ogenvik <erik@ogenvik.org>
*/
class MapAdapter : public AdapterBase {
public:

	MapAdapter(const ::Atlas::Message::Element& element, CEGUI::Window* childContainer);

	~MapAdapter() override;

	std::vector<std::string> getAttributeNames();

	const ::Atlas::Message::Element& valueOfAttr(const std::string& attr) const;

	bool hasAttr(const std::string& attr) const;

	/**
	Updates the gui with new values.
	*/
	void updateGui(const ::Atlas::Message::Element& element) override;

	void addAttributeAdapter(const std::string& attributeName, Adapters::Atlas::AdapterBase* adapter, CEGUI::Window* containerWindow);

	void removeAdapters();

	::Atlas::Message::Element getSelectedChangedElements();

	/**
	 * @brief Returns true if the map adapter already contains an adapter with the specified name.
	 * This works very much like hasAttr(), with the exception that this also will take into considerations those adapters that have been added but not yet applied to the underlying element.
	 * @param attr The name of the attribute to look for an adapter for.
	 * @return True if an adapter for the named attribute exists.
	 */
	bool hasAdapter(const std::string& attr) const;

protected:
	typedef std::map<std::string, AdapterWrapper> AdapterStore;

	CEGUI::Window* mChildContainer;
	const ::Atlas::Message::MapType& mAttributes;

	AdapterStore mAdapters;

	void fillElementFromGui() override;

	bool _hasChanges() override;

	::Atlas::Message::Element _getChangedElement() override;

};

}


#endif
