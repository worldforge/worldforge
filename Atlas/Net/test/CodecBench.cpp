/*
        PackedAsciiTest.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include <Python.h>
#include <Atlas/Net/Codec.h>
#include <Atlas/Object/Object.h>
//#include "Protocol.h"

#include <Atlas/Net/XMLProtocol.h>
#include <Atlas/Net/PackedProtocol.h>


#include "CodecBench.h"

#include <string>
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

	Atlas::Protocol*	proto;
        Atlas::Codec* 	codec;
/*
PAS Encode/Sec = 769.23
PAS Decode/Sec = 847.46
XML Encode/Sec = 657.89
XML Decode/Sec = 471.70
281.79user 0.18system 5:05.91elapsed 92%CPU (0avgtext+0avgdata 0maxresident)k

Old Python using one:
PAS Encode/Sec = 925.93
PAS Decode/Sec = 1000.00
XML Encode/Sec = 980.39
XML Decode/Sec = 609.76
226.69user 0.64system 3:57.14elapsed 95%CPU (0avgtext+0avgdata 0maxresident)k
*/

	Py_Initialize();

        Atlas::Object test = Atlas::Object::mkMap();
        Atlas::Object list = Atlas::Object::mkList(0);
        Atlas::Object amap = Atlas::Object::mkMap();

        string strval("stringval");
        list.append(strval);
        list.append((long)12345);
        list.append(9876.54);

        test.set("astring",	"stringval");
        test.set("aint",	(long)12345);
        test.set("afloat",	9876.54);

        test.set("list1", list);
        test.set("map1", amap);

	// packed ascii codec tests !!

	proto = new Atlas::PackedProtocol();
	codec = new Atlas::Codec(proto);

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

	proto = new Atlas::XMLProtocol();
	codec = new Atlas::Codec(proto);

	stim = time(NULL);

	for (i=0; i<50000; i++) {
	        msg = codec->encodeMessage(test);
	}

	enctime = time(NULL) - stim;

	encloop = 50000.0 / enctime;
	printf("XML Encode/Sec = %.2f\n", encloop);
	fflush(stdout);

	stim = time(NULL);

	for (i=0; i<50000; i++) {
   	codec->feedStream(msg);
   	codec->hasMessage();
		codec->freeMessage();
	}

	dectime = time(NULL) - stim;

	decloop = 50000.0 / dectime;
	printf("XML Decode/Sec = %.2f\n", decloop);
	fflush(stdout);

	delete codec;
	delete proto;
}






