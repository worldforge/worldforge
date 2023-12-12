//
// C++ Interface: OrientationAdapter
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
#ifndef EMBEROGRE_GUI_ADAPTERS_ATLASORIENTATIONADAPTER_H
#define EMBEROGRE_GUI_ADAPTERS_ATLASORIENTATIONADAPTER_H

#include "AdapterBase.h"


namespace Ember::OgreView::Gui::Adapters::Atlas {

/**
	@author Erik Ogenvik <erik@ogenvik.org>
*/
class OrientationAdapter : public AdapterBase {
public:
	OrientationAdapter(const ::Atlas::Message::Element& element, CEGUI::Window* xWindow, CEGUI::Window* yWindow, CEGUI::Window* zWindow, CEGUI::Window* scalarWindow);

	~OrientationAdapter() override;

	/**
	Updates the gui with new values.
	*/
	void updateGui(const ::Atlas::Message::Element& element) override;

protected:
	CEGUI::Window* mXWindow;
	CEGUI::Window* mYWindow;
	CEGUI::Window* mZWindow;
	CEGUI::Window* mScalarWindow;

	bool window_TextChanged(const CEGUI::EventArgs& e);

	void fillElementFromGui() override;

	bool _hasChanges() override;
};

}


#endif
