#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/loadDefaults.h>

#include <iostream>
#include <cstdlib>

bool root_arrived = false;
bool look_arrived = false;
bool acct_arrived = false;
bool anonymous_arrived = false;
bool unknown_arrived = false;

struct TestDecoder : public Atlas::Objects::ObjectsDecoder
{
	explicit TestDecoder(Atlas::Objects::Factories& factories);
	void objectArrived(const Atlas::Objects::Root& obj) override {

    	if (obj) {
    		switch (obj->getClassNo()) {
				case Atlas::Objects::ROOT_NO:
					assert(obj->getAttr("id").asString() == "root_instance");
					root_arrived = true;
					break;
				case Atlas::Objects::Operation::LOOK_NO:
					assert(obj->getAttr("id").asString() == "look_instance");
					look_arrived = true;
					break;
				case Atlas::Objects::Entity::ACCOUNT_NO:
					acct_arrived = true;
					break;
				case Atlas::Objects::Entity::ANONYMOUS_NO:
					anonymous_arrived = true;
					break;
				default:
					unknown_arrived = true;
			}
    	}
	}

};

TestDecoder::TestDecoder(Atlas::Objects::Factories& factories) : ObjectsDecoder(factories) {

}

int main(int argc, char** argv)
{
	Atlas::Objects::Factories factories;
    std::string atlas_xml_path;
    char * srcdir_env = getenv("srcdir");
    if (srcdir_env != 0) {
        atlas_xml_path = srcdir_env;
        atlas_xml_path += "/";
    }
    atlas_xml_path += "../../protocol/spec/atlas.xml";
    try {
        Atlas::Objects::loadDefaults(atlas_xml_path, factories);
    } catch(const Atlas::Objects::DefaultLoadingException& e) {
        std::cout << "DefaultLoadingException: "
             << e.getDescription() << std::endl;
    }
    TestDecoder t(factories);
    t.streamBegin();
    t.streamMessage();
        t.mapStringItem("parent", "");
//            t.listItem("root");
//        t.mapItem("id", "root_instance");
        t.mapStringItem("id", "foo");
    t.mapEnd();
    t.streamMessage();
        t.mapStringItem("objtype", "op");
        t.mapStringItem("parent", "look");
        t.mapStringItem("id", "look_instance");
    t.mapEnd();
    t.streamEnd();
    assert(!root_arrived);
    assert(anonymous_arrived);
    assert(look_arrived);
    assert(!acct_arrived);
    assert(!unknown_arrived);
}
