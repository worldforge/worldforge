/*
        PackedAsciiTest.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "AtlasCodec.h"
#include "AtlasObject.h"
#include "AtlasProtocol.h"

#include "AtlasXMLProtocol.h"
#include "AtlasPackedAsciiProtocol.h"


#include "CodecBench.h"

#include <string.h>
#include <stdio.h>

#ifdef	_WIN32
#include <windows.h>
#endif

#include <time.h>

int main(int argc, char** argv)
{
	CodecBench* app = new CodecBench();

	app->execute();
}

#ifdef _WIN32
int WINAPI WinMain( 
	HINSTANCE hInstance, 	  // handle to current instance 
	HINSTANCE hPrevInstance,  // handle to previous instance 
	LPSTR lpCmdLine,	  // pointer to command line 
	int nCmdShow		  // show state of window 
) {
	main(0,NULL);
	return 0;
}
#endif

void CodecBench::execute()
{
	int	i;
	string	msg;
	time_t	stim,enctime,dectime;
	double	encloop,decloop;

	AProtocol*	proto;
        ACodec* 	codec;

	Py_Initialize();

        AObject test = AObject::mkMap();
        AObject list = AObject::mkList(0);
        AObject amap = AObject::mkMap();

        list.append("stringval");
        list.append((long)12345);
        list.append(9876.54);

        test.set("astring",	"stringval");
        test.set("aint",	(long)12345);
        test.set("afloat",	9876.54);

        test.set("list1", list);
        test.set("map1", amap);

	// packed ascii codec tests !!

	proto = new APackedAsciiProtocol();
	codec = new ACodec(proto);

	stim = time(NULL);

	for (i=0; i<50000; i++) {
	        msg = codec->encodeMessage(test);
	}

	enctime = time(NULL) - stim;

	encloop = 50000.0 / enctime;
	printf("PAS Encode/Sec = %.2f\n", encloop);
	fflush(stdout);

	stim = time(NULL);

	for (i=0; i<50000; i++) {
	   codec->feedStream(msg);
		codec->hasMessage();
		codec->freeMessage();
	}

	dectime = time(NULL) - stim;

	decloop = 50000.0 / dectime;
	printf("PAS Decode/Sec = %.2f\n", decloop);
	fflush(stdout);

	delete codec;
	delete proto;

	// XML codec tests !!

	proto = new AXMLProtocol();
	codec = new ACodec(proto);

	stim = time(NULL);

	for (i=0; i<50000; i++) {
	        msg = codec->encodeMessage(test);
	}

	enctime = time(NULL) - stim;

	encloop = 50000.0 / enctime;
	printf("PAS Encode/Sec = %.2f\n", encloop);
	fflush(stdout);

	stim = time(NULL);

	for (i=0; i<50000; i++) {
   	codec->feedStream(msg);
   	codec->hasMessage();
		codec->freeMessage();
	}

	dectime = time(NULL) - stim;

	decloop = 50000.0 / dectime;
	printf("PAS Decode/Sec = %.2f\n", decloop);
	fflush(stdout);

	delete codec;
	delete proto;
}






