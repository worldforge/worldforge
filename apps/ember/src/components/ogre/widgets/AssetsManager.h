//
// C++ Interface: AssetsManager
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
#ifndef EMBEROGRE_GUIASSETSMANAGER_H
#define EMBEROGRE_GUIASSETSMANAGER_H

#include "TexturePair.h"
#include "PMInjectorSignaler.h"

#include <string>
#include <filesystem>

namespace CEGUI {

class Image;

class Texture;
}


namespace Ember::OgreView::Gui {


/**
	@brief A helper class for the AssetManager, providing resource related methods.
	
	An instance of this is used whenever there's a need for asset and resource management functionality. It's used mainly by AssetsManager.lua, but other widgets and classes are free to use it as well.
	The methods defined here are mainly basic resource handling methods that would be either redundant or too complicated to implement in Lua.
	Whenever you want to do something in Lua related to resource management that you feel will be overly complicated, or will require a lot of additional lua bindings, this is a good place to place it.
	@author Erik Ogenvik <erik@ogenvik.org>
*/
class AssetsManager {
public:
	AssetsManager();

	~AssetsManager();

	/**
	 * @brief Shorthand for quickly creating a CEGUI::OmageSet for an Ogre::Texture.
	 *
	 * This will create a new CEGUI Imageset and Image for the Ogre Texture with the supplied name.
	 *
	 * @see createTextureImage
	 * @param textureName The name of the texture to create a CEGUI representation of.
	 * @return A TexturePair struct, which basically just is a wrapper around both a Ogre::TexturePtr and a CEGUI::ImageSet amd a CEGUI::Image.
	 */
	TexturePair showTexture(const std::string& textureName);

	/**
	 *    @brief Creates a CEGUI texture from an ogre image.
	 *
	 * This is mainly useful when you want to provide a way to view different Ogre images in a CEGUI context. It will create a new CEGUI::ImageSet from the supplied image, containing just one image.
	 * @param texturePtr The Ogre texture which we want to view in CEGUI.
	 * @param imageSetName The new name of the imageset which will be created. This must be unique and not already created.
	 * @return A TexturePair struct, which basically just is a wrapper around both a Ogre::TexturePtr and a CEGUI::ImageSet amd a CEGUI::Image.
	 */
	TexturePair createTextureImage(Ogre::TexturePtr& texturePtr, const std::string& imageSetName);

	/**
	 *    @brief Returns a string representation of the material.
	 *
	 * Uses the Ogre::MaterialSerializer to serialize the material into a string definition, and returns it.
	 * @param material A valid material pointer which you want to see the string representation for.
	 * @return A string which is a representation of the material as defined in a .material file.
	 */
	std::string materialAsText(const Ogre::MaterialPtr& material);

	/**
	 * @brief Tries to resolve a file path to a local resource path.
	 * @param filePath A fully qualified file path.
	 * @returns A local resource path, if a match if found. Else the original file path.	 *
	 */
	std::string resolveResourceNameFromFilePath(const std::string& filePath);

	/**
	 * @brief Tries to resolve the file path for a mesh.
	 *
	 * @param meshPtr The mesh.
	 * @returns A fully qualified file name, or an empty string if none could be resolved.
	 */
	std::string resolveFilePathForMesh(const Ogre::MeshPtr& meshPtr);

	/**
	 * @brief Exports a mesh.
	 *
	 * @param mesh The mesh to export.
	 * @param filePath The path to export to, local on the file system.
	 * @returns True if the mesh was exported successfully.
	 */
	bool exportMesh(const Ogre::MeshPtr& mesh, const std::filesystem::path& filePath);

	/**
	 * Creates and exports a model from the mesh.
	 * @param mesh
	 */
	void createModel(const Ogre::MeshPtr& mesh);

	Lod::PMInjectorSignaler& getLodInjectorSignaler() {
		return *mPMInjectorSignaler;
	}

protected:
	/**
	 * @brief Signals, when a Lod is injected.
	 */
	std::unique_ptr<Lod::PMInjectorSignaler> mPMInjectorSignaler;

};

}


#endif
