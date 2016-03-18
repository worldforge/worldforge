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
    long long i;

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

    std::string message;

    //Warm up process first
    for (i = 0; i < 100000000; i += 1) {
        Atlas::Message::Element element;
    }
    {
        TIME_ON
        for (i = 0; i < 200000; i += 1) {
            MapType map = sight->asMessage();
        }
        TIME_OFF("BaseObject::asMessage");
    }
    {
        TIME_ON
        for (i = 0; i < 200000; i += 1) {
            MapType map;
            sight->addToMessage(map);
        }
        TIME_OFF("BaseObject::addToMessage");
    }

    {
        TIME_ON
        for (i = 0; i < 1000000; i += 1) {
            Atlas::Message::Element element(
                    std::string(
                            "fdf adda ds dsafds asdfdasdsafdsdsaffdsdsadsafdds a dsf dsdfsads fdsads adsasa"));
        }
        TIME_OFF("Element string ctor");
    }

    {
        TIME_ON
        for (i = 0; i < 1000000; i += 1) {
            Atlas::Message::Element element;
            element =
                    std::string(
                            "fdf adda ds dsafds asdfdasdsafdsdsaffdsdsadsafdds a dsf dsdfsads fdsads adsasa");
        }
        TIME_OFF("Element string rvalue assign");
    }

    {
        TIME_ON
        for (i = 0; i < 1000000; i += 1) {
            Atlas::Message::Element element;
            const std::string aString(
                    "fdf adda ds dsafds asdfdasdsafdsdsaffdsdsadsafdds a dsf dsdfsads fdsads adsasa");
            element = aString;
        }
        TIME_OFF("Element string assign");
    }

    {
        TIME_ON
        for (i = 0; i < 1000000; i += 1) {
            Atlas::Message::Element element;
            std::string aString(
                    "fdf adda ds dsafds asdfdasdsafdsdsaffdsdsadsafdds a dsf dsdfsads fdsads adsasa");
            element = std::move(aString);
        }
        TIME_OFF("Element string move assign");
    }

    {
        TIME_ON
        for (i = 0; i < 1000000; i += 1) {
            Atlas::Message::Element element(
                    "fdf adda ds dsafds asdfdasdsafdsdsaffdsdsadsafdds a dsf dsdfsads fdsads adsasa");
        }
        TIME_OFF("Element char* ctor");
    }

    {
        TIME_ON
        for (i = 0; i < 1000000; i += 1) {
            Atlas::Message::Element element;
            element =
                    "fdf adda ds dsafds asdfdasdsafdsdsaffdsdsadsafdds a dsf dsdfsads fdsads adsasa";
        }
        TIME_OFF("Element char* assign");
    }

    {
        TIME_ON
        for (i = 0; i < 1000000; i += 1.0) {
            MapType mapCopy = map;
            Atlas::Message::Element element(std::move(mapCopy));
        }
        TIME_OFF("Element move ctor");
    }

    {
        TIME_ON
        for (i = 0; i < 1000000; i += 1.0) {
            MapType mapCopy = map;
            Atlas::Message::Element element;
            element = std::move(mapCopy);
        }
        TIME_OFF("Element move assign");
    }

    {
        TIME_ON
        for (i = 0; i < 1000000; i += 1.0) {
            MapType mapCopy = map;
            Atlas::Message::Element element(mapCopy);
        }
        TIME_OFF("Element ctor");
    }

    {
        TIME_ON
        for (i = 0; i < 1000000; i += 1.0) {
            MapType mapCopy = map;
            Atlas::Message::Element element;
            element = mapCopy;
        }
        TIME_OFF("Element assign");
    }

    return 0;
}
