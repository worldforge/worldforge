#include <iostream>
#include <fstream>
#include <strstream>
#include <cassert>
#include <vector>

#include "Message/Object.h"
#include "Objects/Root.h"
#include "Objects/Entity.h"
#include "Objects/Operation.h"
#include "Objects/Encoder.h"
#include "Objects/Decoder.h"
#include "EncoderBase.h"
#include "Objects/loadDefaults.h"
//#include "../../src/Net/Stream.h"
#include "../../tutorial/DebugBridge.h"

#define DEBUG_PRINT(foo) //foo;

using namespace Atlas;
using namespace std;

#define USE_XML 1
#if USE_XML
#include "../../src/Codecs/XML.h"
#else
#include "../../src/Codecs/Packed.h"
#endif

#if USE_XML
#define USE_FILE 0
#else
#define USE_FILE 0
#endif

string object2String(const Root& obj)
{
    DebugBridge bridge;
    strstream stream;
    Atlas::Codec *codec;
    codec = new XML(Codec::Parameters((iostream&)stream, &bridge));
    assert(codec);
    codec->streamBegin();
    ObjectsEncoder eno(codec);
    eno.streamMessage(obj);
    codec->streamEnd();
    delete codec;
    return stream.str();
}

class TestDecoder : public ObjectsDecoder
{
protected:
    virtual void objectArrived(const Root& r)
    {
//        assert(r->getAttr("id").asString() == "root_instance");
      cout << "got Root! " << object2String(r) << endl;
    }

    virtual void objectArrived(const Login& r)
    {
        cout << "got Account!" << endl;
//        assert(r->getAttr("id").asString() == "root_instance");
    }

    virtual void objectArrived(const Look& l)
    {
//        assert(l->getAttr("id").asString() == "look_instance");
        cout << "got Look!" << endl;
    }

    virtual void objectArrived(const Account &a)
    {
        cout << "got Account!" << endl;
    }
};

