//
// C++ Implementation: XMLEntityRecipeSerializer
//
// Description:
//
//
// Author: Alexey Torkhov <atorkhov@gmail.com>, (C) 2008
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

#include "XMLEntityRecipeSerializer.h"
#include "EntityRecipeManager.h"
#include "GUIAdapterBindings.h"
#include "components/ogre/XMLHelper.h"

namespace Ember::OgreView::Authoring {
XMLEntityRecipeSerializer::XMLEntityRecipeSerializer() = default;

XMLEntityRecipeSerializer::~XMLEntityRecipeSerializer() = default;

std::map<std::string, std::unique_ptr<EntityRecipe>> XMLEntityRecipeSerializer::parseScript(Ogre::DataStreamPtr& stream) {
	TiXmlDocument xmlDoc;
	if (!Ember::OgreView::XMLHelper::Load(xmlDoc, stream)) {
		return {};
	}
	TiXmlElement* rootElem = xmlDoc.RootElement();

	std::map<std::string, std::unique_ptr<EntityRecipe>> entries;
	for (TiXmlElement* smElem = rootElem->FirstChildElement(); smElem != nullptr; smElem = smElem->NextSiblingElement()) {
		const char* tmp = smElem->Attribute("name");
		std::string name;
		if (!tmp) {
			continue;
		} else {
			name = tmp;
		}

		try {
			// Entity specification
			auto entitiesElement = smElem->FirstChildElement("entities");
			if (entitiesElement) {
				std::vector<std::unique_ptr<TiXmlElement>> entities;

				for (auto entityElem = entitiesElement->FirstChildElement(); entityElem; entityElem = entityElem->NextSiblingElement()) {
					entities.emplace_back(std::unique_ptr<TiXmlElement>(entityElem->Clone()->ToElement()));
				}

				auto entRecipe = std::make_unique<EntityRecipe>(std::move(entities));
				if (entRecipe) {
					entRecipe->mName = name;
					readRecipe(*entRecipe, smElem);
					entries.emplace(name, std::move(entRecipe));
				}
			}


		} catch (const std::exception& ex) {
			logger->error("Error when parsing entity recipe '{}': {}", name, ex.what());
		}
	}
	return entries;
}

void XMLEntityRecipeSerializer::readRecipe(EntityRecipe& entRecipe, TiXmlElement* recipeNode) {
	TiXmlElement* elem;

	// Author
	elem = recipeNode->FirstChildElement("author");
	if (elem) {
		const char* text = elem->GetText();
		if (text) {
			entRecipe.setAuthor(text);
		}
	}

	// Description
	elem = recipeNode->FirstChildElement("description");
	if (elem) {
		const char* text = elem->GetText();
		if (text) {
			entRecipe.setDescription(text);
		}
	}

	// GUI adapters
	elem = recipeNode->FirstChildElement("adapters");
	if (elem) {
		readAdapters(entRecipe, elem);
	}


	// Script
	elem = recipeNode->FirstChildElement("script");
	if (elem) {
		readScript(entRecipe, elem);
	}
}

void XMLEntityRecipeSerializer::readAdapters(EntityRecipe& entRecipe, TiXmlElement* adaptersNode) {
	logger->debug("Read adapters.");
	for (TiXmlElement* smElem = adaptersNode->FirstChildElement("adapter"); smElem != nullptr; smElem = smElem->NextSiblingElement("adapter")) {
		const std::string* name, * type, * tooltip, * defaultValue;
		std::string tooltipText;
		if (!(name = smElem->Attribute(std::string("name"))))
			continue;

		if (!(type = smElem->Attribute(std::string("type"))))
			continue;

		if ((tooltip = smElem->Attribute(std::string("tooltip")))) {
			tooltipText = *tooltip;
		}

		defaultValue = smElem->Attribute(std::string("default"));

		logger->debug(" adapter '{}' of type {}", *name, *type);

		auto adapter = std::make_unique<GUIAdapter>();
		adapter->mType = *type;
		adapter->mTooltip = tooltipText;

		const std::string* title;
		if ((title = smElem->Attribute(std::string("title")))) {
			adapter->mTitle = *title;
		}

		if (defaultValue) {
			adapter->mDefaultValue = *defaultValue;
		}

		// Custom adapter parameters
		if (*type == "string") {
			for (TiXmlElement* item = smElem->FirstChildElement("item"); item != nullptr; item = item->NextSiblingElement("item")) {
				const char* text;
				const std::string* value;
				text = item->GetText();
				if ((value = item->Attribute(std::string("value")))) {
					adapter->mSuggestions.emplace(*value, text);
				} else {
					adapter->mSuggestions.emplace(text, text);
				}
			}

			const std::string* allowRandom;
			if ((allowRandom = smElem->Attribute(std::string("allowrandom"))) && (*allowRandom == "yes" || *allowRandom == "true" || *allowRandom == "on")) {
				adapter->mAllowRandom = true;
			}
		}
		entRecipe.addGUIAdapter(*name, std::move(adapter));
	}
}

void XMLEntityRecipeSerializer::readBindAdapters(EntityRecipe& /*entRecipe*/, GUIAdapterBindings* bindings, TiXmlElement* bindAdaptersNode) {
	logger->debug("  Reading bind adapters.");
	for (TiXmlElement* elem = bindAdaptersNode->FirstChildElement(); elem != nullptr; elem = elem->NextSiblingElement()) {
		const std::string* name;

		if (!(name = elem->Attribute(std::string("name"))))
			continue;

		bindings->addAdapter(*name);
	}
}

void XMLEntityRecipeSerializer::readScript(EntityRecipe& entRecipe, TiXmlElement* scriptNode) {
	logger->debug("Read script.");
	const char* text = scriptNode->GetText();
	if (text) {
		entRecipe.mScript = text;
	}
}

}


