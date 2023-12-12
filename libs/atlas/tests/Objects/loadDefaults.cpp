// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Aloril,
// Copyright 2004-2005 Alistair Riddoch

// $Id$

#include "loadDefaults.h"

#include "Atlas/Objects/SmartPtr.h"
#include "Atlas/Objects/Anonymous.h"
#include "Atlas/Objects/Factories.h"
#include "Atlas/Message/DecoderBase.h"

#include "Atlas/Codecs/XML.h"

#include <fstream>
#include <set>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

namespace Atlas {
namespace Objects {

typedef std::map<std::string, Element> MessageElementMap;

struct LoadDefaultsDecoder : public Atlas::Message::DecoderBase {
	LoadDefaultsDecoder(const std::string& filename, const Factories& factories);

	const Element& getMessageElement(const std::string& id) const;

	void messageArrived(MapType) override;

	void setAttributes(Root& obj, //Root &obj_inst,
					   const Element& melem,
					   std::set<std::string> used_attributes);

	void fillDefaults();

	MessageElementMap m_objects;
	const Factories& m_factories;
	std::map<std::string, Root> objectDefinitions;
};

LoadDefaultsDecoder::LoadDefaultsDecoder(const std::string& filename, const Factories& factories)
		: m_factories(factories) {
	std::fstream stream;
	stream.open(filename.c_str(), std::ios::in);
	if (!stream) {
		throw DefaultLoadingException("Failed to open file " + filename);
	}

	//replace following code with:
	//getCodecByName(const std::string& name, Stream& stream, Bridge* bridge)

	//typedef list<Atlas::Factory<Atlas::Codec<std::iostream> >*> FactoryCodecs;
	//FactoryCodecs *myCodecs = Factory<Codec<std::iostream> >::factories();
	//FactoryCodecs::iterator i;
	//Atlas::Codec<std::iostream> *codec = nullptr;
	//for (i = myCodecs->begin(); i != myCodecs->end(); ++i)
	//if((*i)->getName() == "XML")
	//codec = (*i)->New(Codec<std::iostream>::Parameters((iostream&)stream, this));
	//end of replace
	{
		Atlas::Codecs::XML codec(stream, stream, *this);

		while (stream) {
			codec.poll();
		}
	}

	MapType anonymous_obj;
	m_objects["anonymous"] = anonymous_obj;
	MapType generic_obj;
	m_objects["generic"] = generic_obj;
	fillDefaults();
}

const Element& LoadDefaultsDecoder::getMessageElement(const std::string& id) const {
	auto I = m_objects.find(id);
	if (I == m_objects.end()) {
		throw DefaultLoadingException(id + " not found in XML file");
	} else {
		return (*I).second;
	}
}

void LoadDefaultsDecoder::messageArrived(MapType o) {
	MessageElementMap::const_iterator I = o.find("id");
	if (I == o.end()) {
		return;
	}

	std::string id(I->second.asString());
	m_objects[id] = std::move(o);
}

void LoadDefaultsDecoder::setAttributes(Root& obj, //Root &obj_inst,
										const Element& melem,
										std::set<std::string> used_attributes) {
	for (auto& entry: melem.asMap()) {
		auto attr_found = used_attributes.find(entry.first);
		if (attr_found == used_attributes.end()) {
			//cout<<"    -->"<<I->first<<endl;
			obj->setAttr(entry.first, entry.second, &m_factories);
			//obj_inst->setAttr(I->first, I->second);
		}
		used_attributes.insert(entry.first);
	}
	auto I = melem.asMap().find(Atlas::Objects::PARENT_ATTR);
	if (I != melem.asMap().end()) {
		const std::string& parent = I->second.asString();
		const Element& parent_melem = getMessageElement(parent);
		setAttributes(obj, /*obj_inst,*/ parent_melem, used_attributes);
	}
}

void LoadDefaultsDecoder::fillDefaults() {
	std::list<std::string> keys = m_factories.getKeys();
	for (auto& key: keys) {
		//cout<<(*I)<<endl;
		//get atlas.xml object
		const Element& melem = getMessageElement(key);
		//get class instances
		Root obj = m_factories.getDefaultInstance(key);
		//Root obj_inst = objectInstanceFactory.createObject(*I).getDefaultObject();
		//add attributes recursively
		std::set<std::string> used_attributes;
		setAttributes(obj, /*obj_inst,*/ melem, used_attributes);

		//add object definition
		Root obj_def = m_factories.createObject(key);
		obj_def->setObjtype(obj->getObjtype());
		for (auto& entry: melem.asMap()) {
			obj_def->setAttr(entry.first, entry.second, &m_factories);
		}
		objectDefinitions[obj_def->getId()] = obj_def;

		//modify attributes in instance that differ from definitions
		obj/*_inst*/->setParent(key);
		obj/*_inst*/->setId("");
		if (obj/*_inst*/->getObjtype() == "op_definition")
			obj/*_inst*/->setObjtype("op");
		else
			obj/*_inst*/->setObjtype("obj");
	}
}

std::map<std::string, Root> loadDefaults(const std::string& filename, const Factories& factories) {
	LoadDefaultsDecoder load_defaults(filename, factories);
	Entity::Anonymous e = Entity::AnonymousData::allocator.getDefaultObjectInstance();
	std::vector<double> coords(3, 0.0);
	e->setPos(coords);
	e->setVelocity(coords);
	return load_defaults.objectDefinitions;
}

}
} // namespace Atlas::Objects
