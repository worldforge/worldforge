#include <iostream>
#include <cassert>
#include "Atlas/Message/Object.h"
#include "Atlas/Objects/Root.h"
#include "Atlas/Objects/Operation.h"
#include "Atlas/Objects/Entity.h"
#include "Atlas/Objects/objectFactory.h"
#include "Atlas/Objects/loadDefaults.h"

using namespace Atlas;
using namespace Atlas::Objects;
using namespace std;

int main(int argc, char** argv)
{
    try {
        loadDefaults("../../../../protocols/atlas/spec/atlas.xml");
    } catch(DefaultLoadingException e) {
        cout << "DefaultLoadingException: "
             << e.msg << endl;
        return 1;
    }
    Root root = objectDefinitions.find("root")->second;
    Root root_inst;
    root_inst->setAttr("id", string("root_instantiation"));
    assert(root->getAttr("id").asString() == "root");
    assert(root_inst->getAttr("id").asString() == "root_instantiation");
    assert(root->getAttr("parents").asList().size() == 0);
    assert(root_inst->getAttr("parents").asList().size() == 1);
    assert((*root_inst->getAttr("parents").asList().begin()).asString() ==
            "root");

    Operation::Look look = (Operation::Look&)objectDefinitions.find("look")->second;
    Operation::Look look_inst;
    look_inst->setAttr("id", string("look_instantiation"));
    assert(look->getAttr("id").asString() == "look");
    assert(look_inst->getAttr("id").asString() == "look_instantiation");
    assert(look->getAttr("parents").asList().size() == 1);
    assert((*look->getAttr("parents").asList().begin()).asString() ==
            "perceive");
    assert(look_inst->getAttr("parents").asList().size() == 1);
    assert((*look_inst->getAttr("parents").asList().begin()).asString() ==
            "look");

    Entity::Account acct = (Entity::Account&)objectDefinitions.find("account")->second;
    Entity::Account acct_inst;
    acct_inst->setAttr("id", string("account_instantiation"));
    assert(acct->getAttr("id").asString() == "account");
    assert(acct_inst->getAttr("id").asString() == "account_instantiation");
    assert(acct->getAttr("parents").asList().size() == 1);
    assert((*acct->getAttr("parents").asList().begin()).asString() ==
            "admin_entity");
    assert(acct_inst->getAttr("parents").asList().size() == 1);
    assert((*acct_inst->getAttr("parents").asList().begin()).asString() ==
            "account");
}
