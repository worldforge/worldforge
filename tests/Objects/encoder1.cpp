#include <string>
#include "Objects/Root.h"
#include "Objects/Encoder.h"
#include "Message/DecoderBase.h"
#include "Objects/loadDefaults.h"

using namespace std;
using namespace Atlas;

class RootDecoder : public DecoderBase
{
protected:
    virtual void objectArrived(const Object& o)
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
        loadDefaults("../../../../protocols/atlas/spec/atlas.xml");
    } catch(DefaultLoadingException e) {
        cout << "DefaultLoadingException: "
             << e.msg << endl;
        return 1;
    }
    RootDecoder rd;
    ObjectsEncoder re(&rd);

    rd.streamBegin(); // important, otherwise we'll segfault!
    Root root_inst;
    root_inst->setAttr("id", string("root_instantiation"));
    re.streamMessage((Root&)root_inst);
    rd.streamEnd();
}
