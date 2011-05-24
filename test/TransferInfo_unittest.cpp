// Eris Online RPG Protocol Library
// Copyright (C) 2011 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <Eris/TransferInfo.h>

#include <cassert>

static const char * TEST_HOST = "04e39dde-7ab7-4033-8eb7-838a8a1bfa60";
static const char * TEST_KEY = "e2a93894-b869-4b21-824a-9f8cc32faf9f";
static const char * TEST_ID = "47fb46d0-ca16-4c52-bbdb-c8f65c003212";

static const int TEST_PORT = 0x199d;

int main()
{
    {
        new Eris::TransferInfo("", 0, "", "");
    }

    {
        Eris::TransferInfo * ti = new Eris::TransferInfo("", 0, "", "");
        delete ti;
    }

    {
        Eris::TransferInfo * ti = new Eris::TransferInfo(TEST_HOST, 0, "", "");
        const std::string & host = ti->getHost();
        assert(host == TEST_HOST);
    }

    {
        Eris::TransferInfo * ti = new Eris::TransferInfo("", TEST_PORT, "", "");
        int port = ti->getPort();
        assert(port == TEST_PORT);
    }

    {
        Eris::TransferInfo * ti = new Eris::TransferInfo("", 0, TEST_KEY, "");
        const std::string & key = ti->getPossessKey();
        assert(key == TEST_KEY);
    }

    {
        Eris::TransferInfo * ti = new Eris::TransferInfo("", 0, "", TEST_ID);
        const std::string & id = ti->getPossessEntityId();
        assert(id == TEST_ID);
    }

    return 0;
}
