/*
        EchoTest.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "EchoTest.h"

int main(int argc, char** argv)
{
	EchoTest* app = new EchoTest();

	ADebug::setDebug(2);
	ADebug::openLog("EchoTest.log");

	app->execute();

	return 0;	
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



void walkTree(int nest, string name, AObject& list)
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

void DisplayMessage(AObject& msg)
{
	printf("<obj>\n");
	walkTree(1, "", msg);
	printf("</obj>\n");
	fflush(stdout);
}






void EchoTest::execute()
{
	long res, i;

	Py_Initialize();

        ASocket* sock = new ATCPSocket();

	printf("NEW SOCK = %i\n", sock->getSock());
		fflush(stdout);

	//string servname("90.0.0.2");
        string servname("127.0.0.1");

	res = sock->connect(servname, 7);
	printf("Connect = %li\n", res);
	fflush(stdout);
	if (res == -1) exit(0);

	ACodec* codec = new ACodec(new APackedAsciiProtocol());

	AClient* client = new EchoClient(sock, codec);

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

	for (i=0;i<100;i++) {
		printf("SEND = %li\n", i);
		test.set("count", i);
		client->sendMsg(test);
		client->doPoll();
	}

	for (i=1;i<50;i++) {
		client->doPoll();
	}
}

void EchoClient::gotMsg(AObject& msg)
{
static	long	cnt = 0;
	long	val;

	cnt++;
	DisplayMessage(msg);
	//if ((cnt % 50) == 0) {
		AObject tmp;
		if (msg.has("count") == 1) {
			msg.get("count",tmp);
			val = tmp.asLong();
			printf("%li = %li\n", cnt, val);
		} else {
			printf("%li = NO COUNT !!\n", cnt);
		}
		fflush(stdout);
	//}
}