void testXML()
{
    RootEntity human;
    human->setId("foo");

    Move move_op;
    move_op->setFrom(string("bar"));
    vector<Root> move_args(1);
    move_args[0] = (Root&)human;
    move_op->setArgs(move_args);

    Object::ListType velocity;
    velocity.push_back(2.0);
    velocity.push_back(1.0);
    velocity.push_back(0.0);
    human->setVelocityAsList(velocity);

//    typedef BaseObjectData *(*alloc_func)();
//    alloc_func alloc_entity = &Entity::RootEntityDataInstance::alloc;
//    BaseObjectData *bod = alloc_entity();
    //Root human2(bod);
    Root human2 = (Root&)RootEntity::factory();
    cout<<"human.id="<<human->getId()<<endl;
    cout<<"human2.id="<<human2->getId()<<endl;
#if 0
    typedef std::list<Atlas::Factory<Atlas::Codec >*> FactoryCodecs;
    FactoryCodecs *myCodecs = &Factory<Codec >::factories();
    FactoryCodecs::iterator i;
    cout<<"myCodecs: "<<myCodecs->size();
    for (i = myCodecs->begin(); i != myCodecs->end(); ++i)
        cout<<":"<<(*i)->getName();
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
//     typedef std::list<Atlas::Factory<Atlas::Codec >*> FactoryCodecs;
//     FactoryCodecs *myCodecs = &Factory<Codec >::factories();
//     FactoryCodecs::iterator codec_i;
//     Atlas::Codec *codec = NULL;
//     for(codec_i = myCodecs->begin(); codec_i != myCodecs->end(); ++codec_i)
//     {
//         cout<<(*codec_i)->getName()<<endl;
//         if ((*codec_i)->getName() == "XML") {
//             codec = (*codec_i)->New(Codec::Parameters(stream, &bridge));
//         }
//     }
//     assert(codec);

    
    Account account;
    Login l;
    account->setAttr("id", string("al"));
    account->setAttr("password", string("ping"));
    //list<Message::Object> args(1,account->asObject());
    //l->setArgsAsList(args);
    vector<Root> args(1);
    args[0] = (Root&)account;
    l->setArgs(args);
    //coder->streamMessage((Root&)l);
//<map><list name="args"><map><string name="id">al</strin
//g></map></list><list name="parents"><string>root</string></list><string name="ob
//jtype">op_definition</string></map>


    Atlas::Codec *codec;
#if USE_XML
    codec = new XML(Codec::Parameters((iostream&)stream, &bridge));
#else
    codec = new Packed(Codec::Parameters(stream, &bridge));
#endif
    assert(codec);

#if USE_FILE
    while(stream) {
      codec->poll();
      //cout<<"--------"<<endl;
    }
#else
    codec->streamBegin();

    ObjectsEncoder eno(codec);
//    eno.streamMessage((Root&)move_op);
    eno.streamMessage((Root&)l);

    Anonymous e;
    eno.streamMessage((Root&)e);
    e->setId("foo");
    eno.streamMessage((Root&)e);
//    Atlas::Message::Encoder en(codec);
//    en.streamMessage(human->asObject());

    codec->streamEnd();
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
    Account account;
    Login l;
    account->setId("al");
    account->setAttr("password", string("ping"));
    l->setArgs1((Root&)account);

    assert(l->getArgs()[0]->getLongDescription()=="Later in hierarchy tree objtype changes to 'object' when actual game objects are made.");
    assert(l->getArgs()[0]->getDescription()=="Base class for accounts");
    assert(l->getId()=="");
    assert(l->getParents().front()=="login");
    assert(l->getObjtype()=="op");
    cout<<endl<<"account.long_description: "
        <<l->getArgs()[0]->getLongDescription()<<endl;
    
    {
    Atlas::Object::MapType mobj;
    Atlas::Object::ListType parents;
    parents.push_back(string("account"));
    mobj["parents"] = parents;
    mobj["name"] = string("foo");
    mobj["objtype"] = string("op");
    Root obj = Atlas::messageObject2ClassObject(mobj);
    assert(obj->getClassNo() == ACCOUNT_NO);
    assert(obj->getId() == "");
    assert(obj->isDefaultId() == true);
    assert(obj->getName() == "foo");
    assert(obj->isDefaultName() == false);
    assert(obj->getParents().front() == "account");
    //should this be true? modify MessageObject2ClassObject if yes
    assert(obj->isDefaultParents() == false);
    assert(obj->getObjtype() == "op");
    assert(obj->isDefaultObjtype() == false); //should this be true? 
    assert(obj->getDescription() == "Base class for accounts");
    assert(obj->isDefaultDescription() == true);
    }
    
    {
    Atlas::Object::MapType mobj;
    Root obj = Atlas::objectDefinitions.find(string("account"))->second;
    assert(obj->getClassNo() == ACCOUNT_NO);
    assert(obj->getId() == "account");
    assert(obj->isDefaultId() == false);
    assert(obj->getName() == "");
    assert(obj->isDefaultName() == true);
    assert(obj->getParents().front() == "admin_entity");
    assert(obj->isDefaultParents() == false);
    assert(obj->getObjtype() == "class");
    assert(obj->isDefaultObjtype() == false);
    assert(obj->getDescription() == "Base class for accounts");
    assert(obj->isDefaultDescription() == false);
    }

    {
    Atlas::Object::MapType mobj;
    Root obj = Atlas::messageObject2ClassObject(mobj);
    assert(obj->getClassNo() == ANONYMOUS_NO);
    assert(obj->getId() == "");
    assert(obj->getName() == "");
    assert(obj->getParents().size() == 0);
    assert(obj->getObjtype() == "obj");
    assert(obj->getDescription() == "");
    }

    {
    Atlas::Object::MapType mobj;
    mobj["id"] = string("bar");
    mobj["name"] = string("foo");
    Atlas::Object::ListType parents;
    parents.push_back(string("account"));
    mobj["parents"] = parents;
    Root obj = Atlas::messageObject2ClassObject(mobj);
    assert(obj->getClassNo() == ANONYMOUS_NO);
    assert(obj->getId() == "bar");
    assert(obj->getName() == "foo");
    assert(obj->getParents().front() == "account");
    assert(obj->getObjtype() == "obj");
    assert(obj->getDescription() == "");
    }

    {
    Atlas::Object::MapType maccount;
    maccount["id"] = string("bar");
    maccount["name"] = string("foo");
    Atlas::Object::ListType parents;
    parents.push_back(string("player"));
    maccount["parents"] = parents;
    maccount["objtype"] = "obj";

    Atlas::Object::MapType mcreate;
    mcreate["from"] = string("bar");
    Atlas::Object::ListType parents2;
    parents2.push_back(string("create"));
    mcreate["parents"] = parents2;
    Atlas::Object::ListType args;
    args.push_back(maccount);
    mcreate["args"] = args;
    mcreate["objtype"] = "op";

    Create op = (Create&)Atlas::messageObject2ClassObject(mcreate);
    assert(op->getClassNo() == CREATE_NO);
    assert(op->instanceOf(CREATE_NO));
    assert(op->instanceOf(ACTION_NO));
    assert(op->instanceOf(ROOT_NO));
    assert(!op->instanceOf(COMBINE_NO));
    assert(!op->instanceOf(ACCOUNT_NO));
    assert(op->getFrom() == "bar");
    assert(op->getParents().size() == 1);
    assert(op->getParents().front() == "create");
    assert(op->getObjtype() == "op");
    assert(op->getDescription() == 
           "Create new things from nothing using this operator.");
    assert(op->getArgs().size() == 1);

    Account op_arg = (Account&)op->getArgs().front();
    assert(op_arg->getClassNo() == PLAYER_NO);
    assert(!op_arg->instanceOf(CREATE_NO));
    assert(!op_arg->instanceOf(ACTION_NO));
    assert(op_arg->instanceOf(ROOT_NO));
    assert(!op_arg->instanceOf(COMBINE_NO));
    assert(op_arg->instanceOf(ACCOUNT_NO));
    assert(op_arg->instanceOf(PLAYER_NO));
    assert(op_arg->getId() == "bar");
    assert(op_arg->getParents().size() == 1);
    assert(op_arg->getParents().front() == "player");
    assert(op_arg->getObjtype() == "obj");
    assert(op_arg->getDescription() == "Player accounts");
    assert(op_arg->getName() == "foo");
#if 0 //tmp
    assert(op_arg->hasAttr("password"));
    assert(op_arg->getAttr("password").isString());
    assert(op_arg->getAttr("password").asString() == "");
#endif
    assert(op_arg->hasAttr("name"));
    assert(!op_arg->hasAttr("foo"));
    assert(op_arg->hasAttr("pos"));
    assert(!op_arg->isDefaultName());
    assert(op_arg->isDefaultPos());
    }
}

