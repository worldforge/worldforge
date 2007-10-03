// Eris Online RPG Protocol Library
// Copyright (C) 2007 Alistair Riddoch
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

#include <Eris/Account.h>

#include <Eris/Connection.h>
#include <Eris/Exceptions.h>

int main()
{
    // Test constructor
    {
        Eris::Connection * con = new Eris::Connection("name", "localhost",
                                                      6767, true);

        Eris::Account acc(con);
    }
    // Test constructor throws on a null connection.
    {
        try {
            Eris::Account acc(0);
            abort();
        }
        catch (Eris::InvalidOperation & eio) {
        }
    }
    // Test getActiveCharacters()
    {
        Eris::Connection * con = new Eris::Connection("name", "localhost",
                                                      6767, true);

        Eris::Account acc(con);

        const Eris::ActiveCharacterMap & acm = acc.getActiveCharacters();

        assert(acm.empty());
    }
    // Test getId()
    {
        Eris::Connection * con = new Eris::Connection("name", "localhost",
                                                      6767, true);

        Eris::Account acc(con);

        const std::string & id = acc.getId();

        assert(id.empty());
    }
    // Test getUsername()
    {
        Eris::Connection * con = new Eris::Connection("name", "localhost",
                                                      6767, true);

        Eris::Account acc(con);

        const std::string & username = acc.getUsername();

        assert(username.empty());
    }

    // Test getConnection()
    {
        Eris::Connection * con = new Eris::Connection("name", "localhost",
                                                      6767, true);

        Eris::Account acc(con);

        Eris::Connection * got_con = acc.getConnection();

        assert(got_con == con);
    }

    return 0;
}
