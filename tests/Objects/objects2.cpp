#include <iostream>
#include <fstream>
#include <strstream>
#include <cassert>
#include <vector>

#include "../../src/Message/Object.h"
#include "../../src/Objects/Root.h"
#include "../../src/Objects/Entity.h"
#include "../../src/Objects/Operation.h"
#include "../../src/Objects/Encoder.h"
#include "../../src/Objects/Decoder.h"
#include "../../src/EncoderBase.h"
#include "../../src/Objects/LoadDefaults.h"
//#include "../../src/Net/Stream.h"
#include "../../tutorial/DebugBridge.h"

#define DEBUG_PRINT(foo) //foo;

using namespace Atlas;
using namespace Atlas::Objects;
using namespace Atlas::Message;
using namespace std;

#define USE_XML 1
#if USE_XML
#include "../../src/Codecs/XML.cpp"
#else
#include "../../src/Codecs/Packed.cpp"
#endif

#if USE_XML
#define USE_FILE 0
#else
#define USE_FILE 0
#endif

string Object2String(const Root& obj)
{
    DebugBridge bridge;
    strstream stream;
    Atlas::Codec<iostream> *codec;
    codec = new XML(Codec<iostream>::Parameters((iostream&)stream, &bridge));
    assert(codec);
    codec->StreamBegin();
    Objects::Encoder eno(codec);
    eno.StreamMessage(obj);
    codec->StreamEnd();
    delete codec;
    return stream.str();
}

class TestDecoder : public Decoder
{
protected:
    virtual void ObjectArrived(const Root& r)
    {
//        assert(r->GetAttr("id").AsString() == "root_instance");
      cout << "got Root! " << Object2String(r) << endl;
    }

    virtual void ObjectArrived(const Operation::Login& r)
    {
        cout << "got Account!" << endl;
//        assert(r->GetAttr("id").AsString() == "root_instance");
    }

    virtual void ObjectArrived(const Operation::Look& l)
    {
//        assert(l->GetAttr("id").AsString() == "look_instance");
        cout << "got Look!" << endl;
    }

    virtual void ObjectArrived(const Entity::Account &a)
    {
        cout << "got Account!" << endl;
    }
};

void testXML()
{
    Entity::RootEntity human;
    human->SetId("foo");

    Operation::Move move_op;
    move_op->SetFrom(string("bar"));
    vector<Root> move_args(1);
    move_args[0] = (Root&)human;
    move_op->SetArgs(move_args);

    Object::ListType velocity;
    velocity.push_back(2.0);
    velocity.push_back(1.0);
    velocity.push_back(0.0);
    human->SetVelocityAsList(velocity);

//    typedef BaseObjectData *(*alloc_func)();
//    alloc_func alloc_entity = &Entity::RootEntityDataInstance::alloc;
//    BaseObjectData *bod = alloc_entity();
    //Root human2(bod);
    Root human2 = (Root&)Entity::RootEntity::Factory();
    cout<<"human.id="<<human->GetId()<<endl;
    cout<<"human2.id="<<human2->GetId()<<endl;
#if 0
    typedef std::list<Atlas::Factory<Atlas::Codec<iostream> >*> FactoryCodecs;
    FactoryCodecs *myCodecs = &Factory<Codec<iostream> >::Factories();
    FactoryCodecs::iterator i;
    cout<<"myCodecs: "<<myCodecs->size();
    for (i = myCodecs->begin(); i != myCodecs->end(); ++i)
        cout<<":"<<(*i)->GetName();
    cout<<endl;
#endif

    //DebugBridge bridge;
    TestDecoder bridge;
#if USE_FILE
    ifstream stream;
    stream.open("../../../../protocols/atlas/spec/atlas.xml");
    assert(!!stream);
#else
    strstream stream;
#endif
//     typedef std::list<Atlas::Factory<Atlas::Codec<iostream> >*> FactoryCodecs;
//     FactoryCodecs *myCodecs = &Factory<Codec<iostream> >::Factories();
//     FactoryCodecs::iterator codec_i;
//     Atlas::Codec<iostream> *codec = NULL;
//     for(codec_i = myCodecs->begin(); codec_i != myCodecs->end(); ++codec_i)
//     {
//         cout<<(*codec_i)->GetName()<<endl;
//         if ((*codec_i)->GetName() == "XML") {
//             codec = (*codec_i)->New(Codec<iostream>::Parameters(stream, &bridge));
//         }
//     }
//     assert(codec);

    
    Objects::Entity::Account account;
    Objects::Operation::Login l;
    account->SetAttr("id", string("al"));
    account->SetAttr("password", string("ping"));
    //list<Message::Object> args(1,account->AsObject());
    //l->SetArgsAsList(args);
    vector<Root> args(1);
    args[0] = (Root&)account;
    l->SetArgs(args);
    //coder->StreamMessage((Root&)l);
//<map><list name="args"><map><string name="id">al</strin
//g></map></list><list name="parents"><string>root</string></list><string name="ob
//jtype">op_definition</string></map>


    Atlas::Codec<iostream> *codec;
#if USE_XML
    codec = new XML(Codec<iostream>::Parameters((iostream&)stream, &bridge));
#else
    codec = new Packed(Codec<iostream>::Parameters(stream, &bridge));
#endif
    assert(codec);

#if USE_FILE
    while(stream) {
      codec->Poll();
      //cout<<"--------"<<endl;
    }
#else
    codec->StreamBegin();

    Objects::Encoder eno(codec);
//    eno.StreamMessage((Root&)move_op);
    eno.StreamMessage((Root&)l);

//    Atlas::Message::Encoder en(codec);
//    en.StreamMessage(human->AsObject());

    codec->StreamEnd();
    cout<<endl<<stream.str()<<endl;
    //[$from=bar(args=[$id=foo])][$id=foo]
    //<atlas><map><string name="from">bar</string><list name="args"><map><string name="id">foo</string></map></list></map><map><string name="id">foo</string></map></atlas>
#endif
}


