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
	initAtlasTransport();

	EchoTest* app = new EchoTest();

        //ADebug::setDebug(2);
        //ADebug::openLog("EchoTest.log");

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

void EchoTest::execute()
{
	long res, i;

        ASocket* sock = new ATCPSocket();

	printf("NEW SOCK = %i\n", sock->getSock());
		fflush(stdout);

	string servname("90.0.0.2");
        //string servname("127.0.0.1");

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

        for (i=0;i<10000;i++) {
                // printf("SEND = %li\n", i);
		test.set("count", i);
		client->sendMsg(test);
		client->doPoll();
	}

	for (i=1;i<50;i++) {
		client->doPoll();
	}
}

void EchoClient::gotMsg(const AObject& msg)
{
static	long	cnt = 0;
	long	val;

	cnt++;
        if ((cnt % 50) == 0) {
		AObject tmp;
		if (msg.has("count") == 1) {
			msg.get("count",tmp);
			val = tmp.asLong();
			printf("%li = %li\n", cnt, val);
		} else {
			printf("%li = NO COUNT !!\n", cnt);
		}
		fflush(stdout);
        }
}


