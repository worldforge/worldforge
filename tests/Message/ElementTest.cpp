
#include <cassert>
#include "Atlas/Message/Element.h"

using namespace Atlas::Message;

int main(int argc, char **argv) {
    {
        Element e = "aString";
        assert(e.isString());
        assert(e.String() == "aString");
        assert(e.asString() == "aString");
        assert(e.getType() == Element::TYPE_STRING);
    }

    {
        Element e = "";
        assert(e.isString());
        assert(e.String() == "");
        assert(e.asString() == "");
        assert(e.getType() == Element::TYPE_STRING);
    }

    {
        Element e = std::string("aString");
        assert(e.isString());
        assert(e.String() == "aString");
        assert(e.asString() == "aString");
        assert(e.getType() == Element::TYPE_STRING);
    }

    {
        Element e = 1;
        assert(e.isInt());
        assert(e.isNum());
        assert(e.Int() == 1);
        assert(e.asInt() == 1);
        assert(e.getType() == Element::TYPE_INT);
    }

    {
        Element e = 1L;
        assert(e.isInt());
        assert(e.isNum());
        assert(e.Int() == 1);
        assert(e.asInt() == 1);
        assert(e.getType() == Element::TYPE_INT);
    }

    {
        Element e = 1.0;
        assert(e.isFloat());
        assert(e.isNum());
        assert(e.Float() == 1.0);
        assert(e.asFloat() == 1.0);
        assert(e.getType() == Element::TYPE_FLOAT);
    }

    {
        Element e = 1.0f;
        assert(e.isFloat());
        assert(e.isNum());
        assert(e.Float() == 1.0f);
        assert(e.asFloat() == 1.0f);
        assert(e.getType() == Element::TYPE_FLOAT);
    }

    {
        Element e = std::vector<Element>{"foo"};
        assert(e.isList());
        assert(e.List() == std::vector<Element>{"foo"});
        assert(e.asList() == std::vector<Element>{"foo"});
        assert(e.getType() == Element::TYPE_LIST);

        auto moved = e.moveList();
        assert(e.List().empty());
        assert(moved == std::vector<Element>{"foo"});
    }

    {
        Element e = std::map<std::string, Element>{{"foo", "bar"}};
        assert(e.isMap());
        const std::map<std::string, Element> aMap{{"foo", "bar"}};
        assert(e.Map() == aMap);
        assert(e.asMap() == aMap);
        assert(e.getType() == Element::TYPE_MAP);

        auto moved = e.moveMap();
        assert(e.Map().empty());
        assert(moved == aMap);
    }
}

