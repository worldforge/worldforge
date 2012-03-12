#include <Atlas/Objects/Dispatcher.h>
#include <Atlas/Objects/loadDefaults.h>

#include <iostream>
#include <cstdlib>

bool root_arrived = false;
bool look_arrived = false;
bool acct_arrived = false;
bool anonymous_arrived = false;
bool unknown_arrived = false;

class TestDecoder : public Atlas::Objects::Dispatcher
{
protected:
    virtual void objectRootArrived(const Atlas::Objects::Root& r)
    {
        assert(r->getAttr("id").asString() == "root_instance");
        root_arrived = true;
    }

    virtual void objectLookArrived(const Atlas::Objects::Operation::Look& l)
    {
        assert(l->getAttr("id").asString() == "look_instance");
        look_arrived = true;
    }

    virtual void objectAccountArrived(const Atlas::Objects::Entity::Account &a)
    {
        acct_arrived = true;
    }

    virtual void objectAnonymousArrived(const Atlas::Objects::Entity::Anonymous&) {
        anonymous_arrived = true;
    }

    virtual void unknownObjectArrived(const Atlas::Message::Element&) {
        unknown_arrived = true;
    }
};

int main(int argc, char** argv)
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
    TestDecoder t;
    t.streamBegin();
    t.streamMessage();
        t.mapListItem("parents");
//            t.listItem("root");
        t.listEnd();
//        t.mapItem("id", "root_instance");
        t.mapStringItem("id", "foo");
    t.mapEnd();
    t.streamMessage();
        t.mapStringItem("objtype", "op");
        t.mapListItem("parents");
            t.listStringItem("look");
        t.listEnd();
        t.mapStringItem("id", "look_instance");
    t.mapEnd();
    t.streamEnd();
    assert(!root_arrived);
    assert(anonymous_arrived);
    assert(look_arrived);
    assert(!acct_arrived);
    assert(!unknown_arrived);
}
