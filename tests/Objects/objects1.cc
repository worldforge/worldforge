#include <iostream>
#include <cassert>
#include "../../src/Message/Object.h"
#include "../../src/Objects/Root.h"
#include "../../src/Objects/Operation/Look.h"
#include "../../src/Objects/Entity/Account.h"

using namespace Atlas;
using namespace Atlas::Objects;
using namespace std;

int main(int argc, char** argv)
{
    Root root;
    Root root_inst = Root::Instantiate();
    root_inst.SetAttr("id", string("root_instantiation"));
    assert(root.GetAttr("id").AsString() == "root");
    assert(root_inst.GetAttr("id").AsString() == "root_instantiation");
    assert(root.GetAttr("parent").AsList().size() == 0);
    assert(root_inst.GetAttr("parent").AsList().size() == 1);
    assert((*root_inst.GetAttr("parent").AsList().begin()).AsString() ==
            "root");

    Operation::Look look;
    Operation::Look look_inst = Operation::Look::Instantiate();
    look_inst.SetAttr("id", string("look_instantiation"));
    assert(look.GetAttr("id").AsString() == "look");
    assert(look_inst.GetAttr("id").AsString() == "look_instantiation");
    assert(look.GetAttr("parent").AsList().size() == 1);
    assert((*look.GetAttr("parent").AsList().begin()).AsString() ==
            "perceive");
    assert(look_inst.GetAttr("parent").AsList().size() == 1);
    assert((*look_inst.GetAttr("parent").AsList().begin()).AsString() ==
            "look");

    Entity::Account acct;
    Entity::Account acct_inst = Entity::Account::Instantiate();
    acct_inst.SetAttr("id", string("account_instantiation"));
    assert(acct.GetAttr("id").AsString() == "account");
    assert(acct_inst.GetAttr("id").AsString() == "account_instantiation");
    assert(acct.GetAttr("parent").AsList().size() == 1);
    assert((*acct.GetAttr("parent").AsList().begin()).AsString() ==
            "admin_entity");
    assert(acct_inst.GetAttr("parent").AsList().size() == 1);
    assert((*acct_inst.GetAttr("parent").AsList().begin()).AsString() ==
            "account");
}
