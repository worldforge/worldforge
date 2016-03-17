/*
 Copyright (C) 2013 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <Atlas/Message/Element.h>
#include <Atlas/Message/MEncoder.h>
#include <Atlas/Message/QueuedDecoder.h>
#include <Atlas/Codecs/XML.h>
#include <Atlas/Codecs/Bach.h>
#include <Atlas/Codecs/Packed.h>

#include <sstream>
#include <string>
#include <cassert>

using namespace Atlas::Message;

void testXMLEscapingOneString(const std::string& string)
{
    assert(
            Atlas::Codecs::XML::unescape(Atlas::Codecs::XML::escape(string)) == string);
    assert(
            Atlas::Codecs::XML::unescape(Atlas::Codecs::XML::escape(" " + string)) == (" " + string));
    assert(
            Atlas::Codecs::XML::unescape(Atlas::Codecs::XML::escape(string + " ")) == (string + " "));
    assert(
            Atlas::Codecs::XML::unescape(Atlas::Codecs::XML::escape(" " + string + " ")) == (" " + string + " "));
}

void testXMLEscaping()
{
    testXMLEscapingOneString("&");
    testXMLEscapingOneString("<");
    testXMLEscapingOneString(">");
    testXMLEscapingOneString("'");
    testXMLEscapingOneString("\"");
    testXMLEscapingOneString("&&");
    testXMLEscapingOneString("<<");
    testXMLEscapingOneString(">> ");
    testXMLEscapingOneString("''");
    testXMLEscapingOneString("\"\"");
    assert(Atlas::Codecs::XML::unescape("&amp") == "&amp");
    assert(Atlas::Codecs::XML::unescape("&amp ") == "&amp ");
    assert(Atlas::Codecs::XML::unescape("&quot") == "&quot");
    assert(Atlas::Codecs::XML::unescape("&quot ") == "&quot ");
    assert(Atlas::Codecs::XML::unescape("&apos") == "&apos");
    assert(Atlas::Codecs::XML::unescape("&apos ") == "&apos ");
    assert(Atlas::Codecs::XML::unescape("&lt") == "&lt");
    assert(Atlas::Codecs::XML::unescape("&lt ") == "&lt ");
    assert(Atlas::Codecs::XML::unescape("&gt") == "&gt");
    assert(Atlas::Codecs::XML::unescape("&gt ") == "&gt ");
}

template<typename T>
void testCodec()
{
    MapType map;

    ListType list = { "foo", 1.5, 5 };
    map["foo1"] = "foo";
    map["foo2"] = 1;
    map["foo3"] = 2.5;
    map["foo4"] = list;
    map["<"] = "<";
    map[">"] = ">";
    map["foo6&"] = "&;";
    map["foo6:"] = ":";
    map["\""] = "\"";
    map["{"] = "{";
    map["}"] = "}";
    map["["] = "[";
    map["]"] = "]";
    map["@"] = "@";
    map["$"] = "$";

    std::stringstream ss;

    {
        Atlas::Message::QueuedDecoder decoder;

        T codec(ss, ss, decoder);
        Atlas::Message::Encoder encoder(codec);
        encoder.streamBegin();
        encoder.streamMessageElement(map);
        encoder.streamEnd();
    }

    std::string atlas_data = ss.str();

    std::stringstream ss2(atlas_data, std::ios::in);

    Atlas::Message::QueuedDecoder decoder;
    {

        T codec(ss2, ss2, decoder);
        Atlas::Message::Encoder enc(codec);

        decoder.streamBegin();
        codec.poll();
        decoder.streamEnd();

    }
//    assert(decoder.queueSize() == 1);
    MapType map2 = decoder.popMessage();

    assert(map2["foo1"].asString() == "foo");
    assert(map2["foo2"].asInt() == 1);
    assert(map2["foo3"].asFloat() == 2.5);
    assert(map2["foo4"].asList()[0].asString() == "foo");
    assert(map2["foo4"].asList()[1].asFloat() == 1.5);
    assert(map2["foo4"].asList()[2].asInt() == 5);
    assert(map2["foo4"].asList()[2].asInt() == 5);
    assert(map2["<"].asString() == "<");
    assert(map2[">"].asString() == ">");
    assert(map2["foo6&"].asString() == "&;");

    assert(map2["foo6:"].asString() == ":");
    assert(map2["\""].asString() == "\"");
    assert(map2["{"].asString() == "{");
    assert(map2["}"].asString() == "}");
    assert(map2["["].asString() == "[");
    assert(map2["]"].asString() == "]");
    assert(map2["@"].asString() == "@");
    assert(map2["$"].asString() == "$");

}

int main(int argc, char** argv)
{
    testXMLEscaping();

    testCodec<Atlas::Codecs::Packed>();
    testCodec<Atlas::Codecs::XML>();
    //Bach is problematic and disabled for now. We should look into using JSON instead.
//    testCodec<Atlas::Codecs::Bach>();

}

