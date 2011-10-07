#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Dispatcher.h>
#include <Atlas/Objects/loadDefaults.h>
//#include "../../src/Net/Stream.h"
#include "DebugBridge.h"

#define DEBUG_PRINT(foo) //foo;

#define USE_XML 1
#if USE_XML
#include <Atlas/Codecs/XML.h>
#else
#include <Atlas/Codecs/Packed.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
#include <cstdlib>

#if USE_XML
#define USE_FILE 0
#else
#define USE_FILE 0
#endif

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Account;
using Atlas::Objects::Entity::Anonymous;

std::string object2String(const Root& obj)
{
    DebugBridge bridge;
    std::stringstream stream;
    Atlas::Codec *codec;
    codec = new Atlas::Codecs::XML(stream, bridge);
    assert(codec);
    codec->streamBegin();
    Atlas::Objects::ObjectsEncoder eno(*codec);
    eno.streamObjectsMessage(obj);
    codec->streamEnd();
    delete codec;
    return stream.str();
}

class TestDecoder : public Atlas::Objects::Dispatcher
{
protected:
    virtual void objectRootArrived(const Root& r)
    {
//        assert(r->getAttr("id").asString() == "root_instance");
      std::cout << "got Root! " << object2String(r) << std::endl;
    }

    virtual void objectLoginArrived(const Login& r)
    {
        std::cout << "got Account!" << std::endl;
//        assert(r->getAttr("id").asString() == "root_instance");
    }

    virtual void objectLookArrived(const Look& l)
    {
//        assert(l->getAttr("id").asString() == "look_instance");
        std::cout << "got Look!" << std::endl;
    }

    virtual void objectAccountArrived(const Account &a)
    {
        std::cout << "got Account!" << std::endl;
    }
};

void testXML()
{
    RootEntity human;
    human->setId("foo");

    Move move_op;
    move_op->setFrom(std::string("bar"));
    std::vector<Root> move_args(1);
    move_args[0] = human;
    move_op->setArgs(move_args);

    Atlas::Message::ListType velocity;
    velocity.push_back(2.0);
    velocity.push_back(1.0);
    velocity.push_back(0.0);
    human->setVelocityAsList(velocity);

//    typedef BaseObjectData *(*alloc_func)();
//    alloc_func alloc_entity = &Entity::RootEntityDataInstance::alloc;
//    BaseObjectData *bod = alloc_entity();
    //Root human2(bod);
    Root human2 = Atlas::Objects::factory<Atlas::Objects::Entity::RootEntityData>("root_enitty", Atlas::Objects::Entity::RootEntity()->getClassNo());
    std::cout<<"human.id="<<human->getId()<<std::endl;
    std::cout<<"human2.id="<<human2->getId()<<std::endl;
#if 0
    typedef std::list<Atlas::Factory<Atlas::Codec >*> FactoryCodecs;
    FactoryCodecs *myCodecs = &Factory<Codec >::factories();
    FactoryCodecs::iterator i;
    std::cout<<"myCodecs: "<<myCodecs->size();
    for (i = myCodecs->begin(); i != myCodecs->end(); ++i)
        std::cout<<":"<<(*i)->getName();
    std::cout<<std::endl;
#endif

    //DebugBridge bridge;
    TestDecoder bridge;
#if USE_FILE
    fstream stream;
    std::string atlas_xml_path;
    char * srcdir_env = getenv("srcdir");
    if (srcdir_env != 0) {
        atlas_xml_path = srcdir_env;
        atlas_xml_path += "/";
    }
    atlas_xml_path += "../../protocol/spec/atlas.xml";
    stream.open(atlas_xml_path, std::ios::in);
    assert(!!stream);
#else
    std::stringstream stream;
#endif
//     typedef std::list<Atlas::Factory<Atlas::Codec >*> FactoryCodecs;
//     FactoryCodecs *myCodecs = &Factory<Codec >::factories();
//     FactoryCodecs::iterator codec_i;
//     Atlas::Codec *codec = NULL;
//     for(codec_i = myCodecs->begin(); codec_i != myCodecs->end(); ++codec_i)
//     {
//         std::cout<<(*codec_i)->getName()<<std::endl;
//         if ((*codec_i)->getName() == "XML") {
//             codec = (*codec_i)->New(Codec::Parameters(stream, &bridge));
//         }
//     }
//     assert(codec);

    
    Account account;
    Login l;
    account->setAttr("id", std::string("al"));
    account->setAttr("password", std::string("ping"));
    //list<Message::Object> args(1,account->asObject());
    //l->setArgsAsList(args);
    std::vector<Root> args(1);
    args[0] = account;
    l->setArgs(args);
    //coder->streamObjectsMessage((Root&)l);
//<map><list name="args"><map><std::string name="id">al</strin
//g></map></list><list name="parents"><std::string>root</std::string></list><std::string name="ob
//jtype">op_definition</std::string></map>


    Atlas::Codec *codec;
#if USE_XML
    codec = new Atlas::Codecs::XML((std::iostream&)stream, bridge);
#else
    codec = new Atlas::Codecs::Packed(stream, bridge);
#endif
    assert(codec);

#if USE_FILE
    while(stream) {
      codec->poll();
      //std::cout<<"--------"<<std::endl;
    }
#else
    codec->streamBegin();

    Atlas::Objects::ObjectsEncoder eno(*codec);
//    eno.streamObjectsMessage(move_op);
    eno.streamObjectsMessage(l);

    Anonymous e;
    eno.streamObjectsMessage(e);
    e->setId("foo");
    eno.streamObjectsMessage(e);
//    Atlas::Message::Encoder en(codec);
//    en.streamObjectsMessage(human->asObject());

    codec->streamEnd();
    std::cout<<std::endl<<stream.str()<<std::endl;
    //[$from=bar(args=[$id=foo])][$id=foo]
    //<atlas><map><std::string name="from">bar</std::string><list name="args"><map><std::string name="id">foo</std::string></map></list></map><map><std::string name="id">foo</std::string></map></atlas>
#endif
}


