#include <string>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Combine.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Message/DecoderBase.h>

using namespace std;
using namespace Atlas;

class RootDecoder : public Message::DecoderBase
{
protected:
    virtual void ObjectArrived(const Message::Object& o)
    {
        assert(o.IsMap());
        assert(o.AsMap().find(string("parent")) != o.AsMap().end());
        assert((*o.AsMap().find("parent")).second.AsList().size() == 1);
        assert(*(*o.AsMap().find("parent")).second.AsList().begin() ==
                string("root"));
    }
};

int main(int argc, char** argv)
{
    RootDecoder rd;
    Objects::Encoder re(&rd);

    rd.StreamBegin(); // important, otherwise we'll segfault!
    Objects::Root root_inst = Objects::Root::Instantiate();
    root_inst.SetAttr("id", string("root_instantiation"));
    re.StreamMessage(&root_inst);
    rd.StreamEnd();
}
