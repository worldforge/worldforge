/*
        XMLTest.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "XMLTest.h"

#include <AtlasTransport.h>
#include <string.h>
#include <stdio.h>



int main(int argc, char** argv)
{
	XMLTest* app = new XMLTest();

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

void XMLTest::walkTree(int nest, string name, const AObject& list)
{
}

void XMLTest::DisplayMessage(const AObject& msg)
{
}



void XMLTest::execute()
{
	string msg;

	initAtlasTransport();

        AProtocol* prot = new AXMLProtocol();

        AObject test = AObject::mkMap();
        AObject list = AObject::mkList(0);
        AObject amap = AObject::mkMap();

        list.append("stringval");
        list.append((long)12345);
        list.append(9876.54);

        test.set("astring",	"stringval");
        test.set("aint",	(long)12345);
        test.set("afloat",	9876.54);
        AObject uri=AObject::mkURI("foo");
        test.set("auri",	uri);
        AObject urilst=AObject::mkURIList(0);
        urilst.append("bar");
        test.set("aurilst",urilst);
        test.set("flst", AObject(2, 2.3, -0.2));
        test.set("ilst", AObject(3, 42, 0, -1));
        test.set("slst", AObject(1, &string("sd")));

        test.set("list1", list);
        test.set("map1", amap);

	printf("TEST len=%i\n", test.length());
	printf("LIST len=%i\n", list.length());
	fflush(stdout);
	
	AObject::dump(test);
	fflush(stdout);

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





