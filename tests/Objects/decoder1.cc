#include "../../src/Objects/Decoder.h"

using namespace Atlas;

bool root_arrived = false;
bool look_arrived = false;
bool acct_arrived = false;

class TestDecoder : public Objects::Decoder
{
protected:
    virtual void ObjectArrived(const Objects::Root& r)
    {
        assert(r.GetAttr("id").AsString() == "root_instance");
        root_arrived = true;
    }

    virtual void ObjectArrived(const Objects::Operation::Look& l)
    {
        assert(l.GetAttr("id").AsString() == "look_instance");
        look_arrived = true;
    }

    virtual void ObjectArrived(const Objects::Entity::Account a)
    {
        acct_arrived = true;
    }
};

int main(int argc, char** argv)
{
    TestDecoder t;
    t.StreamBegin();
    t.StreamMessage(Bridge::MapBegin);
        t.MapItem("parents", Bridge::ListBegin);
            t.ListItem("root");
        t.ListEnd();
        t.MapItem("id", "root_instance");
    t.MapEnd();
    t.StreamMessage(Bridge::MapBegin);
        t.MapItem("parents", Bridge::ListBegin);
            t.ListItem("look");
        t.ListEnd();
        t.MapItem("id", "look_instance");
    t.MapEnd();
    t.StreamEnd();
    assert(root_arrived);
    assert(look_arrived);
    assert(!acct_arrived);
}
