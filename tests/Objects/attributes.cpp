#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/loadDefaults.h>

#include <iostream>
#include <cassert>

using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Entity::Account;

int main(int argc, char** argv)
{
    Look rop;
    Account ac;

    assert(!rop->hasAttr("parents"));
    rop->setParents(std::list<std::string>(1, "foop"));
    assert(rop->hasAttr("parents"));
    
    assert(!rop->hasAttr("args"));
    rop->setArgs(std::vector<Root>(1, ac));
    assert(rop->hasAttr("args"));

    Root r;

    assert(!r->hasAttr("parents"));
    r->setParents(std::list<std::string>(1, "foo"));
    assert(r->hasAttr("parents"));

    assert(!r->hasAttr("name"));
    r->setName("bob");
    assert(r->hasAttr("name"));
}
