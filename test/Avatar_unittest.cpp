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

#include <Eris/Avatar.h>

#include <Eris/Connection.h>
#include <Eris/Account.h>
#include <Eris/Log.h>

#include <iostream>

static void writeLog(Eris::LogLevel, const std::string & msg)
{       
    std::cerr << msg << std::endl << std::flush;
}

class TestAvatar : public Eris::Avatar {
  public:
    TestAvatar(Eris::Account * ac, const std::string & ent_id) :
               Eris::Avatar(ac, ent_id) { }
};


int main()
{
    Eris::Logged.connect(sigc::ptr_fun(writeLog));
    Eris::setLogLevel(Eris::LOG_DEBUG);

    {
        Eris::Connection * con = new Eris::Connection("name", "localhost",
                                                      6767, true);

        Eris::Account * acc = new Eris::Account(con);
        std::string fake_char_id("1");
        Eris::Avatar * ea = new TestAvatar(acc, fake_char_id);
    }


    return 0;
}
