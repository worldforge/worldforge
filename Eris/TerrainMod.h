//
// C++ Interface: TerrainMod
//
// Description: The purpose of this class is to handle the bulk of the work
//		involved with using Mercator::TerrainMods. It handles parsing
//		the Atlas data and storing all the information needed by 
//		TerrainGenerator to add and remove them from the Terrain.
//
//		TerrainGenerator listens for changes in the modifier and
//		updates or removes the modifiers from the terrain as needed.
//
//
// Author: Tamas Bates <rhymer@gmail.com>, (C) 2008
// Author: Erik Hjortsberg <erik.hjortsberg@iteam.se>, (C) 2008
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
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.//
//
#ifndef ERIS_TERRAINMOD_H
#define ERIS_TERRAINMOD_H

#include <sigc++/signal.h>
#include <Eris/Entity.h>

namespace Mercator {
    class TerrainMod;
}

namespace Eris {

/**
@author Erik Hjortsberg <erik.hjortsberg@iteam.se>
@brief Base class for all terrain mod specific classes.This is not meant to be used directly by anything else than the TerrainMod class.
The TerrainMod class in itself doesn't hold the actual reference to the terrain mod, and doesn't handle the final parsing of Atlas data. This is instead handled by the different subclasses of this class. Since the different kinds of terrain mods found in Mercator behave differently depending on their type and the kind of shape used, we need to separate the code for handling them into different classes.
*/
class InnerTerrainMod
{
public:
    typedef enum shape { SHAPE_UNKNOWN,
                         SHAPE_ROTBOX,
                         SHAPE_POLYGON,
                         SHAPE_BALL } ShapeT;
    
    virtual ~InnerTerrainMod();
    
    const std::string& getTypename() const;

protected:
    template <template <int> class Shape>
    bool parseStuff(const WFMath::Point<3> & pos,
                    const WFMath::Quaternion & orientation,
                    const Atlas::Message::MapType& modElement,
                    Shape<2> & shape,
                    const Atlas::Message::Element & shapeElement);

public:

    bool parseData(const WFMath::Point<3> & pos,
                   const WFMath::Quaternion &,
                   const Atlas::Message::MapType &);
    
    /**
     * @brief Accessor for the Mercator::TerrainMod created and held by this instance.
     * If no terrain mod could be created, such as with faulty Atlas data, or if parseAtlasData() hasn't been called yet, this will return a null pointer.
     * @return A pointer to the TerrainMod held by this instance, or null if none created.
     */
    Mercator::TerrainMod* getModifier();

    InnerTerrainMod(const std::string& typemod);
    
protected:

    /**
     * @brief The type of mod this instance handles.
     * @see getTypename()
     */
    std::string mTypeName;

    static float parsePosition(const WFMath::Point<3> & pos, const Atlas::Message::MapType& modElement);

    template <template <int> class Shape>
    static bool parseShapeAtlasData(const Atlas::Message::Element& shapeElement,
                                    const WFMath::Point<3>& pos,
                                    const WFMath::Quaternion& orientation,
                                    Shape<2> & shape);

    template <template <int> class Shape,
              template <template <int> class Shape> class Mod>
    bool createInstance(const Atlas::Message::Element& shapeElement,
                        const WFMath::Point<3>& pos,
                        const WFMath::Quaternion& orientation,
                        const Atlas::Message::MapType &,
                        float,
                        float);

    template <template <int> class Shape,
              template <template <int> class Shape> class Mod>
    bool createInstance(const Atlas::Message::Element& shapeElement,
                        const WFMath::Point<3>& pos,
                        const WFMath::Quaternion& orientation,
                        const Atlas::Message::MapType &);

