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
#include "AtlasPackedAsciiProtocol.h"



#include "CodecBench.h"

#include <string.h>
#include <stdio.h>

#include <windows.h>

#include <time.h>

int main(int argc, char** argv)
{
	CodecBench* app = new CodecBench();

	app->execute();
}

int WINAPI WinMain( 
	HINSTANCE hInstance, 	  // handle to current instance 
	HINSTANCE hPrevInstance,  // handle to previous instance 
	LPSTR lpCmdLine,	  // pointer to command line 
	int nCmdShow		  // show state of window 
) {
	main(0,NULL);
}
 
void CodecBench::execute()
{
	string msg;

	Py_Initialize();

	AProtocol*	proto = new APackedAsciiProtocol();
        ACodec* 	codec = new ACodec(proto);

        AObject test = AObject::mkMap("test1");
        AObject list = AObject::mkList("list1",1);
        AObject amap = AObject::mkMap("map1");

        list.append("stringval");
        list.append((long)12345);
        list.append(9876.54);

        test.set("astring",	"stringval");
        test.set("aint",	(long)12345);
        test.set("afloat",	9876.54);

        test.set(list);
        test.set(amap);

	time_t stim = time(NULL);

	for (int i=0; i<50; i++) {
		printf("encode\n",i);
		fflush(stdout);
	        msg = codec->encodeMessage(test);
		printf("feed stream\n",i);
		fflush(stdout);
	        codec->feedStream(msg);
		printf("check for msg\n",i);
		fflush(stdout);
	        int res = codec->hasMessage();
		codec->freeMessage();
	}

	time_t tim = (time(NULL) - stim);

	double perloop = 1 / (tim / 50.0);

	printf("Codecs/Sec = %.2f\n", perloop);

}