void check_float_list3(const Atlas::Message::ListType &list,
                       double el1, double el2, double el3)
{
    assert( list.size() == 3 );
    Atlas::Message::ListType::const_iterator i = list.begin();
    assert( (*i++) == el1 );
    assert( (*i++) == el2 );
    assert( (*i++) == el3 );
    i++;
}

void testValues()
{
    Account account;
    Login l;
    account->setId("al");
    account->setAttr("password", std::string("ping"));
    l->setArgs1(account);

    // assert(l->getArgs()[0]->getLongDescription()=="Later in hierarchy tree objtype changes to 'object' when actual game objects are made.");
    // assert(l->getArgs()[0]->getDescription()=="Base class for accounts");
    assert(l->getId()=="");
    assert(l->getParents().front()=="login");
    assert(l->getObjtype()=="op");
    // std::cout<<std::endl<<"account.long_description: "
        // <<l->getArgs()[0]->getLongDescription()<<std::endl;
    
    {
    Atlas::Message::MapType mobj;
    Atlas::Message::ListType parents;
    parents.push_back(std::string("account"));
    mobj["parents"] = parents;
    mobj["name"] = std::string("foo");
    mobj["objtype"] = std::string("op");
    Root obj = Atlas::Objects::Factories::instance()->createObject(mobj);
    assert(obj->getClassNo() == Atlas::Objects::Entity::ACCOUNT_NO);
    assert(obj->getId() == "");
    assert(obj->isDefaultId() == true);
    assert(obj->getName() == "foo");
    assert(obj->isDefaultName() == false);
    assert(obj->getParents().front() == "account");
    //should this be true? modify MessageObject2ClassObject if yes
    assert(obj->isDefaultParents() == false);
    assert(obj->getObjtype() == "op");
    assert(obj->isDefaultObjtype() == false); //should this be true? 
    // assert(obj->getDescription() == "Base class for accounts");
    // assert(obj->isDefaultDescription() == true);
    }
    
    {
    Atlas::Message::MapType mobj;
    Root obj = Atlas::Objects::objectDefinitions.find(std::string("account"))->second;
    assert(obj->getClassNo() == Atlas::Objects::Entity::ACCOUNT_NO);
    assert(obj->getId() == "account");
    assert(obj->isDefaultId() == false);
    assert(obj->getName() == "");
    assert(obj->isDefaultName() == true);
    assert(obj->getParents().front() == "admin_entity");
    assert(obj->isDefaultParents() == false);
    assert(obj->getObjtype() == "class");
    assert(obj->isDefaultObjtype() == false);
    // assert(obj->getDescription() == "Base class for accounts");
    // assert(obj->isDefaultDescription() == false);
    }

    {
    Atlas::Message::MapType mobj;
    Root obj = Atlas::Objects::Factories::instance()->createObject(mobj);
    assert(obj->getClassNo() == Atlas::Objects::Entity::ANONYMOUS_NO);
    assert(obj->getId() == "");
    assert(obj->getName() == "");
    assert(obj->getParents().size() == 0);
    assert(obj->getObjtype() == "obj");
    // assert(obj->getDescription() == "");
    }

    {
    Atlas::Message::MapType mobj;
    mobj["id"] = std::string("bar");
    mobj["name"] = std::string("foo");
    Atlas::Message::ListType parents;
    parents.push_back(std::string("account"));
    mobj["parents"] = parents;
    Root obj = Atlas::Objects::Factories::instance()->createObject(mobj);
    assert(obj->getClassNo() == Atlas::Objects::Entity::ANONYMOUS_NO);
    assert(obj->getId() == "bar");
    assert(obj->getName() == "foo");
    assert(obj->getParents().front() == "account");
    assert(obj->getObjtype() == "obj");
    // assert(obj->getDescription() == "");
    }

    {
    Atlas::Message::MapType maccount;
    maccount["id"] = std::string("bar");
    maccount["name"] = std::string("foo");
    Atlas::Message::ListType parents;
    parents.push_back(std::string("player"));
    maccount["parents"] = parents;
    maccount["objtype"] = "obj";

    Atlas::Message::MapType mcreate;
    mcreate["from"] = std::string("bar");
    Atlas::Message::ListType parents2;
    parents2.push_back(std::string("create"));
    mcreate["parents"] = parents2;
    Atlas::Message::ListType args;
    args.push_back(maccount);
    mcreate["args"] = args;
    mcreate["objtype"] = "op";

    Create op = Atlas::Objects::smart_dynamic_cast<Create>(Atlas::Objects::Factories::instance()->createObject(mcreate));
    assert(op->getClassNo() == Atlas::Objects::Operation::CREATE_NO);
    assert(op->instanceOf(Atlas::Objects::Operation::CREATE_NO));
    assert(op->instanceOf(Atlas::Objects::Operation::ACTION_NO));
    assert(op->instanceOf(Atlas::Objects::ROOT_NO));
    assert(!op->instanceOf(Atlas::Objects::Operation::COMBINE_NO));
    assert(!op->instanceOf(Atlas::Objects::Entity::ACCOUNT_NO));
    assert(op->getFrom() == "bar");
    assert(op->getParents().size() == 1);
    assert(op->getParents().front() == "create");
    assert(op->getObjtype() == "op");
    // assert(op->getDescription() == 
           // "Create new things from nothing using this operator.");
    assert(op->getArgs().size() == 1);

    Account op_arg = (Account&)op->getArgs().front();
    assert(op_arg->getClassNo() == Atlas::Objects::Entity::PLAYER_NO);
    assert(!op_arg->instanceOf(Atlas::Objects::Operation::CREATE_NO));
    assert(!op_arg->instanceOf(Atlas::Objects::Operation::ACTION_NO));
    assert(op_arg->instanceOf(Atlas::Objects::ROOT_NO));
    assert(!op_arg->instanceOf(Atlas::Objects::Operation::COMBINE_NO));
    assert(op_arg->instanceOf(Atlas::Objects::Entity::ACCOUNT_NO));
    assert(op_arg->instanceOf(Atlas::Objects::Entity::PLAYER_NO));
    assert(op_arg->getId() == "bar");
    assert(op_arg->getParents().size() == 1);
    assert(op_arg->getParents().front() == "player");
    assert(op_arg->getObjtype() == "obj");
    // assert(op_arg->getDescription() == "Player accounts");
    assert(op_arg->getName() == "foo");
#if 0 //tmp
    assert(op_arg->hasAttr("password"));
    assert(op_arg->getAttr("password").isString());
    assert(op_arg->getAttr("password").asString() == "");
#endif
    assert(op_arg->hasAttr("name"));
    assert(!op_arg->hasAttr("foo"));
    assert(!op_arg->hasAttr("pos"));
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

    std::vector<RootEntity> ent_vec(10);

    for(int i=0; i<10; i++) {
        DEBUG_PRINT(std::cout<<std::endl<<"round:"<<i<<std::endl);
        RootEntity human;

        //check for empty default:
        DEBUG_PRINT(std::cout<<"empty ok?"<<std::endl);
        Atlas::Message::ListType empty = human->getVelocityAsList();
        if(i==0) check_float_list3(empty, 0.0, 0.0, 0.0);
        else check_float_list3(empty, 0.0, y2, 0.0);
        
        //check after setting it
        DEBUG_PRINT(std::cout<<"setting ok?"<<std::endl);
        Atlas::Message::ListType velocity;
        velocity.push_back(x1);
        velocity.push_back(y1);
        velocity.push_back(z1);
        check_float_list3(velocity, x1, y1, z1);
        human->setVelocityAsList(velocity);
        Atlas::Message::ListType foo = human->getVelocityAsList();
        check_float_list3(foo, x1, y1, z1);

        DEBUG_PRINT(std::cout<<"changing it?"<<std::endl);
        std::vector<double> &foo2 = human->modifyVelocity();
        *foo2.begin() = x2;
        check_float_list3(human->getVelocityAsList(), x2, y1, z1);

        DEBUG_PRINT(std::cout<<"check change result?"<<std::endl);
        foo = human->getVelocityAsList();
        check_float_list3(foo, x2, y1, z1);
        
        DEBUG_PRINT(std::cout<<"std::vector of entities?"<<std::endl);
        const Atlas::Message::ListType &ent_velocity = ent_vec[i]->getVelocityAsList();
        if(i==0) check_float_list3(ent_velocity, 0.0, 0.0, 0.0);
        else check_float_list3(ent_velocity, 0.0, y2, 0.0);

        DEBUG_PRINT(std::cout<<"base?"<<std::endl);
        RootEntity base_entity = human.getDefaultObject();
        std::vector<double> &base = base_entity->modifyVelocity();
        base[1] = y2;
        check_float_list3(base_entity->getVelocityAsList(), 0.0, y2, 0.0);

        RootOperation move_op;
        std::vector<Root> move_args(1);
        move_args[0] = human;
        move_op->setArgs(move_args);

        RootOperation sight_op;
        //sight_op->setFrom(humanent.asObjectPtr());
        std::vector<Root> sight_args(1);
        sight_args[0] = move_op;
        sight_op->setArgs(sight_args);
        
        //test 
        DEBUG_PRINT(std::cout<<"get move_op?"<<std::endl);
        const std::vector<Root>& test_args = sight_op->getArgs();
        assert(test_args.size() == 1);
        RootOperation test_op = 
            (RootOperation&)test_args[0];
        
        DEBUG_PRINT(std::cout<<"get human_ent?"<<std::endl);
        const std::vector<Root>& test_args2 = test_op->getArgs();
        assert(test_args2.size() == 1);
        RootEntity test_ent =
            (RootEntity&)test_args2[0];
        Atlas::Message::ListType foo3 = test_ent->getVelocityAsList();
        check_float_list3(foo3, x2, y1, z1);

        std::vector<double> coords(3, 0.0);
        human->setPos(coords);
        human->setVelocity(coords);
        human->modifyVelocity()[0] = 1.0;
        check_float_list3(human->getPosAsList(), 0.0, 0.0, 0.0);
        check_float_list3(human->getVelocityAsList(), 1.0, 0.0, 0.0);
    }
}

int main()
{
    std::string atlas_xml_path;
    char * srcdir_env = getenv("srcdir");
    if (srcdir_env != 0) {
        atlas_xml_path = srcdir_env;
        atlas_xml_path += "/";
    }
    atlas_xml_path += "../../protocol/spec/atlas.xml";
    try {
        Atlas::Objects::loadDefaults(atlas_xml_path);
    } catch(Atlas::Objects::DefaultLoadingException e) {
        std::cout << "DefaultLoadingException: "
             << e.getDescription() << std::endl;
    }
    testXML();
    testValues();
    test();
    return 0;
}