    Mercator::TerrainMod * m_mod;
};

class Entity;

/**
@author Tamas Bates
@author Erik Hjortsberg
@brief Wrapper class that envelopes a Mercator::TerrainMod.
This class is mainly responsible for parsing atlas data and create or update an instance of Mercator::TerrainMod with the correct data.
The actual application of the Mercator::TerrainMod to the terrain and the subsequent update of the rendering display (i.e. the Ogre terrain) is handled mainly by TerrainGenerator, which reacts to the events emitted by this class whenever a terrain mod changes or is moved.
After you've created an instance of this you must call the init() method.
*/
class TerrainMod
{
public:
    /**
    * @brief Ctor.
    * @param entity The entity to which this mod belongs.
    */
    TerrainMod(Entity* entity);

    /**
    * @brief Dtor.
    */
    virtual ~TerrainMod();
    
    /**
     * @brief Sets up the observation of the entity, and parses the mod info, creating the initial mod instance.
     * @param alwaysObserve If set to true, the observation of the entity will be set up even if the parsing failed. If false however, if there was an error during the parsing no observation will be set up. The calling code is then expected to delete this instance.
     * @return True if the atlas data was conformant and successfully parsed.
     */
    virtual bool init(bool alwaysObserve = false);

    /**
    *    @brief Used to retrieve a pointer to this modifier
    * @returns a pointer to this modifier
    */
    inline Mercator::TerrainMod* getMod() const;

    
    /**
    * @brief Emitted whenever the modifier is changed or moved.
    * Should be caught by a listener to apply the change to the terrain.
    */
    sigc::signal<void> ModChanged;

    /**
    * @brief Emitted just before the entity owning this mod is deleted.
    * Should be caught by a listener to remove this mod from the terrain.
    */
    sigc::signal<void> ModDeleted;
    
    /**
    * @brief Accessor for the entity to which this terrain mod belongs.
    * @return A pointer to the entity to which this terrain mod belongs.
    */
    Entity* getEntity() const;

protected:
    
    /**
    @brief The owner of this modifier.
    */
    Entity* mEntity;
    
    /**
    * @brief Slot used to listen for changes to attributes in the Entity to which this mod belongs to.
    */
    Entity::AttrChangedSlot mAttrChangedSlot;

    
    /**
    * @brief Called before the ModChanged signal is emitted.
    */
    virtual void onModChanged();
    
    /**
    * @brief Called before the ModDeleted signal is emitted.
    */
    virtual void onModDeleted();
    
    /**
    * @brief Called whenever a modifier is changed and handles the update
    * @param attributeValue The new Atlas data for the terrain mod
    */	
    void attributeChanged(const Atlas::Message::Element& attributeValue);

    /**
    *    @brief Called whenever a modifier is moved and handles the update
    */
    void entity_Moved();

    /**
    Called whenever the entity holding a modifier is deleted and handles
    removing the mod from the terrain
    */
    void entity_Deleted();

    /**
    * @brief Sets up the previous three handler functions to be called when a change
    * is made to the entity holding the modifier. 
    */
    virtual void observeEntity();

    /**
    * @brief Parses the Atlas data for a modifier
    * @returns True if it was able to successfully create a Mercator::TerrainMod, False otherwise
    * All work specific to a certain kind of TerrainMod is handled by the functions below.
    */
    virtual bool parseMod();
    
    /**
     * @brief If an existing mod changes we need to reparse it. Call this method in those cases.
     * If there's already an existing mod, that will be deleted. If the changes to the entity results in an invalid parsing, the Deleted signal will be emitted. If the parsing was successful however the Changed signal will be emitted.
     */
    virtual void reparseMod();
    

    /**
    * @brief The inner terrain mod instance which holds the actual Mercator::TerrainMod instance and handles the parsing of it.
    * In order to be able to better support different types of mods the actual instance will be any of the subclasses of InnerTerrainMod, depending on the type of the mod.
    */
    InnerTerrainMod* mInnerMod;
};

Mercator::TerrainMod* TerrainMod::getMod() const
{
    if (mInnerMod) {
        return mInnerMod->getModifier();
    }
    return 0;
}

}


#endif
