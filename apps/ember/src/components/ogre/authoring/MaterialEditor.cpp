//
// C++ Implementation: MaterialEditor
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
#include "MaterialEditor.h"
#include "framework/Tokeniser.h"
#include <OgreStringConverter.h>
#include <OgreTechnique.h>
#include <OgreMaterialManager.h>
#include <framework/Log.h>

namespace Ember::OgreView::Authoring {
MaterialEditor::MaterialEditor() :
		AlterMaterial("alter_material", this, "Alters a material. Usage: <material name> <technique index> <pass index> [<tecture unit index>] <property> <value>") {
}

MaterialEditor::~MaterialEditor() = default;

void MaterialEditor::runCommand(const std::string& command, const std::string& args) {

	if (AlterMaterial == command) {
		try {
			Tokeniser tokeniser;
			tokeniser.initTokens(args);

			std::vector<std::string> tokens;
			std::string token;
			while (!(token = tokeniser.nextToken()).empty()) {
				tokens.push_back(token);
			}

			std::string materialName = tokens[0];

			Ogre::MaterialPtr materialPtr = static_cast<Ogre::MaterialPtr> (Ogre::MaterialManager::getSingleton().getByName(materialName));
			if (materialPtr) {
				std::string techniqueIndexString = tokens[1];
				if (!techniqueIndexString.empty()) {
					int techniqueIndex = Ogre::StringConverter::parseInt(techniqueIndexString);
					Ogre::Technique* technique = materialPtr->getTechnique(techniqueIndex);
					if (technique) {
						std::string passIndexString = tokens[2];
						if (!passIndexString.empty()) {
							int passIndex = Ogre::StringConverter::parseInt(passIndexString);
							Ogre::Pass* pass = technique->getPass(passIndex);
							//is texture unit specified
							if (tokens.size() == 6) {
								std::string textureUnitIndexString = tokens[3];
								//std::string property = tokens[4];
								//std::string value = tokens[5];

								int textureUnitIndex = Ogre::StringConverter::parseInt(textureUnitIndexString);

								Ogre::TextureUnitState* textureUnit = pass->getTextureUnitState(textureUnitIndex);
								if (textureUnit) {

								}
							} else {
								std::string property = tokens[3];
								std::string value = tokens[4];
								if (property == "alpha_rejection") {
									pass->setAlphaRejectValue(Ogre::StringConverter::parseInt(value));
								}
							}
						}
					}
				}
			}
		} catch (const std::exception& ex) {
			logger->warn("Error when altering material: ", ex.what());
		} catch (...) {
			logger->warn("Error when altering material.");
		}
	}
}

}