void test()
{
    const double x1 = 3.5;
    const double y1 = -4.6;
    const double z1 = 2.0;

    const double x2 = 42.0;
    const double y2 = 7.0;

    vector<RootEntity> ent_vec(10);

    for(int i=0; i<10; i++) {
        DEBUG_PRINT(cout<<endl<<"round:"<<i<<endl);
        RootEntity human;

        //check for empty default:
        DEBUG_PRINT(cout<<"empty ok?"<<endl);
        Object::ListType empty = human->getVelocityAsList();
        if(i==0) check_float_list3(empty, 0.0, 0.0, 0.0);
        else check_float_list3(empty, 0.0, y2, 0.0);
        
        //check after setting it
        DEBUG_PRINT(cout<<"setting ok?"<<endl);
        Object::ListType velocity;
        velocity.push_back(x1);
        velocity.push_back(y1);
        velocity.push_back(z1);
        check_float_list3(velocity, x1, y1, z1);
        human->setVelocityAsList(velocity);
        Object::ListType foo = human->getVelocityAsList();
        check_float_list3(foo, x1, y1, z1);

        DEBUG_PRINT(cout<<"changing it?"<<endl);
        vector<double> &foo2 = human->modifyVelocity();
        *foo2.begin() = x2;
        check_float_list3(human->getVelocityAsList(), x2, y1, z1);

        DEBUG_PRINT(cout<<"check change result?"<<endl);
        foo = human->getVelocityAsList();
        check_float_list3(foo, x2, y1, z1);
        
        DEBUG_PRINT(cout<<"vector of entities?"<<endl);
        const Object::ListType &ent_velocity = ent_vec[i]->getVelocityAsList();
        if(i==0) check_float_list3(ent_velocity, 0.0, 0.0, 0.0);
        else check_float_list3(ent_velocity, 0.0, y2, 0.0);

        DEBUG_PRINT(cout<<"base?"<<endl);
        RootEntity base_entity = human.getDefaultObject();
        vector<double> &base = base_entity->modifyVelocity();
        base[1] = y2;
        check_float_list3(base_entity->getVelocityAsList(), 0.0, y2, 0.0);

        RootOperation move_op;
        vector<Root> move_args(1);
        move_args[0] = (Root&)human;
        move_op->setArgs(move_args);

        RootOperation sight_op;
        //sight_op->setFrom(humanent.asObjectPtr());
        vector<Root> sight_args(1);
        sight_args[0] = (Root&)move_op;
        sight_op->setArgs(sight_args);
        
        //test 
        DEBUG_PRINT(cout<<"get move_op?"<<endl);
        const vector<Root>& test_args = sight_op->getArgs();
        assert(test_args.size() == 1);
        RootOperation test_op = 
            (RootOperation&)test_args[0];
        
        DEBUG_PRINT(cout<<"get human_ent?"<<endl);
        const vector<Root>& test_args2 = test_op->getArgs();
        assert(test_args2.size() == 1);
        RootEntity test_ent =
            (RootEntity&)test_args2[0];
        Object::ListType foo3 = test_ent->getVelocityAsList();
        check_float_list3(foo3, x2, y1, z1);

        vector<double> coords(3, 0.0);
        human->setPos(coords);
        human->setVelocity(coords);
        human->modifyVelocity()[0] = 1.0;
        check_float_list3(human->getPosAsList(), 0.0, 0.0, 0.0);
        check_float_list3(human->getVelocityAsList(), 1.0, 0.0, 0.0);
    }
}

int main()
{
    try {
        loadDefaults("../../../../protocols/atlas/spec/atlas.xml");
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
