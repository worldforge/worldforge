#include "Objects/Root.h"
#include "Objects/Encoder.h"
#include "Message/DecoderBase.h"
#include "Objects/loadDefaults.h"

#include <string>
#include <iostream>

class RootDecoder : public Atlas::Message::DecoderBase
{
protected:
    virtual void objectArrived(const Atlas::Message::Element& o)
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
	Atlas::Objects::loadDefaults("../../../../protocols/atlas/spec/atlas.xml");
    } catch(Atlas::Objects::DefaultLoadingException e) {
	std::cout << "DefaultLoadingException: "
             << e.getDescription() << std::endl;
        return 1;
    }
    RootDecoder rd;
    Atlas::Objects::ObjectsEncoder re(&rd);

    rd.streamBegin(); // important, otherwise we'll segfault!
    Atlas::Objects::Root root_inst;
    root_inst->setAttr("id", std::string("root_instantiation"));
    re.streamMessage((Atlas::Objects::Root&)root_inst);
    rd.streamEnd();
}
