/*
 Copyright (C) 2008  Alexey Torkhov <atorkhov@gmail.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "GUIAdapterBindings.h"
#include <Atlas/Formatter.h>
#include <Atlas/Codecs/XML.h>
#include <Atlas/Message/MEncoder.h>
#include <Atlas/Message/QueuedDecoder.h>
#include <framework/Log.h>

namespace Ember::OgreView::Authoring {
GUIAdapterBindings::GUIAdapterBindings() :
		mElement(nullptr) {

}

GUIAdapterBindings::~GUIAdapterBindings() = default;

const std::string& GUIAdapterBindings::getFunc() {
	return mFunc;
}

void GUIAdapterBindings::setFunc(std::string func) {
	mFunc = std::move(func);
}

void GUIAdapterBindings::addAdapter(std::string adapterName) {
	mAdapters.push_back(std::move(adapterName));
}

std::vector<std::string>& GUIAdapterBindings::getAdapters() {
	return mAdapters;
}

void GUIAdapterBindings::setValue(Atlas::Message::Element& val, TiXmlNode&) {
	TiXmlNode* parent = mElement->Parent();
	/*
	 TiXmlNode* newNode;
	 if (val.isNone())
	 {
	 // Bah! None! Let's forge out something
	 newNode = new TiXmlText("");
	 }
	 else if (val.isInt())
	 {
	 // itoa in C++ :)
	 std::ostringstream out;
	 out << val.asInt();
	 newNode = new TiXmlText(out.str());
	 }
	 else if (val.isFloat())
	 {
	 std::ostringstream out;
	 out << val.asFloat();
	 newNode = new TiXmlText(out.str());
	 }
	 else if (val.isString())
	 {
	 newNode = new TiXmlText(val.asString());
	 }
	 else if (val.isMap() || val.isList())
	 {
	 }
	 else
	 {
	 throw std::logic_error("New unknown Atlas element type seen in the wild for the first time.");
	 }
	 */
	// Got Atlas XML representation of adapter value
	auto xmlDoc = convertAtlasToXml(val);

	auto xmlNode = xmlDoc.RootElement();


	if (xmlNode->NoChildren()) {
		throw std::logic_error("Empty result from adapter.");
	}
	// Checking node validity
	TiXmlElement* newNode = xmlNode->ToElement()->FirstChildElement();
	if (newNode && xmlNode->FirstChild() == xmlNode->LastChild()) {
		const char* name = mElement->ToElement()->Attribute("name");

		// Saving "name" attribute of old node, if any
		if (name) {
			newNode->SetAttribute("name", name);
		}

		// Replacing placeholder node with received value
		mElement = parent->ReplaceChild(mElement, *newNode);
	} else {
		throw std::logic_error("Adapter returns Atlas message with multiply elements.");
	}
}

TiXmlDocument GUIAdapterBindings::convertAtlasToXml(Atlas::Message::Element& val) {
	// Print out Atlas node
	std::stringstream data;

	Atlas::Message::QueuedDecoder decoder;
	Atlas::Codecs::XML codec(data, data, decoder);
	Atlas::Formatter formatter(data, codec);
	Atlas::Message::Encoder encoder(formatter);
	formatter.streamBegin();
	encoder.listElementItem(val);
	formatter.streamEnd();

	logger->debug("  got adapter value {}", data.str());
	// Create TinyXml node
	TiXmlDocument xmlDoc;
	xmlDoc.Parse(data.str().c_str());
	if (xmlDoc.Error()) {
		throw std::logic_error("TinyXml unable to parse Atlas generated data.");
	}

	return xmlDoc;
}

}



