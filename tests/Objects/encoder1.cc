#include <string>
#include "../../src/Objects/Root.h"
#include "../../src/Objects/Encoder.h"
#include "../../src/Message/DecoderBase.h"

using namespace std;
using namespace Atlas;

class RootDecoder : public Message::DecoderBase
{
protected:
    virtual void ObjectArrived(const Message::Object& o)
    {
        assert(o.IsMap());
        assert(o.AsMap().find(string("parents")) != o.AsMap().end());
        assert((*o.AsMap().find("parents")).second.AsList().size() == 1);
        assert(*(*o.AsMap().find("parents")).second.AsList().begin() ==
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
