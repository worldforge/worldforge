#include <Atlas/Objects/Decoder.h>

using namespace Atlas;

class TestDecoder : public Objects::Decoder
{
protected:
    virtual void ObjectArrived(const Objects::Root& r)
    {
        assert(r.GetAttr("id").AsString() == "root_instance");
    }

    virtual void ObjectArrived(const Objects::Operation::Look& l)
    {
        assert(l.GetAttr("id").AsString() == "look_instance");
    }
};

int main(int argc, char** argv)
{
    TestDecoder t;
    t.StreamBegin();
    t.StreamMessage(Bridge::MapBegin);
        t.MapItem("parent", Bridge::ListBegin);
            t.ListItem("root");
        t.ListEnd();
        t.MapItem("id", "root_instance");
    t.MapEnd();
    t.StreamMessage(Bridge::MapBegin);
        t.MapItem("parent", Bridge::ListBegin);
            t.ListItem("look");
        t.ListEnd();
        t.MapItem("id", "look_instance");
    t.MapEnd();
    t.StreamEnd();
}
