#include "timer.h"

#include <iostream>
#include <sstream>
#include <cassert>

#include <Atlas/Codecs/Packed.h>
#include <Atlas/Codecs/XML.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Message/QueuedDecoder.h>
#include <Atlas/Message/MEncoder.h>
#include <Atlas/Objects/Entity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

int main(int argc, char** argv)
{

	Atlas::Objects::Factories factories;

    Atlas::Objects::Entity::Anonymous anon;
    anon->setLoc("12345");
    ListType velocity;
    velocity.push_back(1.4);
    velocity.push_back(2.4);
    velocity.push_back(3.4);
    anon->setVelocityAsList(velocity);
    ListType bbox;
    bbox.push_back(1.4);
    bbox.push_back(2.4);
    bbox.push_back(3.4);
    bbox.push_back(2.4);
    anon->setAttr("bbox", bbox);

    Atlas::Objects::Operation::Move move;
    move->setFrom("123456");
    move->setTo("123456");
    move->setSeconds(12345678);
    move->setId("123456");
    move->setArgs1(anon);

    Atlas::Objects::Operation::Sight sight;
    sight->setFrom("123456");
    sight->setTo("123456");
    sight->setSeconds(12345678);
    sight->setId("123456");
    sight->setArgs1(move);

    const MapType map = sight->asMessage();

    long long i;
    //Warm up process first
    for (i = 0; i < 100000000; i += 1) {
        Atlas::Message::Element element;
    }

    std::string string;
    ListType list;
    for (int i = 0; i < 1000; ++i) {
        list.push_back(i);
    }
    for (int i = 0; i < 10000; ++i) {
        string += i;
    }

    TIME_ON
    for (i = 0; i < 200000; i += 1) {
        MapType mapCopy(sight->asMessage());
		factories.createObject(mapCopy);
    }
    TIME_OFF("createObject");

    TIME_ON
    for (i = 0; i < 20000; i += 1) {
        Atlas::Message::Element foo(list);

        const ListType& newList = foo.List();
        newList.size();
    }
    TIME_OFF("Element List ref");

    TIME_ON
    for (i = 0; i < 20000; i += 1) {
        Atlas::Message::Element foo(list);

        ListType newList = foo.List();
    }
    TIME_OFF("Element List");

    TIME_ON
    for (i = 0; i < 20000; i += 1) {
        Atlas::Message::Element foo(list);

        ListType newList = foo.moveList();
    }
    TIME_OFF("Element moveList");

    TIME_ON
    for (i = 0; i < 200000; i += 1) {
        Atlas::Message::Element foo(string);

        const std::string& newString = foo.String();
        newString.size();
    }
    TIME_OFF("Element String ref");

    TIME_ON
    for (i = 0; i < 200000; i += 1) {
        Atlas::Message::Element foo(string);

        std::string newString = foo.String();
    }
    TIME_OFF("Element String");

    TIME_ON
    for (i = 0; i < 200000; i += 1) {
        Atlas::Message::Element foo(string);

        std::string newString = foo.moveString();
    }
    TIME_OFF("Element moveString");

    //Set foo string

    TIME_ON
    for (i = 0; i < 2000000; i += 1) {
        Atlas::Objects::Root object;
        Element element(string);
        object->setAttr("foo", element);
    }
    TIME_OFF("setAttr string");

    TIME_ON
    for (i = 0; i < 2000000; i += 1) {
        Atlas::Objects::Root object;
        Element element(string);
        object->setAttr("foo", std::move(element));
    }
    TIME_OFF("setAttr string move");

    //Set id string

    TIME_ON
    for (i = 0; i < 2000000; i += 1) {
        Atlas::Objects::Root object;
        Element element(string);
        object->setAttr("id", element);
    }
    TIME_OFF("setAttr id string");

    TIME_ON
    for (i = 0; i < 2000000; i += 1) {
        Atlas::Objects::Root object;
        Element element(string);
        object->setAttr("id", std::move(element));
    }
    TIME_OFF("setAttr id string move");

    //Set foo map

    TIME_ON
    for (i = 0; i < 2000000; i += 1) {
        Atlas::Objects::Root object;
        Element element(map);
        object->setAttr("foo", element);
    }
    TIME_OFF("setAttr map");

    TIME_ON
    for (i = 0; i < 2000000; i += 1) {
        Atlas::Objects::Root object;
        Element element(map);
        object->setAttr("foo", std::move(element));
    }
    TIME_OFF("setAttr map");

    return 0;
}
