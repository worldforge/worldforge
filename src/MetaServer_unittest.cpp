// WorldForge Next Generation MetaServer
// Copyright (C) 2011 Alistair Riddoch

#include "MetaServerHandlerTCP.hpp"
#include "MetaServerHandlerUDP.hpp"
#include "MetaServerPacket.hpp"

#include <cassert>

int main()
{
}

// stubs

// MetaServer won't currently link to this file as it contains the main function

std::string
MetaServerPacket::getAddress()
{
    return "";
}

unsigned int
MetaServerPacket::getPacketType()
{
}

void
MetaServerPacket::setPacketType(unsigned int)
{
}

void
MetaServerPacket::addPacketData(unsigned int)
{
}

void
MetaServerPacket::dumpBuffer()
{
}

MetaServerHandlerUDP::~MetaServerHandlerUDP()
{
}

MetaServerHandlerTCP::~MetaServerHandlerTCP()
{
}
