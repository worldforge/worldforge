#include "Objects/Decoder.h"
#include "Objects/loadDefaults.h"

#include <iostream>

bool root_arrived = false;
bool look_arrived = false;
bool acct_arrived = false;
bool anonymous_arrived = false;
bool unknown_arrived = false;

class TestDecoder : public Atlas::Objects::ObjectsDecoder
{
protected:
    virtual void objectArrived(const Atlas::Objects::Root& r)
    {
        assert(r->getAttr("id").asString() == "root_instance");
        root_arrived = true;
    }

    virtual void objectArrived(const Atlas::Objects::Operation::Look& l)
    {
        assert(l->getAttr("id").asString() == "look_instance");
        look_arrived = true;
    }

    virtual void objectArrived(const Atlas::Objects::Entity::Account &a)
    {
        acct_arrived = true;
    }

    virtual void objectArrived(const Atlas::Objects::Entity::Anonymous&) {
        anonymous_arrived = true;
    }

    virtual void unknownObjectArrived(const Atlas::Message::Element&) {
        unknown_arrived = true;
    }
};

int main(int argc, char** argv)
{
    try {
	Atlas::Objects::loadDefaults("../../../../protocols/atlas/spec/atlas.xml");
    } catch(Atlas::Objects::DefaultLoadingException e) {
	std::cout << "DefaultLoadingException: "
             << e.getDescription() << std::endl;
        return 1;
    }
    TestDecoder t;
    t.streamBegin();
    t.streamMessage(Atlas::Bridge::m_mapBegin);
        t.mapItem("parents", Atlas::Bridge::m_listBegin);
//            t.listItem("root");
        t.listEnd();
//        t.mapItem("id", "root_instance");
        t.mapItem("id", "foo");
    t.mapEnd();
    t.streamMessage(Atlas::Bridge::m_mapBegin);
        t.mapItem("objtype", "op");
        t.mapItem("parents", Atlas::Bridge::m_listBegin);
            t.listItem("look");
        t.listEnd();
        t.mapItem("id", "look_instance");
    t.mapEnd();
    t.streamEnd();
    assert(!root_arrived);
    assert(anonymous_arrived);
    assert(look_arrived);
    assert(!acct_arrived);
    assert(!unknown_arrived);
}
