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

#include "PackedAsciiTest.h"

#include <string.h>
#include <stdio.h>


int main(int argc, char** argv)
{
	PackedAsciiTest* app = new PackedAsciiTest();

	ADebug::setDebug(5);

	app->execute();
}

#ifdef _WIN32
#include <windows.h>
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

void PackedAsciiTest::execute()
{
	string msg;

	Py_Initialize();

        AProtocol* prot = new APackedAsciiProtocol();

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

	printf("TEST len=%i\n", test.length());
	printf("LIST len=%i\n", list.length());
	fflush(stdout);
	
	AObject::dump(test);

        AProtocolEncoder* encoder = prot->getEncoder();

        printf("Packing Message\n");
	fflush(stdout);

        msg = encoder->encodeMessage(test);
        printf("Packed Message:\n%s\n\n", msg.c_str());
	fflush(stdout);

        ACodec* codec = new ACodec(prot);

        codec->feedStream(msg);
        int res = codec->hasMessage();
        printf("Result=%i\n", res);
	fflush(stdout);

        AObject reply = codec->getMessage();

        AObject::dump(reply);

        msg = encoder->encodeMessage(reply);
        printf("Packed Message:\n%s\n\n", msg.c_str());
}





