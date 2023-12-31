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

#ifndef EMBER_NUMBERRANGEADAPTER_H
#define EMBER_NUMBERRANGEADAPTER_H


#include <optional>
#include "AdapterBase.h"


namespace Ember::OgreView::Gui::Adapters::Atlas {

/**
	@author Erik Ogenvik <erik@ogenvik.org>
*/
class NumberRangeAdapter : public AdapterBase {
public:

	struct Windows {
		CEGUI::Editbox& MinWindow;
		CEGUI::Editbox& MaxWindow;
		CEGUI::Editbox& ValueWindow;
		CEGUI::PushButton& Randomize;
	};

	NumberRangeAdapter(const ::Atlas::Message::Element& element, Windows windows);

	~NumberRangeAdapter() override;

	/**
	Updates the gui with new values.
	*/
	void updateGui(const ::Atlas::Message::Element& element) override;

	void randomize() override;

protected:
	Windows mWindows;

	std::optional<float> mMin;
	std::optional<float> mMax;

	void fillElementFromGui() override;

};

}


#endif //EMBER_NUMBERRANGEADAPTER_H
