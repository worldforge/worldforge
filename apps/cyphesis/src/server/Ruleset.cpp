// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifdef HAVE_CONFIG_H

#include "config.h"

#endif

#include "Ruleset.h"

#include "EntityBuilder.h"
#include "EntityRuleHandler.h"
#include "OpRuleHandler.h"
#include "PropertyRuleHandler.h"
#include "ArchetypeRuleHandler.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/const.h"
#include "common/Inheritance.h"
#include "common/AtlasFileLoader.h"
#include "common/AssetsManager.h"
#include "Remotery.h"

#include <Atlas/Objects/Anonymous.h>

#include <boost/filesystem.hpp>

#include <iostream>
#include <chrono>
#include <boost/asio/steady_timer.hpp>
#include <Atlas/Objects/Factories.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;


typedef std::map<std::string, Root> RootDict;

static const bool debug_flag = false;

Ruleset::Ruleset(EntityBuilder& eb, boost::asio::io_context& io_context, PropertyManager& propertyManager) :
        m_entityHandler(new EntityRuleHandler(eb, propertyManager)),
        m_opHandler(new OpRuleHandler()),
        m_propertyHandler(new PropertyRuleHandler(propertyManager)),
        m_archetypeHandler(new ArchetypeRuleHandler(eb, propertyManager)),
        m_io_context(io_context)
{
}

Ruleset::~Ruleset()
{
    for (auto& timer :m_reloadTimers) {
        timer->cancel();
    }
}

int Ruleset::installRuleInner(const std::string& class_name,
                              const Root& class_desc,
                              std::string& dependent,
                              std::string& reason,
                              std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    assert(class_name == class_desc->getId());

    if (class_name.size() > consts::id_len) {
        spdlog::error("Rule \"{}\" has name longer than {} characters. "
                           "Skipping.", class_name, consts::id_len);
        return -1;
    }

    const std::string& parent = class_desc->getParent();
    if (parent.empty()) {
        spdlog::error("Rule \"{}\" has empty parent. Skipping.",
                           class_name);
        return -1;
    }
    int ret = -1;
    if (m_opHandler->check(class_desc) == 0) {
        ret = m_opHandler->install(class_name, parent, class_desc,
                                   dependent, reason, changes);
    } else if (m_entityHandler->check(class_desc) == 0) {
        ret = m_entityHandler->install(class_name, parent, class_desc,
                                       dependent, reason, changes);
    } else if (m_propertyHandler->check(class_desc) == 0) {
        ret = m_propertyHandler->install(class_name, parent, class_desc,
                                         dependent, reason, changes);
    } else if (m_archetypeHandler->check(class_desc) == 0) {
        ret = m_archetypeHandler->install(class_name, parent, class_desc,
                                          dependent, reason, changes);
    } else {
        spdlog::error(R"(Rule "{}" has unknown objtype="{}". Skipping.)",
                           class_name, class_desc->getObjtype());
        return -1;
    }

    return ret;
}

int Ruleset::installRule(const std::string& class_name,
                         const std::string& section,
                         const Root& class_desc)
{
    std::string dependent, reason;
    std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;
    // Possibly we should report some types of failure here.
    int ret = installRuleInner(class_name, class_desc, dependent, reason, changes);

    if (!changes.empty()) {
        for (auto& entry : changes) {
            Inheritance::instance().updateClass(entry.first->name(), entry.first->description(Visibility::PRIVATE));
            //TODO: write to user writable rule files
        }

        Inheritance::instance().typesUpdated(changes);
    }

    return ret;
}

void Ruleset::installItem(const std::string& class_name,
                          const Root& class_desc,
                          std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    std::string dependent, reason;
    int ret = installRuleInner(class_name, class_desc, dependent, reason, changes);
    if (ret != 0) {
        if (ret > 0) {
            waitForRule(class_name, class_desc, dependent, reason);
        }
        return;
    }

    // Install any rules that were waiting for this rule before they
    // could be installed
    auto I = m_waitingRules.lower_bound(class_name);
    auto Iend = m_waitingRules.upper_bound(class_name);
    RootDict readyRules;
    for (; I != Iend; ++I) {
        const std::string& wClassName = I->second.name;
        const Root& wClassDesc = I->second.desc;
        readyRules.emplace(wClassName, wClassDesc);
        cy_debug_print("WAITING rule " << wClassName
                        << " now ready from " << class_name);
    }
    m_waitingRules.erase(class_name);

    auto K = readyRules.begin();
    auto Kend = readyRules.end();
    for (; K != Kend; ++K) {
        const std::string& rClassName = K->first;
        const Root& rClassDesc = K->second;
        installItem(rClassName, rClassDesc, changes);
    }
}

int Ruleset::modifyRule(const std::string& class_name,
                        const Root& class_desc)
{
    std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;
    auto ret = modifyRuleInner(class_name, class_desc, changes);

    if (!changes.empty()) {
        for (auto& entry : changes) {
            Inheritance::instance().updateClass(entry.first->name(), entry.first->description(Visibility::PRIVATE));
            //TODO: write to user writable rule files
        }

        Inheritance::instance().typesUpdated(changes);
    }

    return ret;
}

