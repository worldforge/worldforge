#include <iostream>
#include <cassert>
#include "../../src/Message/Object.h"
#include "../../src/Objects/Root.h"
#include "../../src/Objects/Operation.h"
#include "../../src/Objects/Entity.h"
#include "../../src/Objects/ObjectFactory.h"
#include "../../src/Objects/LoadDefaults.h"

using namespace Atlas;
using namespace Atlas::Objects;
using namespace std;

#include "../../src/Codecs/XML.cpp"

int main(int argc, char** argv)
{
    try {
        LoadDefaults("../../../../protocols/atlas/spec/atlas.xml");
    } catch(DefaultLoadingException e) {
        cout << "DefaultLoadingException: "
             << e.msg << endl;
        return 1;
    }
    Root root = ObjectDefinitions.find("root")->second;
    Root root_inst;
    root_inst->SetAttr("id", string("root_instantiation"));
    assert(root->GetAttr("id").AsString() == "root");
    assert(root_inst->GetAttr("id").AsString() == "root_instantiation");
    assert(root->GetAttr("parents").AsList().size() == 0);
    assert(root_inst->GetAttr("parents").AsList().size() == 1);
    assert((*root_inst->GetAttr("parents").AsList().begin()).AsString() ==
            "root");

    Operation::Look look = (Operation::Look&)ObjectDefinitions.find("look")->second;
    Operation::Look look_inst;
    look_inst->SetAttr("id", string("look_instantiation"));
    assert(look->GetAttr("id").AsString() == "look");
    assert(look_inst->GetAttr("id").AsString() == "look_instantiation");
    assert(look->GetAttr("parents").AsList().size() == 1);
    assert((*look->GetAttr("parents").AsList().begin()).AsString() ==
            "perceive");
    assert(look_inst->GetAttr("parents").AsList().size() == 1);
    assert((*look_inst->GetAttr("parents").AsList().begin()).AsString() ==
            "look");

    Entity::Account acct = (Entity::Account&)ObjectDefinitions.find("account")->second;
    Entity::Account acct_inst;
    acct_inst->SetAttr("id", string("account_instantiation"));
    assert(acct->GetAttr("id").AsString() == "account");
    assert(acct_inst->GetAttr("id").AsString() == "account_instantiation");
    assert(acct->GetAttr("parents").AsList().size() == 1);
    assert((*acct->GetAttr("parents").AsList().begin()).AsString() ==
            "admin_entity");
    assert(acct_inst->GetAttr("parents").AsList().size() == 1);
    assert((*acct_inst->GetAttr("parents").AsList().begin()).AsString() ==
            "account");
}
