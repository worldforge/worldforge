// This file is distributed under the GNU Lesser General Public license.
// Copyright (C) 2000 Michael Day (See the file COPYING for details).

#include "Debug.h"

using std::string;

static int level = 0;
static FILE* logfile = 0;

void Atlas::Debug::setLevel(int l)
{
    level = l;
}

int Atlas::Debug::getLevel()
{
    return level;
}

void Atlas::Debug::openLog(const string& filename)
{
    closeLog();
    logfile = fopen(filename.c_str(), "a");
}

void Atlas::Debug::closeLog()
{
    if (logfile)
    {
	fclose(logfile);
    }
}

void Atlas::Debug::logmsg(const char* buf)
{
    if (logfile)
    {
	fprintf(logfile, "%s\n", buf);
	fflush(logfile);
    }
    
    printf("%s\n", buf);
}

