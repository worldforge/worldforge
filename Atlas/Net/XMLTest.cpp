/*
        XMLTest.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "XMLTest.h"

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
}
#endif

void XMLTest::walkTree(int nest, string name, AObject& list)
{
	int	i;
	string	buf;
	string	pre;

	for (int j=0; j<nest; j++) {
		pre.append("    ");
	}

	if (list.isList()) {
		// precheck types here
		if (name.length() > 0) {
			printf("%s<list name=\"%s\">\n", pre.c_str(), name.c_str());
		} else {
			printf("%s<list>\n", pre.c_str());
		}
		for (i=0; i<list.length(); i++) {
			AObject tmp;
			//printf("******* get node %i\n", i);
			fflush(stdout);
			list.get(i, tmp);
			//printf("******* walk node %i\n", i);
			walkTree(nest+1, "", tmp);
		}
		printf("%s</list>\n",pre.c_str());
	} 
	if (list.isMap()) {
		AObject keys = list.keys();
		if (name.length() > 0) {
			printf("%s<map name=\"%s\">\n",pre.c_str(), name.c_str());
		} else {
			printf("%s<map>\n", pre.c_str());
		}
		for (i=0; i<keys.length(); i++) {
			AObject key;
			//printf("******* get node %i\n", i);
			fflush(stdout);
			keys.get(i, key);
			//printf("******* get key %s\n", key.asString().c_str());
			fflush(stdout);
			AObject tmp;
			list.get(key.asString(), tmp);
			walkTree(nest+1, key.asString(), tmp);
		}
		printf("%s</map>\n",pre.c_str());
	} 

	if (list.isString()) {
		if (name.length() > 0) {
			printf("%s<str name=\"%s\">%s</str>\n",
				pre.c_str(), name.c_str(),list.asString().c_str()
			);
		} else {
			printf("%s<str>%s</str>\n",pre.c_str(), list.asString().c_str());
		}
	}
	if (list.isLong()) {
		if (name.length() > 0) {
			printf("%s<int name=\"%s\">%li</int>\n",
				pre.c_str(), name.c_str(),list.asLong()
			);
		} else {
			printf("%s<int>%li</int>\n",pre.c_str(), list.asLong());
		}
	}
	if (list.isFloat()) {
		if (name.length() > 0) {
			printf("%s<float name=\"%s\">%.2f</float>\n",
				pre.c_str(), name.c_str(),list.asFloat()
			);
		} else {
			printf("%s<float>%.2f</float>\n",pre.c_str(), list.asFloat());
		}
	}

}

void XMLTest::DisplayMessage(AObject& msg)
{
	int	i;

	AObject keys = msg.keys();
	printf("<obj>\n");
	walkTree(1, "", msg);
	printf("</obj>\n");
	fflush(stdout);
}



void XMLTest::execute()
{
	string msg;

	Py_Initialize();

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

        test.set("list1", list);
        test.set("map1", amap);

	printf("TEST len=%i\n", test.length());
	printf("LIST len=%i\n", list.length());
	fflush(stdout);
	
	DisplayMessage(test);

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

        DisplayMessage(reply);

        msg = encoder->encodeMessage(reply);
        printf("Packed Message:\n%s\n\n", msg.c_str());
}





