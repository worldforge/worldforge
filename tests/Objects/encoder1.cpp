#include <string>
#include "Atlas/Objects/Root.h"
#include "Atlas/Objects/Encoder.h"
#include "Atlas/Message/DecoderBase.h"
#include "Atlas/Objects/loadDefaults.h"

using namespace std;
using namespace Atlas;

class RootDecoder : public Message::DecoderBase
{
protected:
    virtual void objectArrived(const Message::Object& o)
    {
        assert(o.isMap());
        assert(o.asMap().find(string("parents")) != o.asMap().end());
        assert((*o.asMap().find("parents")).second.asList().size() == 1);
        assert(*(*o.asMap().find("parents")).second.asList().begin() ==
                string("root"));
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
    RootDecoder rd;
    Objects::Encoder re(&rd);

    rd.streamBegin(); // important, otherwise we'll segfault!
    Objects::Root root_inst;
    root_inst->setAttr("id", string("root_instantiation"));
    re.streamMessage((Objects::Root&)root_inst);
    rd.streamEnd();
}