int Ruleset::modifyRuleInner(const std::string& class_name,
                             const Root& class_desc,
                             std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
{
    assert(class_name == class_desc->getId());

    Root o = Inheritance::instance().getClass(class_name, Visibility::PRIVATE);
    if (!o.isValid()) {
        spdlog::error("Could not find existing type \"{}\" in "
                           "inheritance", class_name);
        return -1;
    }
    assert(!o->isDefaultParent());
    assert(!o->getParent().empty());
    if (class_desc->isDefaultParent() || class_desc->getParent().empty()) {
        spdlog::error("Updated type \"{}\" has no parent in its "
                           "description", class_name);
        return -1;
    }
    //TODO: allow changing of parents.
    if (class_desc->getParent() != o->getParent()) {
        spdlog::error("Updated type \"{}\" attempting to change parent "
                           "from {} to {}", class_name,
                           o->getParent(), class_desc->getParent());
        return -1;
    }
    int ret = -1;
    if (m_opHandler->check(class_desc) == 0) {
        ret = m_opHandler->update(class_name, class_desc, changes);
    } else if (m_entityHandler->check(class_desc) == 0) {
        ret = m_entityHandler->update(class_name, class_desc, changes);
    } else if (m_propertyHandler->check(class_desc) == 0) {
        ret = m_propertyHandler->update(class_name, class_desc, changes);
    }

    return ret;
}

void Ruleset::processChangedRules()
{
    rmt_ScopedCPUSample(processChangedRules, 0)
    if (!m_changedRules.empty()) {
        RootDict updatedRules;
        for (auto& path : m_changedRules) {
            try {
                spdlog::debug("Reloading rule file {}", path.string());
                auto& filename = path.native();
                AtlasFileLoader f(Inheritance::instance().getFactories(), filename, updatedRules);
                if (!f.isOpen()) {
                    spdlog::error("Unable to open rule file \"{}\".", filename);
                } else {
                    spdlog::info("Rule file \"{}\" reloaded.", filename);
                    f.read();
                }
            } catch (const std::exception& e) {
                spdlog::error("Error when reacting to changed file at '{}': {}", path.string(), e.what());
            }
        }
        if (!updatedRules.empty()) {
            std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;
            for (auto& entry: updatedRules) {
                auto& class_name = entry.first;
                auto& class_desc = entry.second;
                if (Inheritance::instance().hasClass(class_name)) {
                    spdlog::info("Updating existing rule \"{}\".", class_name);
                    modifyRuleInner(class_name, class_desc, changes);
                } else {
                    spdlog::info("Installing new rule \"{}\".", class_name);
                    installItem(class_name, class_desc, changes);
                }
            }
            if (!changes.empty()) {
                for (auto& entry : changes) {
                    Inheritance::instance().updateClass(entry.first->name(), entry.first->description(Visibility::PRIVATE));
                    //TODO: write to user writable rule files
                }

                Inheritance::instance().typesUpdated(changes);
            }
        }
        m_changedRules.clear();
    }
}

/// \brief Mark a rule down as waiting for another.
///
/// Note that a rule cannot yet be installed because it depends on something
/// that has not yet occurred, or a more fatal condition has occurred.
void Ruleset::waitForRule(const std::string& rulename,
                          const Root& ruledesc,
                          const std::string& dependent,
                          const std::string& reason)
{
    RuleWaiting rule;
    rule.name = rulename;
    rule.desc = ruledesc;
    rule.reason = reason;

    m_waitingRules.emplace(dependent, rule);
}

void Ruleset::getRulesFromFiles(boost::filesystem::path directory,
                                RootDict& rules)
{

    if (boost::filesystem::is_directory(directory)) {

        AssetsManager::instance().observeDirectory(directory, [&](const boost::filesystem::path& path) {
            m_changedRules.insert(path);

            auto timer = std::make_shared<boost::asio::steady_timer>(m_io_context);
            m_reloadTimers.insert(timer.get());
#if BOOST_VERSION >= 106600
            timer->expires_after(std::chrono::milliseconds(20));
#else
            timer->expires_from_now(std::chrono::milliseconds(20));
#endif
            timer->async_wait([&, timer](const boost::system::error_code& ec) {
                if (!ec) {
                    processChangedRules();
                    m_reloadTimers.erase(timer.get());
                }
            });

        });

        boost::filesystem::recursive_directory_iterator dir(directory), end;
        spdlog::info("Trying to load rules from directory '{}'", directory.string());

        int count = 0;
        while (dir != end) {
            if (boost::filesystem::is_regular_file(dir->status())) {
                auto filename = dir->path().native();
                AtlasFileLoader f(Inheritance::instance().getFactories(), filename, rules);
                if (!f.isOpen()) {
                    spdlog::error("Unable to open rule file \"{}\".", filename);
                } else {
                    f.read();
                    count += f.count();
                }
            }
            ++dir;
        }

        spdlog::info("Loaded {} rules.", count);
    }


}

void Ruleset::loadRules(const std::string& ruleset)
{
    boost::filesystem::path shared_rules_directory = boost::filesystem::path(share_directory) / "cyphesis" / "rulesets/" / ruleset / "rules";
    boost::filesystem::path var_rules_directory = boost::filesystem::path(var_directory) / "lib" / "cyphesis" / "rulesets" / ruleset / "rules";

    //Prefer rules from the "var" directory over the ones from the "etc" directory.
    RootDict ruleTable;
    getRulesFromFiles(var_rules_directory, ruleTable);
    RootDict shared_ruleTable;
    getRulesFromFiles(shared_rules_directory, shared_ruleTable);

    for (auto& entry : shared_ruleTable) {
        //Will only insert if a rule doesn't already exist.
        ruleTable.insert(entry);
    }

    if (ruleTable.empty()) {
        spdlog::error("Rule database table contains no rules.");
    }

    //Just ignore any changes, since this happens at startup before any clients are connected.
    std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;

    for (auto& entry : ruleTable) {
        const std::string& class_name = entry.first;
        const Root& class_desc = entry.second;
        installItem(class_name, class_desc, changes);
    }
    // Report on the non-cleared rules.
    // Perhaps we can keep them too?
    // m_waitingRules.clear();
    for (auto& entry : m_waitingRules) {
        spdlog::error(entry.second.reason);
    }
}
