// Eris - The WorldForge client entity library
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

#include <Eris/Connection.h>
#include <Eris/Log.h>
#include <Eris/PollDefault.h>

#include <iostream>

#include <cstdlib>
#include <unistd.h>

#include <getopt.h>

bool done = false;

static void usage(const char * prgname)
{
    std::cout << "usage: " << prgname << " [-n] host [port]"
              << std::endl << std::flush;
}

static void erisLog(Eris::LogLevel level, const std::string & msg)
{
    std::cout << "LOG: " << msg << std::endl << std::flush;
}

static void onConnected()
{
    std::cout << "connected to server" << std::endl;
    done = true;
}

static void onConnectionFail(const std::string& errMsg)
{
    std::cout << "failed to connect to server: " << errMsg << std::endl;
    done = true;
}

int main(int argc, char ** argv)
{
    bool option_verbose = false;
    bool option_nonblock = false;

    while (1) {
        int c = getopt(argc, argv, "nv");
        if (c == -1) {
            break;
        } else if (c == '?') {
            usage(argv[0]);
            return 1;
        } else if (c == 'v') {
            option_verbose = true;
        } else if (c == 'n') {
            option_nonblock = true;
        }
    }

    if (option_verbose && option_nonblock) {
        std::cerr << "Connecting non blocking"
                  << std::endl << std::flush;
    }

    int arg_left = argc - optind;

    if (arg_left < 1 || arg_left > 2) {
        usage(argv[0]);
        return 1;
    }

    int port = 6767;

    if (arg_left == 2) {
        port = strtol(argv[optind + 1], 0, 10);
    }

    Eris::Logged.connect(sigc::ptr_fun(erisLog));
    Eris::setLogLevel(Eris::LOG_DEBUG);

    Eris::Connection * c = new Eris::Connection("test", argv[optind], port, false);

    c->Connected.connect(sigc::ptr_fun(onConnected));
    c->Failure.connect(sigc::ptr_fun(onConnectionFail));

    c->connect();

    while (!done) {
        Eris::PollDefault::poll();
        usleep(10000);
    }

    return 0;
}
