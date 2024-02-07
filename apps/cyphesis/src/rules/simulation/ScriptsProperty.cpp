//
// Created by erik on 2018-07-02.
//

#include "ScriptsProperty.h"
#include "rules/simulation/LocatedEntity.h"
#include "rules/Script_impl.h"
#include "common/log.h"
#include "common/Property_impl.h"
#include "common/ScriptKit.h"


using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;


std::map<std::pair<std::string, std::string>, std::shared_ptr<ScriptKit<LocatedEntity, LocatedEntity>>> ScriptsProperty::sScriptFactories;


void ScriptsProperty::set(const Atlas::Message::Element& element) {
	Property::set(element);

	m_scripts.clear();
	for (auto& entry: m_data) {
		if (entry.isMap()) {
			std::string script_package;
			std::string script_class;
			if (Script<LocatedEntity>::getScriptDetails(entry.Map(), "<none>", "Entity",
														script_package, script_class) == 0) {
				auto key = std::make_pair(script_package, script_class);
				auto I = sScriptFactories.find(key);
				if (I != sScriptFactories.end()) {
					m_scripts.push_back(I->second);
				} else {
					auto psf = ScriptKit<LocatedEntity, LocatedEntity>::createScriptFactory(script_package, script_class);
					if (psf->setup() == 0) {
						sScriptFactories.emplace(key, psf);
						m_scripts.push_back(std::move(psf));
					} else {
						spdlog::error("Python class \"{}.{}\" failed to load",
									  script_package, script_class);
					}
				}
			}
		}
	}
}

void ScriptsProperty::apply(LocatedEntity& entity) {
	applyScripts(entity);
}

void ScriptsProperty::applyScripts(LocatedEntity& entity) const {

	entity.m_scripts.clear();
	for (auto& scriptFactory: m_scripts) {
		entity.setScript(scriptFactory->createScriptWrapper(entity));
	}
}

void ScriptsProperty::reloadAllScriptFactories() {
	for (auto& entry: sScriptFactories) {
		entry.second->refreshClass();
	}
}

void ScriptsProperty::remove(LocatedEntity& entity, const std::string& name) {
	entity.m_scripts.clear();
}

ScriptsProperty* ScriptsProperty::copy() const {
	return new ScriptsProperty(*this);
}




