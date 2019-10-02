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

    //Warm up process first
    for (i = 0; i < 100000000; i += 1) {
        Atlas::Message::Element element;
    }

    std::string message;
    {
        std::stringstream sstream;
        Atlas::Message::QueuedDecoder decoder;
        Atlas::Codecs::Packed packed(sstream, sstream, decoder);
        Atlas::Message::Encoder encoder(packed);

        encoder.streamBegin();
        encoder.streamMessageElement(map);
        encoder.streamEnd();

        message = sstream.str();
        //std::cout << message << std::endl << std::flush;

        //Disable storing of data.
        sstream.setstate(std::ios_base::badbit);

        TIME_ON

        for (i = 0; i < 1000000.0; i += 1.0) {

            encoder.streamBegin();
            encoder.streamMessageElement(map);
            encoder.streamEnd();
        }
        TIME_OFF("Encoding message with Packed");
    }

    {

        std::stringstream sstream;
        std::istringstream istream;
        Atlas::Message::QueuedDecoder decoder;
        Atlas::Codecs::Packed packed(istream, sstream, decoder);
        Atlas::Message::Encoder encoder(packed);

        TIME_ON
        for (i = 0; i < 100000.0; i += 1.0) {

            istream.clear();
            istream.str(message);

            packed.streamBegin();
            packed.poll();
            packed.streamEnd();
            decoder.popMessage();
        }
        TIME_OFF("Decoding message with Packed");
    }
    return 0;
}
