/*
        SocketTest.cpp
        ----------------
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com
*/

#include "SocketTest.h"

int main(int argc, char** argv)
{
	SocketTest* app = new SocketTest();

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
}
#endif
 
void SocketTest::execute()
{
	int res;

	Py_Initialize();

        ASocket* sock = new ATCPSocket();

	printf("NEW SOCK = %i\n", sock->getSock());
		fflush(stdout);

	string servname("www.worldforge.org");

	res = sock->connect(servname, 80);
	printf("Connect = %i\n", res);
	fflush(stdout);
	if (res == -1) exit(0);

	string command("GET /\n\n");
	res = sock->send(command);
	printf("send = %i\n", res);
	fflush(stdout);

	string	buf;

	while (sock->recv(buf) > 0)
	{
		printf("%s", buf.c_str());
	}
	sock->close();
}


