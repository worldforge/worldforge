#include "Atlas/Objects/Decoder.h"
#include "Atlas/Objects/loadDefaults.h"

using namespace Atlas;

bool root_arrived = false;
bool look_arrived = false;
bool acct_arrived = false;
bool unknown_arrived = false;

class TestDecoder : public Objects::Decoder
{
protected:
    virtual void objectArrived(const Objects::Root& r)
    {
        assert(r->getAttr("id").asString() == "root_instance");
        root_arrived = true;
    }

    virtual void objectArrived(const Objects::Operation::Look& l)
    {
        assert(l->getAttr("id").asString() == "look_instance");
        look_arrived = true;
    }

    virtual void objectArrived(const Objects::Entity::Account &a)
    {
        acct_arrived = true;
    }

    virtual void unknownObjectArrived(const Atlas::Message::Object&) {
        unknown_arrived = true;
    }
};

int main(int argc, char** argv)
{
    try {
        Objects::loadDefaults("../../../../protocols/atlas/spec/atlas.xml");
    } catch(Objects::DefaultLoadingException e) {
        cout << "DefaultLoadingException: "
             << e.msg << endl;
        return 1;
    }
    TestDecoder t;
    t.streamBegin();
    t.streamMessage(Bridge::mapBegin);
        t.mapItem("parents", Bridge::listBegin);
            t.listItem("root");
        t.listEnd();
        t.mapItem("id", "root_instance");
    t.mapEnd();
    t.streamMessage(Bridge::mapBegin);
        t.mapItem("parents", Bridge::listBegin);
            t.listItem("look");
        t.listEnd();
        t.mapItem("id", "look_instance");
    t.mapEnd();
    t.streamEnd();
    assert(root_arrived);
    assert(look_arrived);
    assert(!acct_arrived);
    assert(!unknown_arrived);
}
