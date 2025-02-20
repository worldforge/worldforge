//
// C++ Interface: AreaAdapter
//
// Description: 
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2009
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
#ifndef EMBEROGRE_GUI_ADAPTERS_ATLASAREAADAPTER_H
#define EMBEROGRE_GUI_ADAPTERS_ATLASAREAADAPTER_H

#include "AdapterBase.h"
#include <wfmath/polygon.h>
#include <memory>

namespace CEGUI {
class Combobox;

class PushButton;
}

namespace Ember {
class EmberEntity;


namespace OgreView::Gui::Adapters::Atlas {

class PolygonAdapter;

/**
@brief An adapter for terrain areas.
This adapter will allow the user to edit the areas graphically through the use of the Polygon classes.
The polygon editing functionality is handled by an instance of PolygonAdapter, which this class wraps.
@author Erik Ogenvik <erik@ogenvik.org>
*/
class AreaAdapter : public AdapterBase {
public:
	/**
	 * @brief Ctor.
	 * @param element The Atlas element containing the initial data for the adapter.
	 * @param showButton A button which will toggle the display of the graphical representation of the area on and off.
	 * @param layerWindow A combox which will contain a list of the available layers which can be specified for the area.
	 * @param entity The entity to which the area belongs. This is needed to do height lookups so that the polygon points are snapped to the ground. It's optional, but the graphical representation might not work if it's omitted.
	 */
	AreaAdapter(const ::Atlas::Message::Element& element,
				CEGUI::PushButton* showButton,
				CEGUI::Combobox* layerWindow,
				CEGUI::ToggleButton* scaledWindow,
				EmberEntity* entity);

	/**
	 * @brief Dtor.
	 */
	~AreaAdapter() override = default;

	/**
	 * @brief Updates the gui with new values.
	 */
	void updateGui(const ::Atlas::Message::Element& element) override;

	/**
	 * @brief Toggles the display of the graphical representation of the polygon.
	 */
	void toggleDisplayOfPolygon();


	/**
	 * @brief Creates a new polygon.
	 */
	void createNewPolygon();

	/**
	 * @brief Adds an area suggestion.
	 * @param id The id of the area.
	 * @param name The name of the area, to be presented to the user.
	 */
	void addAreaSuggestion(int id, const std::string& name);

	/**
	 * @brief Clears all area suggestions.
	 */
	void clearAreaSuggestions();

protected:

	/**
	 * @brief The area layer.
	 */
	int mLayer;

	bool mIsScaled;

	/**
	 * @brief A combo box used for selecting the area layer to use.
	 */
	CEGUI::Combobox* mLayerWindow;

	CEGUI::ToggleButton* mScaledWindow;

	/**
	 * @brief An optional entity to which the area belongs.
	 * Mainly used for height lookups, so that the polygon snaps to the ground.
	 */
	EmberEntity* mEntity;

	/**
	 * @brief The polygon adapter, which handles the actual polygon editing.
	 */
	std::unique_ptr<PolygonAdapter> mPolygonAdapter;

	bool layerWindow_TextChanged(const CEGUI::EventArgs& e);

	bool layerWindow_ListSelectionChanged(const CEGUI::EventArgs& e);


	void fillElementFromGui() override;

	bool _hasChanges() override;

	/**
	 * @brief Creates a default area polygon for when there's none specified.
	 * @return A square polygon, 2 meters in each direction.
	 */
	static WFMath::Polygon<2> getDefaultPolygon();

};

}


}

#endif
