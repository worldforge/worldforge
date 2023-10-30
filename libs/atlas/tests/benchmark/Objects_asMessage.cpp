#include "timer.h"

#include <Atlas/Objects/RootOperation.h>

#include <iostream>

int main(int argc, char ** argv)
{
    Atlas::Objects::Operation::RootOperation op;
    Atlas::Message::Element message = Atlas::Message::MapType();

    TIME_ON;
    int i;
    for (i = 0; i < 1000000; ++i) {
        op->addToMessage(message.asMap());
    }
    TIME_OFF("Object to Message conversions");
}
