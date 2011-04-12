#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/loadDefaults.h>

#include <iostream>
#include <cassert>

using Atlas::Objects::Root;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Entity::Account;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::objectDefinitions;

int main(int argc, char** argv)
{
    try {
	Atlas::Objects::loadDefaults("../../protocol/spec/atlas.xml");
    } catch(Atlas::Objects::DefaultLoadingException e) {
	std::cout << "DefaultLoadingException: "
             << e.getDescription() << std::endl;
    }
    Root root = Atlas::Objects::objectDefinitions.find("root")->second;
    Root root_inst;
    root_inst->setAttr("id", std::string("root_instantiation"));
    assert(root->getAttr("id").asString() == "root");
    assert(root_inst->getAttr("id").asString() == "root_instantiation");
    assert(root->getAttr("parents").asList().size() == 0);
    assert(root_inst->getAttr("parents").asList().size() == 1);
    assert((*root_inst->getAttr("parents").asList().begin()).asString() ==
            "root");

    Look look = smart_dynamic_cast<Look>(objectDefinitions.find("look")->second);
    Look look_inst;
    look_inst->setAttr("id", std::string("look_instantiation"));
    assert(look->getAttr("id").asString() == "look");
    assert(look_inst->getAttr("id").asString() == "look_instantiation");
    assert(look->getAttr("parents").asList().size() == 1);
    assert((*look->getAttr("parents").asList().begin()).asString() ==
            "perceive");
    assert(look_inst->getAttr("parents").asList().size() == 1);
    assert((*look_inst->getAttr("parents").asList().begin()).asString() ==
            "look");

    Account acct = smart_dynamic_cast<Account>(objectDefinitions.find("account")->second);
    Account acct_inst;
    acct_inst->setAttr("id", std::string("account_instantiation"));
    assert(acct->getAttr("id").asString() == "account");
    assert(acct_inst->getAttr("id").asString() == "account_instantiation");
    assert(acct->getAttr("parents").asList().size() == 1);
    assert((*acct->getAttr("parents").asList().begin()).asString() ==
            "admin_entity");
    assert(acct_inst->getAttr("parents").asList().size() == 1);
    assert((*acct_inst->getAttr("parents").asList().begin()).asString() ==
            "account");
}
