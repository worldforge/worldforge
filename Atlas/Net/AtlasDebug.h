/*
        AtlasDebug.h
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#ifndef __AtlasDebug_h_
#define __AtlasDebug_h_

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#ifdef _ADebug_
#define DebugMsg1(v,f,w)	if (v > ADebug::doDebug) ADebug::logmsg(f,w)
#define DebugMsg2(v,f,w,x)	if (v > ADebug::doDebug) ADebug::logmsg(f,w,x)
#define DebugMsg3(v,f,w,x,y)	if (v > ADebug::doDebug) ADebug::logmsg(f,w,x,y)
#define DebugMsg4(v,f,w,x,y,z)	if (v > ADebug::doDebug) ADebug::logmsg(f,w,x,y,z)
#else
#define DebugMsg1(v,f,w)	assert(true)
#define DebugMsg2(v,f,w,x)	assert(true)
#define DebugMsg3(v,f,w,x,y)	assert(true)
#define DebugMsg4(v,f,w,x,y,z)	assert(true)
#endif

class ADebug
{

private:

static	FILE*	logfile;	// log file to output

public:

static	int	doDebug;	// debug output switch

		ADebug();

static	void	openLog(char* fname);
static	void	closeLog();

static	void	setDebug(int level);
static	int	getDebug();

static	void	logmsg(char* fmt, ...);

};

#endif
