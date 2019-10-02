#include "timer.h"

#include <Atlas/Objects/RootOperation.h>

#include <iostream>

int main(int argc, char ** argv)
{
    Atlas::Objects::Operation::RootOperation op;
    Atlas::Message::MapType message;

    TIME_ON;
    int i;
    for (i = 0; i < 1000000; ++i) {
        Atlas::Message::MapType message;
        op->addToMessage(message);
        if (i == 0) {
            std::cout << "Message: " << message.size() << std::endl << std::flush;
        }
        Atlas::Message::MapType::const_iterator I = message.begin();
        Atlas::Message::MapType::const_iterator Iend = message.end();
        for (; I != Iend; ++I) {
            std::string key = I->first;
            Atlas::Message::Element val = I->second;
        }
    }
    TIME_OFF("Empty Object to Message conversion and iteration");

    TIME_ON;
    for (i = 0; i < 1000000; ++i) {
        Atlas::Objects::Operation::RootOperation::const_iterator I = op->begin();
        Atlas::Objects::Operation::RootOperation::const_iterator Iend = op->end();
        for (; I != Iend; ++I) {
            std::string key = I->first;
            Atlas::Message::Element val = I->second;
        }
    }
    TIME_OFF("Empty Object iteration");

    // Fill the object with non-default values
    op->setId("foo");
    op->setParent("bar");
    op->setStamp(23.42);
    op->setObjtype("non_standard");
    op->setName("fred");
    op->setSerialno(17);
    op->setRefno(23);
    op->setFrom("jim");
    op->setTo("bob");
    op->setSeconds(10800);
    op->setFutureSeconds(1);
    op->setArgs(std::vector<Atlas::Objects::Root>(1, Atlas::Objects::Operation::RootOperation()));

    TIME_ON;
    for (i = 0; i < 1000000; ++i) {
        Atlas::Message::MapType message;
        op->addToMessage(message);
        if (i == 0) {
            std::cout << "Message: " << message.size() << std::endl << std::flush;
        }
        Atlas::Message::MapType::const_iterator I = message.begin();
        Atlas::Message::MapType::const_iterator Iend = message.end();
        for (; I != Iend; ++I) {
            std::string key = I->first;
            Atlas::Message::Element val = I->second;
        }
    }
    TIME_OFF("Full Object to Message conversion and iteration");


    TIME_ON;
    for (i = 0; i < 1000000; ++i) {
        Atlas::Objects::Operation::RootOperation::const_iterator I = op->begin();
        Atlas::Objects::Operation::RootOperation::const_iterator Iend = op->end();
        for (; I != Iend; ++I) {
            std::string key = I->first;
            Atlas::Message::Element val = I->second;
        }
    }
    TIME_OFF("Full Object iteration");
}
