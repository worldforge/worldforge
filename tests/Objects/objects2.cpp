#include <iostream>
#include <cassert>
#include <vector>

#include "../../src/Message/Object.h"
#include "../../src/Objects/Root.h"
#include "../../src/Objects/Entity/RootEntity.h"
#include "../../src/Objects/Operation/RootOperation.h"

#define DEBUG_PRINT(foo) foo;

using namespace Atlas;
using namespace Atlas::Objects;
using namespace Atlas::Message;
using namespace std;

void check_float_list3(const Object::ListType &list,
                       double el1, double el2, double el3)
{
    assert( list.size() == 3 );
    Object::ListType::const_iterator i = list.begin();
    assert( (*i++) == el1 );
    assert( (*i++) == el2 );
    assert( (*i++) == el3 );
}

void test()
{
    const double x1 = 3.5;
    const double y1 = -4.6;
    const double z1 = 2.0;

    const double x2 = 42.0;
    const double y2 = 7.0;

    vector<Entity::RootEntityInstance> ent_vec(10);

    for(int i=0; i<10; i++) {
        DEBUG_PRINT(cout<<endl<<"round:"<<i<<endl);
        Entity::RootEntityInstance human;

        //check for empty default:
        DEBUG_PRINT(cout<<"empty ok?"<<endl);
        Object::ListType empty = human->GetVelocity();
        if(i==0) check_float_list3(empty, 0.0, 0.0, 0.0);
        else check_float_list3(empty, 0.0, y2, 0.0);
        
        //check after setting it
        DEBUG_PRINT(cout<<"setting ok?"<<endl);
        Object::ListType velocity;
        velocity.push_back(x1);
        velocity.push_back(y1);
        velocity.push_back(z1);
        check_float_list3(velocity, x1, y1, z1);
        human->SetVelocity(velocity);
        Object::ListType foo = human->GetVelocity();
        check_float_list3(foo, x1, y1, z1);

        DEBUG_PRINT(cout<<"changing it?"<<endl);
        Object::ListType &foo2 = human->GetVelocityRef();
        *foo2.begin() = x2;
        check_float_list3(foo2, x2, y1, z1);

        DEBUG_PRINT(cout<<"check change result?"<<endl);
        foo = human->GetVelocity();
        check_float_list3(foo, x2, y1, z1);
        
        DEBUG_PRINT(cout<<"vector of entities?"<<endl);
        const Object::ListType &ent_velocity = ent_vec[i]->GetVelocity();
        if(i==0) check_float_list3(ent_velocity, 0.0, 0.0, 0.0);
        else check_float_list3(ent_velocity, 0.0, y2, 0.0);

        DEBUG_PRINT(cout<<"base?"<<endl);
        Entity::RootEntityInstance base_entity = human.GetDefaultObject();
        Object::ListType &base = base_entity->GetVelocityRef();
        Object::ListType::iterator i_base = base.begin();
        i_base++;
        *i_base = y2;
        check_float_list3(base, 0.0, y2, 0.0);

        Operation::RootOperationInstance move_op;
        vector<Root> move_args(1);
        move_args[0] = (Root&)human;
        move_op->SetArgs(move_args);

        Operation::RootOperationInstance sight_op;
        //sight_op->SetFrom(humanent.AsObjectPtr());
        vector<Root> sight_args(1);
        sight_args[0] = (Root&)move_op;
        sight_op->SetArgs(sight_args);
        
        //test 
        DEBUG_PRINT(cout<<"get move_op?"<<endl);
        const vector<Root>& test_args = sight_op->GetArgs();
        assert(test_args.size() == 1);
        Operation::RootOperationInstance test_op = 
            (Operation::RootOperationInstance&)test_args[0];
        
        DEBUG_PRINT(cout<<"get human_ent?"<<endl);
        const vector<Root>& test_args2 = test_op->GetArgs();
        assert(test_args2.size() == 1);
        Entity::RootEntityInstance test_ent =
            (Entity::RootEntityInstance&)test_args2[0];
        Object::ListType foo3 = test_ent->GetVelocity();
        check_float_list3(foo3, x2, y1, z1);
        
    }
}

int main()
{
    test();
    return 0;
}
