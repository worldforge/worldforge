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

#include <windows.h>



int main(int argc, char** argv)
{
	PackedAsciiTest* app = new PackedAsciiTest();

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
 

void PackedAsciiTest::walkTree(int nest, int names, AObject *list)
{
	int	i;
	char	buf[80];
	char	pre[80];

	*pre = 0;
	for (int j=0; j<nest; j++) {
		strcat(pre,"\t");
	}

	if (list->isList()) {
		// precheck types here
		if (names) printf("%s<list name=%s>\n",
			pre, list->getName()
		);
		if (!names) printf("%s<list>\n", pre);
		for (i=0; i<list->length(); i++) {
			walkTree(nest+1, 0, list->get(i));
		}
		printf("%s</list>\n",pre);
	} 
	if (list->isMap()) {
		AObject* keys = list->keys();
		if (names) printf("%s<map name=%s>\n",
			pre, list->getName()
		);
		if (!names) printf("%s<map>\n",	pre);
		for (i=0; i<keys->length(); i++) {
			AObject* key = keys->get(i);
			walkTree(nest+1, 1, list->get(key));
		}
		printf("%s</map>\n",pre);
	} 

	if (list->isString()) {
		if (names) printf("%s<str name=%s>%s</str>\n",
			pre, list->getName(),
			list->asString()
		);
		if (!names) printf("%s<str>%s</str>\n",
			pre, list->asString()
		);
	}
	if (list->isLong()) {
		if (names) printf("%s<int name=%s>%li</int>\n",
			pre, list->getName(),
			list->asLong()
		);
		if (!names) printf("%s<int>%li</int>\n",
			pre, list->asLong()
		);
	}
	if (list->isFloat()) {
		if (names) printf("%s<float name=%s>%.2f</float>\n",
			pre, list->getName(), list->asFloat()
		);
		if (!names) printf("%s<float>%.2f</float>\n",
			pre, list->asFloat()
		);
	}

	fflush(stdout);
}

void PackedAsciiTest::DisplayMessage(AObject *msg)
{
	int	i;

	AObject* keys = msg->keys();
	printf("<msg name=%s>\n", msg->getName());
	for (i=0; i<keys->length(); i++) {
		AObject* key = keys->get(i);
		walkTree(1, 1, msg->get(key));
	}
	printf("</msg>\n");
	fflush(stdout);
}



void PackedAsciiTest::execute()
{
	char* msg;

	Py_Initialize();

        AProtocol* prot = new APackedAsciiProtocol();

        AObject* test = AObject::mkMap("test1");
        AObject* list = AObject::mkList("list1",0);
        AObject* amap = AObject::mkMap("map1");

        list->append("stringval");
        list->append((long)12345);
        list->append(9876.54);

        test->set("astring",	"stringval");
        test->set("aint",	(long)12345);
        test->set("afloat",	9876.54);

        test->set(list);
        test->set(amap);

	printf("TEST len=%i\n", test->length());
	printf("LIST len=%i\n", list->length());

	DisplayMessage(test);

        AProtocolEncoder* encoder = prot->getEncoder();

        msg = strdup(encoder->encodeMessage(test));
        printf("Packed Message:\n%s\n\n", msg);

        ACodec* codec = new ACodec(prot);

        codec->feedStream(msg);
        int res = codec->hasMessage();
        printf("Result=%i\n", res);

        AObject* reply = codec->getMessage();

        DisplayMessage(reply);

        msg = strdup(encoder->encodeMessage(reply));
        printf("Packed Message:\n%s\n\n", msg);
}