void check_float_list3(const Object::ListType &list,
                       double el1, double el2, double el3)
{
    assert( list.size() == 3 );
    Object::ListType::const_iterator i = list.begin();
    assert( (*i++) == el1 );
    assert( (*i++) == el2 );
    assert( (*i++) == el3 );
}

void testValues()
{
    Objects::Entity::Account account;
    Objects::Operation::Login l;
    account->SetId("al");
    account->SetAttr("password", string("ping"));
    l->SetArgs1((Root&)account);

    assert(l->GetArgs()[0]->GetLongDescription()=="Later in hierarchy tree objtype changes to 'object' when actual game objects are made.");
    assert(l->GetArgs()[0]->GetDescription()=="Base class for accounts");
    assert(l->GetId()=="");
    assert(l->GetParents().front()=="login");
    assert(l->GetObjtype()=="op");
    cout<<endl<<"acount.long_description: "<<l->GetArgs()[0]->GetLongDescription()<<endl;
    
    {
    Atlas::Message::Object::MapType mobj;
    Atlas::Message::Object::ListType parents;
    parents.push_back(string("account"));
    mobj["parents"] = parents;
    mobj["name"] = string("foo");
    mobj["objtype"] = string("op");
    Root obj = Atlas::Objects::MessageObject2ClassObject(mobj);
    assert(obj->GetClassNo() == Entity::ACCOUNT_NO);
    assert(obj->GetId() == "");
    assert(obj->IsDefaultId() == true);
    assert(obj->GetName() == "foo");
    assert(obj->IsDefaultName() == false);
    assert(obj->GetParents().front() == "account");
    //should this be true? modify MessageObject2ClassObject if yes
    assert(obj->IsDefaultParents() == false);
    assert(obj->GetObjtype() == "op");
    assert(obj->IsDefaultObjtype() == false); //should this be true? 
    assert(obj->GetDescription() == "Base class for accounts");
    assert(obj->IsDefaultDescription() == true);
    }
    
    {
    Atlas::Message::Object::MapType mobj;
    Root obj = Atlas::Objects::ObjectDefinitions.find(string("account"))->second;
    assert(obj->GetClassNo() == Entity::ACCOUNT_NO);
    assert(obj->GetId() == "account");
    assert(obj->IsDefaultId() == false);
    assert(obj->GetName() == "");
    assert(obj->IsDefaultName() == true);
    assert(obj->GetParents().front() == "admin_entity");
    assert(obj->IsDefaultParents() == false);
    assert(obj->GetObjtype() == "class");
    assert(obj->IsDefaultObjtype() == false);
    assert(obj->GetDescription() == "Base class for accounts");
    assert(obj->IsDefaultDescription() == false);
    }

    {
    Atlas::Message::Object::MapType mobj;
    mobj["id"] = string("account");
    mobj["name"] = string("foo");
    Root obj = Atlas::Objects::MessageObject2ClassObject(mobj);
    cout<<"Implement Empty class !!!!!!!!!!!!!!!!!!"<<endl;
#if 0 // do these when Empty class done
    assert(obj->GetClassNo() == Entity::ACCOUNT_NO);
    assert(obj->GetId() == "account");
    assert(obj->GetName() == "foo");
    assert(obj->GetParents().front() == "admin_entity");
    assert(obj->GetObjtype() == "class");
    assert(obj->GetDescription() == "Base class for accounts");
#endif
    }
}

