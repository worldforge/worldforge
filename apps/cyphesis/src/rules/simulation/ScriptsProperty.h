//
// Created by erik on 2018-07-02.
//

#ifndef CYPHESIS_SCRIPTSPROPERTY_H
#define CYPHESIS_SCRIPTSPROPERTY_H

#include "common/Property.h"
#include "common/ScriptKit.h"
#include "rules/simulation/LocatedEntity.h"

#include <Atlas/Message/Element.h>

#include <memory>
#include <map>

/**
 * Specifies a list of scripts to be used by the entity.
 */
class ScriptsProperty : public Property<Atlas::Message::ListType, LocatedEntity> {
public:

	ScriptsProperty() = default;

	~ScriptsProperty() override = default;

	/**
	 * Cache of script factories.
	 */
	static std::map<std::pair<std::string, std::string>, std::shared_ptr<ScriptKit<LocatedEntity, LocatedEntity>>> sScriptFactories;

	/**
	 * Reloads all script factories (i.e. reload the scripts from disk).
	 */
	static void reloadAllScriptFactories();

	void set(const Atlas::Message::Element&) override;

	void apply(LocatedEntity&) override;

	void remove(LocatedEntity&, const std::string& name) override;

	/**
	 * Applies the scripts to the entity.
	 */
	void applyScripts(LocatedEntity&) const;

	ScriptsProperty* copy() const override;


protected:

	std::vector<std::shared_ptr<ScriptKit<LocatedEntity, LocatedEntity>>> m_scripts;

};


#endif //CYPHESIS_SCRIPTSPROPERTY_H
