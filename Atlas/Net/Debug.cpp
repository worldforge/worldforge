/*
        AtlasDebug.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "AtlasDebug.h"

int 	ADebug::doDebug = 0;
FILE*	ADebug::logfile = NULL;

	ADebug::ADebug()		{}

void	ADebug::setDebug(int level)	{ doDebug = level; }
int	ADebug::getDebug()		{ return doDebug; }

void	ADebug::openLog(char* fname)
{
	ADebug::closeLog();
	logfile = fopen(fname, "a");
}

void	ADebug::closeLog()
{
	if (logfile != NULL) {
		fclose(logfile);
	}
}

void	ADebug::logmsg(char* fmt, ...)
{
	char	buf[1024];
	va_list	va;

	va = va_start(va,fmt);
	vsprintf(buf, fmt, va);
	va_end(va);

	if (logfile != NULL) {
		fprintf(logfile, "%s\n", buf);
		fflush(logfile);
	}
	fprintf(stdout, "%s\n", buf);
	fflush(stdout);
}