void test()
{
    const double x1 = 3.5;
    const double y1 = -4.6;
    const double z1 = 2.0;

    const double x2 = 42.0;
    const double y2 = 7.0;

    vector<Entity::RootEntity> ent_vec(10);

    for(int i=0; i<10; i++) {
        DEBUG_PRINT(cout<<endl<<"round:"<<i<<endl);
        Entity::RootEntity human;

        //check for empty default:
        DEBUG_PRINT(cout<<"empty ok?"<<endl);
        Object::ListType empty = human->GetVelocityAsList();
        if(i==0) check_float_list3(empty, 0.0, 0.0, 0.0);
        else check_float_list3(empty, 0.0, y2, 0.0);
        
        //check after setting it
        DEBUG_PRINT(cout<<"setting ok?"<<endl);
        Object::ListType velocity;
        velocity.push_back(x1);
        velocity.push_back(y1);
        velocity.push_back(z1);
        check_float_list3(velocity, x1, y1, z1);
        human->SetVelocityAsList(velocity);
        Object::ListType foo = human->GetVelocityAsList();
        check_float_list3(foo, x1, y1, z1);

        DEBUG_PRINT(cout<<"changing it?"<<endl);
        vector<double> &foo2 = human->ModifyVelocity();
        *foo2.begin() = x2;
        check_float_list3(human->GetVelocityAsList(), x2, y1, z1);

        DEBUG_PRINT(cout<<"check change result?"<<endl);
        foo = human->GetVelocityAsList();
        check_float_list3(foo, x2, y1, z1);
        
        DEBUG_PRINT(cout<<"vector of entities?"<<endl);
        const Object::ListType &ent_velocity = ent_vec[i]->GetVelocityAsList();
        if(i==0) check_float_list3(ent_velocity, 0.0, 0.0, 0.0);
        else check_float_list3(ent_velocity, 0.0, y2, 0.0);

        DEBUG_PRINT(cout<<"base?"<<endl);
        Entity::RootEntity base_entity = human.GetDefaultObject();
        vector<double> &base = base_entity->ModifyVelocity();
        base[1] = y2;
        check_float_list3(base_entity->GetVelocityAsList(), 0.0, y2, 0.0);

        Operation::RootOperation move_op;
        vector<Root> move_args(1);
        move_args[0] = (Root&)human;
        move_op->SetArgs(move_args);

        Operation::RootOperation sight_op;
        //sight_op->SetFrom(humanent.AsObjectPtr());
        vector<Root> sight_args(1);
        sight_args[0] = (Root&)move_op;
        sight_op->SetArgs(sight_args);
        
        //test 
        DEBUG_PRINT(cout<<"get move_op?"<<endl);
        const vector<Root>& test_args = sight_op->GetArgs();
        assert(test_args.size() == 1);
        Operation::RootOperation test_op = 
            (Operation::RootOperation&)test_args[0];
        
        DEBUG_PRINT(cout<<"get human_ent?"<<endl);
        const vector<Root>& test_args2 = test_op->GetArgs();
        assert(test_args2.size() == 1);
        Entity::RootEntity test_ent =
            (Entity::RootEntity&)test_args2[0];
        Object::ListType foo3 = test_ent->GetVelocityAsList();
        check_float_list3(foo3, x2, y1, z1);
        
    }
}

int main()
{
    try {
        LoadDefaults("../../../../protocols/atlas/spec/atlas.xml");
    } catch(DefaultLoadingException e) {
        cout << "DefaultLoadingException: "
             << e.msg << endl;
        return 1;
    }
    testXML();
    testValues();
    test();
    return 0;
}
